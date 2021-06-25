#pragma once

#include "../core/base.h"

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