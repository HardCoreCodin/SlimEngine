#pragma once

#include "../core/base.h"
#include "./line.h"

INLINE void drawRect(Rect rect, vec3 color, f32 opacity, Viewport *viewport) {
    if (rect.max.x < 0 || rect.min.x >= viewport->dimensions.width ||
        rect.max.y < 0 || rect.min.y >= viewport->dimensions.height)
        return;

    drawHLine(rect.min.x, rect.max.x, rect.min.y, color, opacity, viewport);
    drawHLine(rect.min.x, rect.max.x, rect.max.y, color, opacity, viewport);
    drawVLine(rect.min.y, rect.max.y, rect.min.x, color, opacity, viewport);
    drawVLine(rect.min.y, rect.max.y, rect.max.x, color, opacity, viewport);
}

INLINE void fillRect(Rect rect, vec3 color, f32 opacity, Viewport *viewport) {
    if (rect.max.x < 0 || rect.min.x >= viewport->dimensions.width ||
        rect.max.y < 0 || rect.min.y >= viewport->dimensions.height)
        return;

    i32 min_x, min_y, max_x, max_y;
    subRange(rect.min.x, rect.max.x, viewport->dimensions.width,  0, &min_x, &max_x);
    subRange(rect.min.y, rect.max.y, viewport->dimensions.height, 0, &min_y, &max_y);
    for (i32 y = min_y; y <= max_y; y++)
        drawHLine(min_x, max_x, y, color, opacity, viewport);
}