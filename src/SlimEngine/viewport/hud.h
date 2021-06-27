#pragma once

#include "../core/base.h"
#include "../core/text.h"

void drawHUD(PixelGrid *canvas, HUD *hud) {
    i32 x = hud->position.x;
    i32 y = hud->position.y;

    HUDLine *line = hud->lines;
    for (u32 i = 0; i < hud->line_count; i++, line++) {
        drawText(canvas, Color(line->title_color), line->title.char_ptr, x, y);
        drawText(canvas, Color(line->value_color), line->value.string.char_ptr, x + line->title.length * FONT_WIDTH, y);
        y += (i32)(hud->line_height * (f32)FONT_HEIGHT);
    }
}