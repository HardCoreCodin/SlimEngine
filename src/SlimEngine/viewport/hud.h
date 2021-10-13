#pragma once

#include "../core/base.h"
#include "../core/text.h"

void drawHUD(Viewport *viewport, HUD *hud) {
    u16 x = (u16)hud->position.x;
    u16 y = (u16)hud->position.y;

    HUDLine *line = hud->lines;
    bool alt;
    for (u32 i = 0; i < hud->line_count; i++, line++) {
        if (line->use_alternate) {
            alt = *line->use_alternate;
            if (line->invert_alternate_use)
                alt = !alt;
        } else
            alt = false;

        drawText(line->title.char_ptr, x, y, Color(line->title_color), 1, viewport);
        drawText(alt ? line->alternate_value.char_ptr : line->value.string.char_ptr,
                 x + (u16)line->title.length * FONT_WIDTH, y,
                 Color(alt ? line->alternate_value_color : line->value_color), 1, viewport);
        y += (u16)(hud->line_height * (f32)FONT_HEIGHT);
    }
}