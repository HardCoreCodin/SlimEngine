#pragma once

#include "../math/vec3.h"

void drawHLine(i32 x_start, i32 x_end, i32 y, vec3 color, f32 opacity, Viewport *viewport) {
    x_start += viewport->position.x;
    x_end   += viewport->position.x;
    y       += viewport->position.y;

    if (!inRange(y, viewport->dimensions.height + viewport->position.y, viewport->position.y)) return;

    i32 first, last, step = 1;
    subRange(x_start, x_end, viewport->dimensions.width + viewport->position.x, viewport->position.x, &first, &last);

    color = mulVec3(color, color);
    if (viewport->settings.antialias) {
        y <<= 1;
        first <<= 1;
        last  <<= 1;
        step = 2;
        for (i32 x = first; x <= last; x += step) {
            setPixel(x+0, y+0, 0, color, opacity, viewport);
            setPixel(x+1, y+0, 0, color, opacity, viewport);
            setPixel(x+0, y+1, 0, color, opacity, viewport);
            setPixel(x+1, y+1, 0, color, opacity, viewport);
        }
    } else
        for (i32 x = first; x <= last; x += step)
            setPixel(x, y, 0, color, opacity, viewport);
}

void drawVLine(i32 y_start, i32 y_end, i32 x, vec3 color, f32 opacity, Viewport *viewport) {
    y_start += viewport->position.y;
    y_end   += viewport->position.y;
    x       += viewport->position.x;

    if (!inRange(x, viewport->dimensions.width + viewport->position.x, viewport->position.x)) return;
    i32 first, last, step = 1;

    subRange(y_start, y_end, viewport->dimensions.height + viewport->position.y, viewport->position.y, &first, &last);
    color = mulVec3(color, color);
    if (viewport->settings.antialias) {
        x <<= 1;
        first <<= 1;
        last  <<= 1;
        step = 2;
        for (i32 y = first; y <= last; y += step) {
            setPixel(x+0, y+0, 0, color, opacity, viewport);
            setPixel(x+1, y+0, 0, color, opacity, viewport);
            setPixel(x+0, y+1, 0, color, opacity, viewport);
            setPixel(x+1, y+1, 0, color, opacity, viewport);
        }
    } else
        for (i32 y = first; y <= last; y += step)
            setPixel(x, y, 0, color, opacity, viewport);
}

INLINE f32 fractionOf(f32 x) {
    return x - floorf(x);
}

INLINE f32 oneMinusFractionOf(f32 x) {
    return 1 - fractionOf(x);
}

INLINE void drawLine(f32 x1, f32 y1, f64 z1,
                     f32 x2, f32 y2, f64 z2,
                     vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    if (x1 < 0 &&
        y1 < 0 &&
        x2 < 0 &&
        y2 < 0)
        return;

    i32 x_left = viewport->position.x;
    i32 y_top  = viewport->position.y;
    x1 += (f32)x_left;
    x2 += (f32)x_left;
    y1 += (f32)y_top;
    y2 += (f32)y_top;

    i32 w = viewport->dimensions.width + x_left;
    i32 h = viewport->dimensions.height + y_top;

    color = mulVec3(color, color);
    i32 x, y;
    if (viewport->settings.antialias) {
        x1 += x1;
        x2 += x2;
        y1 += y1;
        y2 += y2;
        w <<= 1;
        h <<= 1;
        line_width <<= 1;
        line_width++;
    }
    f64 tmp, z_range, range_remap;
    f32 dx = x2 - x1;
    f32 dy = y2 - y1;
    f32 gap, grad, first_offset, last_offset;
    f64 z, z_curr, z_step;
    vec3 first, last;
    vec2i start, end;
    bool has_depth = z1 || z2;
    if (fabsf(dx) > fabsf(dy)) { // Shallow:
        if (x2 < x1) { // Left to right:
            tmp = x2; x2 = x1; x1 = (f32)tmp;
            tmp = y2; y2 = y1; y1 = (f32)tmp;
            tmp = z2; z2 = z1; z1 = tmp;
        }

        grad = dy / dx;

        first.x = roundf(x1);
        last.x  = roundf(x2);
        first_offset = first.x - x1;
        last_offset  = last.x  - x2;

        first.y = y1 + grad * first_offset;
        last.y  = y2 + grad * last_offset;

        start.x = (i32)first.x;
        start.y = (i32)first.y;
        end.x   = (i32)last.x;
        end.y   = (i32)last.y;

        x = start.x;
        y = start.y;
        gap = oneMinusFractionOf(x1 + 0.5f);

        if (inRange(x, w, x_left)) {
            if (inRange(y, h, y_top)) setPixel(x, y, z1, color, oneMinusFractionOf(first.y) * gap * opacity, viewport);

            for (u8 i = 0; i < line_width; i++) {
                y++;
                if (inRange(y, h, y_top)) setPixel(x, y, z1, color, opacity, viewport);
            }

            y++;
            if (inRange(y, h, y_top)) setPixel(x, y, z1, color, fractionOf(first.y) * gap * opacity, viewport);
        }

        x = end.x;
        y = end.y;
        gap = fractionOf(x2 + 0.5f);

        if (inRange(x, w, x_left)) {
            if (inRange(y, h, y_top)) setPixel(x, y, z2, color, oneMinusFractionOf(last.y) * gap * opacity, viewport);

            for (u8 i = 0; i < line_width; i++) {
                y++;
                if (inRange(y, h, y_top)) setPixel(x, y, z2, color, opacity, viewport);
            }

            y++;
            if (inRange(y, h, y_top)) setPixel(x, y, z2, color, fractionOf(last.y) * gap * opacity, viewport);
        }

        if (has_depth) { // Compute one-over-z start and step
            z1 = 1.0 / z1;
            z2 = 1.0 / z2;
            z_range = z2 - z1;
            range_remap = z_range / (f64)(x2 - x1);
            z1 += range_remap * (f64)(first_offset + 1);
            z2 += range_remap * (f64)(last_offset  - 1);
            z_range = z2 - z1;
            z_step = z_range / (f64)(last.x - first.x - 1);
            z_curr = z1;
        } else z = 0;

        gap = first.y + grad;
        for (x = start.x + 1; x < end.x; x++) {
            if (inRange(x, w, x_left)) {
                if (has_depth) z = 1.0 / z_curr;
                y = (i32) gap;
                if (inRange(y, h, y_top)) setPixel(x, y, z, color, oneMinusFractionOf(gap) * opacity, viewport);

                for (u8 i = 0; i < line_width; i++) {
                    y++;
                    if (inRange(y, h, y_top)) setPixel(x, y, z, color, opacity, viewport);
                }

                y++;
                if (inRange(y, h, y_top)) setPixel(x, y, z, color, fractionOf(gap) * opacity, viewport);
            }

            gap += grad;
            if (has_depth) z_curr += z_step;
        }
    } else { // Steep:
        if (y2 < y1) { // Bottom up:
            tmp = x2; x2 = x1; x1 = (f32)tmp;
            tmp = y2; y2 = y1; y1 = (f32)tmp;
            tmp = z2; z2 = z1; z1 = tmp;
        }

        grad = dx / dy;

        first.y = roundf(y1);
        last.y  = roundf(y2);

        first_offset = y1 - first.y;
        last_offset  = last.y  - y2;

        first.x = x1 + grad * first_offset;
        last.x  = x2 + grad * last_offset;

        start.y = (i32)first.y;
        start.x = (i32)first.x;

        end.y = (i32)last.y;
        end.x = (i32)last.x;

        x = start.x;
        y = start.y;
        gap = oneMinusFractionOf(y1 + 0.5f);

        if (inRange(y, h, y_top)) {
            if (inRange(x, w, x_left)) setPixel(x, y, z1, color, oneMinusFractionOf(first.x) * gap * opacity, viewport);

            for (u8 i = 0; i < line_width; i++) {
                x++;
                if (inRange(x, w, x_left)) setPixel(x, y, z1, color, opacity, viewport);
            }

            x++;
            if (inRange(x, w, x_left)) setPixel(x, y, z1, color, fractionOf(first.x) * gap * opacity, viewport);
        }

        x = end.x;
        y = end.y;
        gap = fractionOf(y2 + 0.5f);

        if (inRange(y, h, y_top)) {
            if (inRange(x, w, x_left)) setPixel(x, y, z2, color, oneMinusFractionOf(last.x) * gap * opacity, viewport);

            for (u8 i = 0; i < line_width; i++) {
                x++;
                if (inRange(x, w, x_left)) setPixel(x, y, z2, color, opacity, viewport);
            }

            x++;
            if (inRange(x, w, x_left)) setPixel(x, y, z2, color, fractionOf(last.x) * gap * opacity, viewport);
        }

        if (has_depth) { // Compute one-over-z start and step
            z1 = 1.0 / z1;
            z2 = 1.0 / z2;
            z_range = z2 - z1;
            range_remap = z_range / (f64)(y2 - y1);
            z1 += range_remap * (f64)(first_offset + 1);
            z2 += range_remap * (f64)(last_offset  - 1);
            z_range = z2 - z1;
            z_step = z_range / (f64)(last.y - first.y - 1);
            z_curr = z1;
        } else z = 0;

        gap = first.x + grad;
        for (y = start.y + 1; y < end.y; y++) {
            if (inRange(y, h, y_top)) {
                if (has_depth) z = 1.0 / z_curr;
                x = (i32)gap;

                if (inRange(x, w, x_left)) setPixel(x, y, z, color, oneMinusFractionOf(gap) * opacity, viewport);

                for (u8 i = 0; i < line_width; i++) {
                    x++;
                    if (inRange(x, w, x_left)) setPixel(x, y, z, color, opacity, viewport);
                }

                x++;
                if (inRange(x, w, x_left)) setPixel(x, y, z, color, fractionOf(gap) * opacity, viewport);
            }

            gap += grad;
            if (has_depth) z_curr += z_step;
        }
    }
}

//INLINE void swapf(f32 *a, f32 *b) {
//    f32 t = *a;
//    *a = *b;
//    *b = t;
//}
//
//f32 fpart(f32 x) {
//    return x - floorf(x);
//}
//
//f32 rfpart(f32 x) {
//    return 1.0f - fpart(x);
//}
//
//#define getPixel(x, y, canvas) ((canvas)->pixels + (canvas)->dimensions.width * (y) + (x))
//#define plot(x, y, opacity, color, canvas) (setPixel(getPixel(x, y, canvas), (color), opacity, 0, (canvas)->settings.gamma_corrected_blending))
//
//
//INLINE void drawLinehh(PixelGrid *canvas, vec3 color, f32 opacity,
//                     f32 x1, f32 y1, f64 z1,
//                     f32 x2, f32 y2, f64 z2,
//                     u8 line_width) {
//    bool steep = fabsf(y2 - y1) > fabsf(x2 - x1);
//
//    if (steep) {
//        swapf(&x1, &y1);
//        swapf(&x2, &y2);
//    }
//
//    if (x1 > x2) {
//        swapf(&x1, &x2);
//        swapf(&y1, &y2);
//    }
//
//    f32 dx = x2 - x1;
//    f32 dy = y2 - y1;
//    f32 gradient = dx ? (dy / dx) : 1.0f;
//
//    // handle first endpoint
//    f32 xend = roundf(x1);
//    f32 yend = y1 + gradient * (xend - x1);
//    f32 xgap = rfpart(x1 + 0.5f);
//    i32 xpxl1 = (i32)xend; // this will be used in the main loop
//    i32 ypxl1 = (i32)yend;
//    if (steep) {
//        plot(   ypxl1    , xpxl1, rfpart(yend) * xgap, color, canvas);
//        plot(ypxl1 + 1, xpxl1,  fpart(yend) * xgap, color, canvas);
//    } else {
//        plot(xpxl1,    ypxl1    , rfpart(yend) * xgap, color, canvas);
//        plot(xpxl1, ypxl1 + 1,  fpart(yend) * xgap, color, canvas);
//    }
//
//    f32 intery = yend + gradient; // first y-intersection for the main loop
//
//    // handle second endpoint
//    xend = roundf(x2);
//    yend = y2 + gradient * (xend - x2);
//    xgap = fpart(x2 + 0.5f);
//    i32 xpxl2 = (i32)xend; //this will be used in the main loop
//    i32 ypxl2 = (i32)yend;
//    if (steep) {
//        plot(   ypxl2    , xpxl2, rfpart(yend) * xgap, color, canvas);
//        plot(ypxl2 + 1, xpxl2,  fpart(yend) * xgap, color, canvas);
//    } else {
//        plot(xpxl2,    ypxl2    , rfpart(yend) * xgap, color, canvas);
//        plot(xpxl2, ypxl2 + 1,  fpart(yend) * xgap, color, canvas);
//    }
//
//    // main loop
//    if (steep) {
//        for (i32 x = xpxl1 + 1; x < xpxl2 - 1; x++, intery += gradient) {
//            plot((i32)intery    , x, rfpart(intery), color, canvas);
//            plot((i32)intery + 1, x,  fpart(intery), color, canvas);
//        }
//    } else {
//        for (i32 x = xpxl1 + 1; x < xpxl2 - 1; x++, intery += gradient) {
//            plot(x, (i32)intery   ,  rfpart(intery), color, canvas);
//            plot(x, (i32)intery + 1,  fpart(intery), color, canvas);
//        }
//    }
//}