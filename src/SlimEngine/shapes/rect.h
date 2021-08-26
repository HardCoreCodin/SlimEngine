#pragma once

#include "../core/base.h"
#include "./line.h"

void drawRect(PixelGrid *canvas, RGBA color, Rect *rect) {
    if (rect->max.x < 0 || rect->min.x >= canvas->dimensions.width ||
        rect->max.y < 0 || rect->min.y >= canvas->dimensions.height)
        return;

    drawHLine(canvas, color, rect->min.x, rect->max.x, rect->min.y);
    drawHLine(canvas, color, rect->min.x, rect->max.x, rect->max.y);
    drawVLine(canvas, color, rect->min.y, rect->max.y, rect->min.x);
    drawVLine(canvas, color, rect->min.y, rect->max.y, rect->max.x);
}
void drawRectF(PixelGrid *canvas, vec3 color, f32 opacity, Rect *rect) {
    if (rect->max.x < 0 || rect->min.x >= canvas->dimensions.width ||
        rect->max.y < 0 || rect->min.y >= canvas->dimensions.height)
        return;

    drawHLineF(canvas, color, opacity, rect->min.x, rect->max.x, rect->min.y);
    drawHLineF(canvas, color, opacity, rect->min.x, rect->max.x, rect->max.y);
    drawVLineF(canvas, color, opacity, rect->min.y, rect->max.y, rect->min.x);
    drawVLineF(canvas, color, opacity, rect->min.y, rect->max.y, rect->max.x);
}

void fillRect(PixelGrid *canvas, RGBA color, Rect *rect) {
    if (rect->max.x < 0 || rect->min.x >= canvas->dimensions.width ||
        rect->max.y < 0 || rect->min.y >= canvas->dimensions.height)
        return;

    i32 min_x, min_y, max_x, max_y;
    subRange(rect->min.x, rect->max.x, canvas->dimensions.width,  0, &min_x, &max_x);
    subRange(rect->min.y, rect->max.y, canvas->dimensions.height, 0, &min_y, &max_y);
    for (u16 y = min_y; y <= max_y; y++)
        drawHLine(canvas, color, min_x, max_x, y);
}
void fillRectF(PixelGrid *canvas, vec3 color, f32 opacity, Rect *rect) {
    if (rect->max.x < 0 || rect->min.x >= canvas->dimensions.width ||
        rect->max.y < 0 || rect->min.y >= canvas->dimensions.height)
        return;

    i32 min_x, min_y, max_x, max_y;
    subRange(rect->min.x, rect->max.x, canvas->dimensions.width,  0, &min_x, &max_x);
    subRange(rect->min.y, rect->max.y, canvas->dimensions.height, 0, &min_y, &max_y);
    for (u16 y = min_y; y <= max_y; y++)
        drawHLineF(canvas, color, opacity, min_x, max_x, y);
}