#pragma once

#include "../core/base.h"
#include "../core/time.h"

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

void initHUD(HUD *hud, u16 width, u16 height) {
    hud->position.x = hud->position.y = 10;
    setDimensionsInHUD(hud, width, height);
    printNumberIntoString(0, &hud->mouseX);
    printNumberIntoString(0, &hud->mouseY);
    printNumberIntoString(0, &hud->fps);
    printNumberIntoString(0, &hud->msf);
}