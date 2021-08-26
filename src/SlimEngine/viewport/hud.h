#pragma once

#include "../core/base.h"
#include "../core/text.h"

void drawHUD(PixelGrid *canvas, HUD *hud) {
    i32 x = hud->position.x;
    i32 y = hud->position.y;

    HUDLine *line = hud->lines;
    bool alt;
    for (u32 i = 0; i < hud->line_count; i++, line++) {
        if (line->use_alternate) {
            alt = *line->use_alternate;
            if (line->invert_alternate_use)
                alt = !alt;
        } else
            alt = false;

        drawText(canvas, ColorOf(line->title_color), line->title.char_ptr, x, y);
        drawText(canvas, ColorOf(
                         alt ? line->alternate_value_color : line->value_color),
                 alt ? line->alternate_value.char_ptr : line->value.string.char_ptr,
                 x + line->title.length * FONT_WIDTH, y);
        y += (i32)(hud->line_height * (f32)FONT_HEIGHT);
    }
}