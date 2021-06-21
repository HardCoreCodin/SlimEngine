#pragma once

#include "../core/base.h"
#include "../core/time.h"

#define HUD_COLOR 0x0000FF00
#define HUD_LENGTH 140
#define HUD_WIDTH 12
#define HUD_RIGHT 100
#define HUD_TOP 10

typedef struct HUD {
    char text[HUD_LENGTH];
    char *width,
         *height,
         *mouseX,
         *mouseY,
         *run_on,
         *fps,
         *msf,
         *mode;
} HUD;

void setCountersInHUD(HUD *hud, Timer *timer) {
    NumberStringBuffer fps_number_string;
    NumberStringBuffer msf_number_string;

    printNumberIntoString(timer->average_frames_per_second,      &fps_number_string);
    printNumberIntoString(timer->average_microseconds_per_frame, &msf_number_string);

    for (u8 i = 0; i < 4; i++) {
        hud->fps[i] = fps_number_string.string[i];
        hud->msf[i] = msf_number_string.string[i];
    }
}

void setDimensionsInHUD(HUD *hud, u16 width, u16 height) {
    NumberStringBuffer width_number_string;
    NumberStringBuffer height_number_string;

    printNumberIntoString(width, &width_number_string);
    printNumberIntoString(height, &height_number_string);

    for (u8 i = 0; i < 4; i++) {
        hud->width[i] = width_number_string.string[i];
        hud->height[i] = height_number_string.string[i];
    }
}

void setMouseCoordinatesInHUD(HUD *hud, Mouse *mouse) {
    NumberStringBuffer mouse_pos_x_number_string;
    NumberStringBuffer mouse_pos_y_number_string;

    printNumberIntoString(mouse->pos.x, &mouse_pos_x_number_string);
    printNumberIntoString(mouse->pos.y, &mouse_pos_y_number_string);

    for (u8 i = 0; i < 4; i++) {
        hud->mouseX[i] = mouse_pos_x_number_string.string[i];
        hud->mouseY[i] = mouse_pos_y_number_string.string[i];
    }
}

void initHUD(HUD *hud, u16 width, u16 height) {
    char* str_template = (char*)"Width  : ___1\n"
                         "Height : ___2\n"
                         "Mouse X: ___3\n"
                         "Mouse Y: ___4\n"
                         "Using  :  5__\n"
                         "FPS    : ___6\n"
                         "mic-s/f: ___7\n";

    char* HUD_char = str_template;
    char* HUD_text_char = hud->text;

    while (*HUD_char) {
        switch (*HUD_char) {
            case '1':  hud->width  = HUD_text_char; break;
            case '2':  hud->height = HUD_text_char; break;
            case '3':  hud->mouseX = HUD_text_char; break;
            case '4':  hud->mouseY = HUD_text_char; break;
            case '5':  hud->run_on = HUD_text_char; break;
            case '6':  hud->fps    = HUD_text_char; break;
            case '7':  hud->msf    = HUD_text_char; break;
            case '8':  hud->mode   = HUD_text_char; break;
        }

        *HUD_text_char++ = *HUD_char++;
    }
    *HUD_text_char = '\0';

    setDimensionsInHUD(hud, width, height);
}