#pragma once

typedef u64 (*GetTicks)();

typedef struct PerTick {
    f64 seconds, milliseconds, microseconds, nanoseconds;
} PerTick;

typedef struct Ticks {
    PerTick per_tick;
    u64 per_second;
} Ticks;

typedef struct Timer {
    GetTicks getTicks;
    Ticks *ticks;

    f32 delta_time;
    u64 ticks_before,
        ticks_after,
        ticks_diff,
        accumulated_ticks,
        accumulated_frame_count,
        ticks_of_last_report,
        seconds,
        milliseconds,
        microseconds,
        nanoseconds;
    f64 average_frames_per_tick,
        average_ticks_per_frame;
    u16 average_frames_per_second,
        average_milliseconds_per_frame,
        average_microseconds_per_frame,
        average_nanoseconds_per_frame;
} Timer;

void initTimer(Timer *timer, GetTicks getTicks, Ticks *ticks) {
    timer->getTicks = getTicks;
    timer->ticks    = ticks;

    timer->delta_time = 0;
    timer->ticks_before = 0;
    timer->ticks_after = 0;
    timer->ticks_diff = 0;

    timer->accumulated_ticks = 0;
    timer->accumulated_frame_count = 0;

    timer->ticks_of_last_report = 0;

    timer->seconds = 0;
    timer->milliseconds = 0;
    timer->microseconds = 0;
    timer->nanoseconds = 0;

    timer->average_frames_per_tick = 0;
    timer->average_ticks_per_frame = 0;
    timer->average_frames_per_second = 0;
    timer->average_milliseconds_per_frame = 0;
    timer->average_microseconds_per_frame = 0;
    timer->average_nanoseconds_per_frame = 0;
}

typedef struct Timers {
    Timer update, render, aux;
} Timers;

typedef struct Time {
    Timers timers;
    Ticks ticks;
    GetTicks getTicks;
} Time;

void initTime(Time *time, GetTicks getTicks, u64 ticks_per_second) {
    time->getTicks = getTicks;
    time->ticks.per_second = ticks_per_second;

    time->ticks.per_tick.seconds      = 1          / (f64)(time->ticks.per_second);
    time->ticks.per_tick.milliseconds = 1000       / (f64)(time->ticks.per_second);
    time->ticks.per_tick.microseconds = 1000000    / (f64)(time->ticks.per_second);
    time->ticks.per_tick.nanoseconds  = 1000000000 / (f64)(time->ticks.per_second);

    initTimer(&time->timers.update, getTicks, &time->ticks);
    initTimer(&time->timers.render, getTicks, &time->ticks);
    initTimer(&time->timers.aux,    getTicks, &time->ticks);

    time->timers.update.ticks_before = time->timers.update.ticks_of_last_report = getTicks();
}

void accumulateTimer(Timer* timer) {
    timer->ticks_diff = timer->ticks_after - timer->ticks_before;
    timer->accumulated_ticks += timer->ticks_diff;
    timer->accumulated_frame_count++;

    timer->seconds      = (u64)(timer->ticks->per_tick.seconds      * (f64)(timer->ticks_diff));
    timer->milliseconds = (u64)(timer->ticks->per_tick.milliseconds * (f64)(timer->ticks_diff));
    timer->microseconds = (u64)(timer->ticks->per_tick.microseconds * (f64)(timer->ticks_diff));
    timer->nanoseconds  = (u64)(timer->ticks->per_tick.nanoseconds  * (f64)(timer->ticks_diff));
}

void averageTimer(Timer *timer) {
    timer->average_frames_per_tick = (f64)timer->accumulated_frame_count / timer->accumulated_ticks;
    timer->average_ticks_per_frame = (f64)timer->accumulated_ticks / timer->accumulated_frame_count;
    timer->average_frames_per_second = (u16)(timer->average_frames_per_tick      * timer->ticks->per_second);
    timer->average_milliseconds_per_frame = (u16)(timer->average_ticks_per_frame * timer->ticks->per_tick.milliseconds);
    timer->average_microseconds_per_frame = (u16)(timer->average_ticks_per_frame * timer->ticks->per_tick.microseconds);
    timer->average_nanoseconds_per_frame = (u16)(timer->average_ticks_per_frame  * timer->ticks->per_tick.nanoseconds);
    timer->accumulated_ticks = timer->accumulated_frame_count = 0;
}

inline void startFrameTimer(Timer *timer) {
    timer->ticks_after = timer->ticks_before;
    timer->ticks_before = timer->getTicks();
    timer->ticks_diff = timer->ticks_before - timer->ticks_after;
    timer->delta_time = (f32)(timer->ticks_diff * timer->ticks->per_tick.seconds);
}

inline void endFrameTimer(Timer *timer) {
    timer->ticks_after = timer->getTicks();
    accumulateTimer(timer);
    if (timer->accumulated_ticks >= timer->ticks->per_tick.seconds) averageTimer(timer);
}