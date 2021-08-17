#pragma once

#include "../math/vec3.h"

void drawHLine2D(PixelGrid *canvas, RGBA color, i32 from, i32 to, i32 at) {
	if (!inRange(at, canvas->dimensions.height, 0)) return;

	i32 offset = at * (i32)canvas->dimensions.width;
    i32 first, last;
    subRange(from, to, canvas->dimensions.width, 0, &first, &last);
	first += offset;
	last += offset;
	for (i32 i = first; i <= last; i++) canvas->pixels[i].color = color;
}

void drawVLine2D(PixelGrid *canvas, RGBA color, i32 from, i32 to, i32 at) {
    if (!inRange(at, canvas->dimensions.width, 0)) return;
    i32 first, last;

    subRange(from, to, canvas->dimensions.height, 0, &first, &last);
	first *= canvas->dimensions.width; first += at;
	last  *= canvas->dimensions.width; last  += at;
	for (i32 i = first; i <= last; i += canvas->dimensions.width) canvas->pixels[i].color = color;
}

void drawLine2D(PixelGrid *canvas, RGBA color, i32 x0, i32 y0, i32 x1, i32 y1) {
    if (x0 < 0 &&
        y0 < 0 &&
        x1 < 0 &&
        y1 < 0)
        return;

    if (x0 == x1) {
	    drawVLine2D(canvas, color, y0, y1, x1);
	    return;
	}

    if (y0 == y1) {
	    drawHLine2D(canvas, color, x0, x1, y1);
	    return;
	}

	i32 width  = (i32)canvas->dimensions.width;
	i32 height = (i32)canvas->dimensions.height;

    i32 pitch = width;
	i32 index = x0 + y0 * pitch;

    i32 run  = x1 - x0;
    i32 rise = y1 - y0;

    i32 dx = 1;
    i32 dy = 1;
    if (run < 0) {
        dx = -dx;
        run = -run;
    }
    if (rise < 0) {
        dy = -dy;
        rise = -rise;
        pitch = -pitch;
    }

    // Configure for a shallow line:
    i32 end = x1 + dx;
    i32 start1 = x0;  i32 inc1 = dx;  i32 index_inc1 = dx;
    i32 start2 = y0;  i32 inc2 = dy;  i32 index_inc2 = pitch;
    i32 rise_twice = rise + rise;
    i32 run_twice = run + run;
    i32 threshold = run;
    i32 error_dec = run_twice;
    i32 error_inc = rise_twice;
    bool is_steap = rise > run;
    if (is_steap) { // Reconfigure for a steep line:
        swap(&inc1, &inc2);
        swap(&start1, &start2);
        swap(&index_inc1, &index_inc2);
        swap(&error_dec, &error_inc);
        end = y1 + dy;
        threshold = rise;
    }

    i32 error = 0;
    i32 current1 = start1;
    i32 current2 = start2;
    while (current1 != end) {
        if (inRange(index, canvas->dimensions.width_times_height, 0)) {
            if (is_steap) {
                if (inRange(current1, height, 0) &&
                    inRange(current2, width, 0))
                    canvas->pixels[index].color = color;
            } else {
                if (inRange(current2, height, 0) &&
                    inRange(current1, width, 0))
                    canvas->pixels[index].color = color;
            }
        }
        index += index_inc1;
        error += error_inc;
        current1 += inc1;
        if (error > threshold) {
            error -= error_dec;
            index += index_inc2;
            current2 += inc2;
        }
    }
}
bool alpha = true;
INLINE void setPixel(PixelGrid *canvas, RGBA color, f32 opacity, i32 x, i32 y, f64 z) {
    if (!inRange(y, canvas->dimensions.height, 0) ||
        !inRange(x, canvas->dimensions.width, 0))
        return;

    i32 index = canvas->dimensions.width * y + x;
    FloatPixel pixel, foreground, background = canvas->float_pixels[index];
    foreground.depth = z;
    foreground.opacity = opacity;// / (z > 10 ? (f32)z*0.05f : 1);
    foreground.color.x = (f32)color.R;
    foreground.color.y = (f32)color.G;
    foreground.color.z = (f32)color.B;
    foreground.color = scaleVec3(foreground.color, foreground.opacity * COLOR_COMPONENT_TO_FLOAT);

    if (foreground.depth > background.depth) {
        pixel = foreground;
        foreground = background;
        background = pixel;
    }

    if (alpha) {
        background.color = mulVec3(background.color, background.color);
        foreground.color = mulVec3(foreground.color, foreground.color);
    }

    opacity = 1.0f - foreground.opacity;
    pixel.color = scaleAddVec3(background.color, opacity, foreground.color);
    if (alpha) {
        pixel.color.x = sqrtf(pixel.color.x);
        pixel.color.y = sqrtf(pixel.color.y);
        pixel.color.z = sqrtf(pixel.color.z);
    }
    pixel.opacity = foreground.opacity + background.opacity * opacity;
    pixel.depth = foreground.depth;

    canvas->float_pixels[index] = pixel;
    canvas->pixels[index].color.R = (u8)((pixel.color.x > 1 ? 1 : pixel.color.x) * FLOAT_TO_COLOR_COMPONENT);
    canvas->pixels[index].color.G = (u8)((pixel.color.y > 1 ? 1 : pixel.color.y) * FLOAT_TO_COLOR_COMPONENT);
    canvas->pixels[index].color.B = (u8)((pixel.color.z > 1 ? 1 : pixel.color.z) * FLOAT_TO_COLOR_COMPONENT);
}

INLINE f32 fpart(f32 x) {
    return x - floorf(x);
}

INLINE f32 rfpart(f32 x) {
    return 1 - fpart(x);
}

void drawLineAA2D(PixelGrid *canvas, RGBA color, f32 x1, f32 y1, f32 x2, f32 y2, u8 line_width) {
    if (x1 < 0 &&
        y1 < 0 &&
        x2 < 0 &&
        y2 < 0)
        return;

    f32 opacity = (f32)color.A * COLOR_COMPONENT_TO_FLOAT;
    f32 dx = x2 - x1;
    f32 dy = y2 - y1;
    f32 tmp, gap, grad;
    i32 x, y;
    vec3 first, last;
    vec2i start, end;
    if (fabsf(dx) > fabsf(dy)) { // Shallow:
        if (x2 < x1) { // Left to right:
            tmp = x2; x2 = x1; x1 = tmp;
            tmp = y2; y2 = y1; y1 = tmp;
        }

        grad = dy / dx;

        first.x = roundf(x1);
        last.x  = roundf(x2);

        first.y = y1 + grad * (first.x - x1);
        last.y  = y2 + grad * (last.x  - x2);

        start.x = (i32)first.x;
        start.y = (i32)first.y;
        end.x   = (i32)last.x;
        end.y   = (i32)last.y;

        x = start.x;
        y = start.y;
        gap = rfpart(x1 + 0.5f);
        setPixel(canvas, color, rfpart(first.y) * gap * opacity, x, y++, 0);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,  opacity, x, y++, 0);
        setPixel(canvas, color,  fpart(first.y) * gap * opacity, x, y,   0);

        x = end.x;
        y = end.y;
        gap = fpart(x2 + 0.5f);
        setPixel(canvas, color,rfpart(last.y) * gap * opacity, x, y++, 0);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x, y++,0);
        setPixel(canvas, color, fpart(last.y) * gap * opacity, x, y,   0);

        gap = first.y + grad;
        for (x = start.x + 1; x < end.x; x++) {
            y = (i32)gap;
            setPixel(canvas, color, rfpart(gap) * opacity, x, y++, 0);
            for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x, y++,0);
            setPixel(canvas, color,  fpart(gap) * opacity, x, y,    0);
            gap += grad;
        }
    } else { // Steep:
        if (y2 < y1) { // Bottom up:
            tmp = x2; x2 = x1; x1 = tmp;
            tmp = y2; y2 = y1; y1 = tmp;
        }

        grad = dx / dy;

        first.y = roundf(y1);
        last.y  = roundf(y2);

        first.x = x1 + grad * (first.y - y1);
        last.x  = x2 + grad * (last.y  - y2);

        start.y = (i32)first.y;
        start.x = (i32)first.x;

        end.y = (i32)last.y;
        end.x = (i32)last.x;

        x = start.x;
        y = start.y;
        gap = rfpart(y1 + 0.5f);
        setPixel(canvas, color, rfpart(first.x) * gap * opacity, x++, y, 0);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x++, y,0);
        setPixel(canvas, color,  fpart(first.x) * gap * opacity, x,   y, 0);

        x = end.x;
        y = end.y;
        gap = fpart(y2 + 0.5f);
        setPixel(canvas, color, rfpart(last.x) * gap * opacity, x++, y, 0);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,opacity, x++, y,0);
        setPixel(canvas, color,  fpart(last.x) * gap * opacity, x,   y, 0);

        gap = first.x + grad;
        for (y = start.y + 1; y < end.y; y++) {
            x = (i32)gap;
            setPixel(canvas, color, rfpart(gap) * opacity, x++, y, 0);
            for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x++, y,0);
            setPixel(canvas, color,  fpart(gap) * opacity, x,   y, 0);
            gap += grad;
        }
    }
}

void drawLine3D(PixelGrid *canvas, RGBA color, vec3 from, vec3 to, u8 line_width) {
    f32 x1 = from.x;
    f32 y1 = from.y;
    f32 x2 = to.x;
    f32 y2 = to.y;

    if (x1 < 0 &&
        y1 < 0 &&
        x2 < 0 &&
        y2 < 0)
        return;

    f32 opacity = (f32)color.A * COLOR_COMPONENT_TO_FLOAT;
    f64 z1 = from.z;
    f64 z2 = to.z;
    f64 tmp;
    f32 dx = x2 - x1;
    f32 dy = y2 - y1;
    f32 gap, grad;
    i32 x, y;
    f64 z, z_curr, z_step;
    vec3 first, last;
    vec2i start, end;
    if (fabsf(dx) > fabsf(dy)) { // Shallow:
        if (x2 < x1) { // Left to right:
            tmp = x2; x2 = x1; x1 = (f32)tmp;
            tmp = y2; y2 = y1; y1 = (f32)tmp;
            tmp = z2; z2 = z1; z1 = tmp;
        }

        grad = dy / dx;

        first.x = roundf(x1);
        last.x  = roundf(x2);

        first.y = y1 + grad * (first.x - x1);
        last.y  = y2 + grad * (last.x  - x2);

        start.x = (i32)first.x;
        start.y = (i32)first.y;
        end.x   = (i32)last.x;
        end.y   = (i32)last.y;

        x = start.x;
        y = start.y;
        gap = rfpart(x1 + 0.5f);
        setPixel(canvas, color, rfpart(first.y) * gap * opacity, x, y++, z1);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,  opacity, x, y++, z1);
        setPixel(canvas, color,  fpart(first.y) * gap * opacity, x, y,   z1);

        x = end.x;
        y = end.y;
        gap = fpart(x2 + 0.5f);
        setPixel(canvas, color,rfpart(last.y) * gap * opacity, x, y++, z2);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x, y++, z1);
        setPixel(canvas, color, fpart(last.y) * gap * opacity, x, y,   z2);

//        z1 = 1.0 / z1;
//        z2 = 1.0 / z2;
        z_step = (z2 - z1) / (f64)(end.x - start.x + 1);
        z_curr = z1 + z_step;
        gap = first.y + grad;
        for (x = start.x + 1; x < end.x; x++) {
            y = (i32)gap;
            z = z_curr;
            setPixel(canvas, color, rfpart(gap) * opacity, x, y++, z);
            for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,opacity, x, y++,z);
            setPixel(canvas, color,  fpart(gap) * opacity, x, y,   z);
            gap += grad;
            z_curr += z_step;
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

        first.x = x1 + grad * (first.y - y1);
        last.x  = x2 + grad * (last.y  - y2);

        start.y = (i32)first.y;
        start.x = (i32)first.x;

        end.y = (i32)last.y;
        end.x = (i32)last.x;

        x = start.x;
        y = start.y;
        gap = rfpart(y1 + 0.5f);
        setPixel(canvas, color, rfpart(first.x) * gap * opacity, x++, y, z1);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,opacity, x++, y,z1);
        setPixel(canvas, color,  fpart(first.x) * gap * opacity, x,   y, z1);

        x = end.x;
        y = end.y;
        gap = fpart(y2 + 0.5f);
        setPixel(canvas, color, rfpart(last.x) * gap * opacity, x++, y, z2);
        for (u8 i = 0; i < line_width; i++) setPixel(canvas, color, opacity, x++, y,z2);
        setPixel(canvas, color,  fpart(last.x) * gap * opacity, x,   y, z2);

        z1 = 1.0 / z1;
        z2 = 1.0 / z2;
        z_step = (z2 - z1) / (f64)(end.y - start.y + 1);
        z_curr = z1 + z_step;
        gap = first.x + grad;
        for (y = start.y + 1; y < end.y; y++) {
            x = (i32)gap;
            z = 1.0 / z_curr;
            setPixel(canvas, color, rfpart(gap) * opacity, x++, y, z);
            for (u8 i = 0; i < line_width; i++) setPixel(canvas, color,opacity, x++, y,z);
            setPixel(canvas, color,  fpart(gap) * opacity, x,   y, z);
            gap += grad;
            z_curr += z_step;
        }
    }
}