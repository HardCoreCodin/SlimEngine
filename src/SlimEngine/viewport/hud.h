#pragma once

#include "../core/base.h"
#include "../core/text.h"

static char *HUD_fps    = "Fps    :";
static char *HUD_msf    = "mic-s/f:";

static char *HUD_width  = "Width  :";
static char *HUD_height = "Height :";

static char *HUD_mouseX = "Mouse X:";
static char *HUD_mouseY = "Mouse Y:";

void setCountersInHUD(HUD *hud, Timer *timer) {
    printNumberIntoString(timer->average_frames_per_second,      &hud->fps);
    printNumberIntoString(timer->average_microseconds_per_frame, &hud->msf);
}

void setDimensionsInHUD(HUD *hud, u16 width, u16 height) {
    printNumberIntoString(width, &hud->width);
    printNumberIntoString(height, &hud->height);
}

void setMouseCoordinatesInHUD(HUD *hud, Mouse *mouse) {
    printNumberIntoString(mouse->pos.x, &hud->mouseX);
    printNumberIntoString(mouse->pos.y, &hud->mouseY);
}

void drawHUD(PixelGrid *canvas, RGBA color, HUD *hud) {
    i32 x = hud->position.x;
    i32 y = hud->position.y;

    i32 small_gap = (i32)((f32)FONT_HEIGHT * 1.2f);
    i32 large_gap = FONT_HEIGHT + FONT_HEIGHT;

    drawText(canvas, color, HUD_fps, x, y);
    drawText(canvas, color, hud->fps.string, x + FONT_WIDTH * 9, y);

    y += small_gap;

    drawText(canvas, color, HUD_msf, x, y);
    drawText(canvas, color, hud->msf.string, x + FONT_WIDTH * 9, y);

    y += large_gap;

    drawText(canvas, color, HUD_width, x, y);
    drawText(canvas, color, hud->width.string, x + FONT_WIDTH * 9, y);

    y += small_gap;

    drawText(canvas, color, HUD_height, x, y);
    drawText(canvas, color, hud->height.string, x + FONT_WIDTH * 9, y);

    y += large_gap;

    drawText(canvas, color, HUD_mouseX, x, y);
    drawText(canvas, color, hud->mouseX.string, x + FONT_WIDTH * 9, y);

    y += small_gap;

    drawText(canvas, color, HUD_mouseY, x, y);
    drawText(canvas, color, hud->mouseY.string, x + FONT_WIDTH * 9, y);
}