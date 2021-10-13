#pragma once

#include "../math/vec3.h"
#include "./hud.h"

typedef union RGBA2u32 {
    RGBA rgba;
    u32 value;
} RGBA2u32;

void drawViewportToWindowContent(Viewport *viewport) {
    PixelQuad *src_pixel = viewport->pixels;
    u32 *trg_value = app->window_content;
    vec3 color;
    RGBA2u32 background, trg_pixel;
    background.rgba.R = (u8)(viewport->settings.background.color.r * FLOAT_TO_COLOR_COMPONENT);
    background.rgba.G = (u8)(viewport->settings.background.color.g * FLOAT_TO_COLOR_COMPONENT);
    background.rgba.B = (u8)(viewport->settings.background.color.b * FLOAT_TO_COLOR_COMPONENT);
    background.rgba.A = (u8)(viewport->settings.background.opacity * FLOAT_TO_COLOR_COMPONENT);
    if (viewport->settings.antialias) {
        for (u16 y = 0; y < viewport->dimensions.height; y++) {
            for (u16 x = 0; x < viewport->dimensions.width; x++, src_pixel++, trg_value++) {
                if (src_pixel->TL.opacity || src_pixel->TR.opacity || src_pixel->BL.opacity || src_pixel->BR.opacity) {
                    color = scaleVec3(src_pixel->TL.color, src_pixel->TL.opacity * 0.25f);
                    color = scaleAddVec3(src_pixel->TR.color, src_pixel->TR.opacity * 0.25f, color);
                    color = scaleAddVec3(src_pixel->BL.color, src_pixel->BL.opacity * 0.25f, color);
                    color = scaleAddVec3(src_pixel->BR.color, src_pixel->BR.opacity * 0.25f, color);
                    trg_pixel.rgba.R = (u8)(color.r > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.r));
                    trg_pixel.rgba.G = (u8)(color.g > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.g));
                    trg_pixel.rgba.B = (u8)(color.b > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.b));
                    trg_pixel.rgba.A = (u8)(clampValue(src_pixel->TL.opacity) * FLOAT_TO_COLOR_COMPONENT);
                } else trg_pixel = background;
                *trg_value = trg_pixel.value;
            }
        }
    } else {
        for (u16 y = 0; y < viewport->dimensions.height; y++) {
            for (u16 x = 0; x < viewport->dimensions.width; x++, src_pixel++, trg_value++) {
                if (src_pixel->TL.depth == INFINITY)
                    trg_pixel = background;
                else {
                    color = scaleVec3(src_pixel->TL.color, src_pixel->TL.opacity);
                    trg_pixel.rgba.R = (u8)(color.r > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.r));
                    trg_pixel.rgba.G = (u8)(color.g > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.g));
                    trg_pixel.rgba.B = (u8)(color.b > (MAX_COLOR_VALUE * MAX_COLOR_VALUE) ? MAX_COLOR_VALUE : sqrt(color.b));
                    trg_pixel.rgba.A = (u8)(clampValue(src_pixel->TL.opacity) * FLOAT_TO_COLOR_COMPONENT);
                }
                *trg_value = trg_pixel.value;
            }
        }
    }
}

void fillViewport(Viewport *viewport, vec3 color, f32 opacity, f64 depth) {
    PixelQuad fill_pixel;
    Pixel fill_sub_pixel;
    fill_sub_pixel.color = color;
    fill_sub_pixel.opacity = opacity;
    fill_sub_pixel.depth = depth;
    fill_pixel.TL = fill_pixel.TR = fill_pixel.BL = fill_pixel.BR = fill_sub_pixel;
    for (i32 y = viewport->position.y; y < (viewport->position.y + viewport->dimensions.height); y++)
        for (i32 x = viewport->position.x; x < (viewport->position.x + viewport->dimensions.width); x++)
            viewport->pixels[viewport->dimensions.stride * y + x] = fill_pixel;
}

void clearViewportToBackground(Viewport *viewport) {
    fillViewport(viewport,
                 viewport->settings.background.color,
                 viewport->settings.background.opacity,
                 viewport->settings.background.depth);
}

void beginDrawing(Viewport *viewport) {
    clearViewportToBackground(viewport);
    setProjectionMatrix(viewport);
}

void endDrawing(Viewport *viewport) {
    if (viewport->settings.show_hud)
        drawHUD(viewport, &viewport->hud);
    drawViewportToWindowContent(viewport);
}