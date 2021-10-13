#pragma once

#include "../core/base.h"
//
//void drawCircle(Viewport *viewport, vec3 color, f32 opacity, i32 center_x, i32 center_y, i32 radius) {
//    if (radius <= 1) {
//        if (inRange(0, center_x, viewport->dimensions.width - 1) &&
//            inRange(0, center_y, viewport->dimensions.height - 1))
//            setPixel(viewport, center_y, center_x, color, opacity, 0);
//
//        return;
//    }
//
//    i32 width = viewport->dimensions.width;
//    i32 size  = viewport->dimensions.width_times_height;
//
//    i32 x = radius, y = 0, y2 = 0;
//    i32 r2 = radius * radius;
//    i32 x2 = r2;
//
//    i32 Sx1 = center_x - radius;
//    i32 Ex1 = center_x + radius;
//    i32 Sy1 = center_y * width;
//    i32 Ey1 = Sy1;
//
//    i32 Sx2 = center_x;
//    i32 Ex2 = center_x;
//    i32 Sy2 = (center_y - radius) * width;
//    i32 Ey2 = (center_y + radius) * width;
//
//    while (y <= x) {
//        if (Sy1 >= 0 && Sy1 < size) {
//            if (Sx1 >= 0 && Sx1 < width) setPixel(canvas->float_pixels + (Sy1 + Sx1), color, opacity, 0);
//            if (Ex1 >= 0 && Ex1 < width) setPixel(canvas->float_pixels + (Sy1 + Ex1), color, opacity, 0);
//        }
//        if (Ey1 >= 0 && Ey1 < size) {
//            if (Sx1 >= 0 && Sx1 < width) setPixel(canvas->float_pixels + (Ey1 + Sx1), color, opacity, 0);
//            if (Ex1 >= 0 && Ex1 < width) setPixel(canvas->float_pixels + (Ey1 + Ex1), color, opacity, 0);
//        }
//
//        if (Sy2 >= 0 && Sy2 < size) {
//            if (Sx2 >= 0 && Sx2 < width) setPixel(canvas->float_pixels + (Sy2 + Sx2), color, opacity, 0);
//            if (Ex2 >= 0 && Ex2 < width) setPixel(canvas->float_pixels + (Sy2 + Ex2), color, opacity, 0);
//        }
//        if (Ey2 >= 0 && Ey2 < size) {
//            if (Sx2 >= 0 && Sx2 < width) setPixel(canvas->float_pixels + (Ey2 + Sx2), color, opacity, 0);
//            if (Ex2 >= 0 && Ex2 < width) setPixel(canvas->float_pixels + (Ey2 + Ex2), color, opacity, 0);
//        }
//
//        if ((x2 + y2) > r2) {
//            x -= 1;
//            x2 = x * x;
//
//            Sx1 += 1;
//            Ex1 -= 1;
//
//            Sy2 += width;
//            Ey2 -= width;
//        }
//
//        y += 1;
//        y2 = y * y;
//
//        Sy1 -= width;
//        Ey1 += width;
//
//        Sx2 -= 1;
//        Ex2 += 1;
//    }
//}
//
//void fillCircle2D(PixelGrid *canvas, RGBA color, i32 center_x, i32 center_y, i32 radius) {
//    if (radius <= 1) {
//        if (inRange(0, center_x, canvas->dimensions.width - 1) &&
//            inRange(0, center_y, canvas->dimensions.height - 1))
//            setPixel(canvas->float_pixels + (canvas->dimensions.width * center_y + center_x), color, opacity, 0);
//
//        return;
//    }
//
//    i32 width = canvas->dimensions.width;
//    i32 size  = canvas->dimensions.width_times_height;
//
//    i32 x = radius, y = 0, y2 = 0;
//    i32 r2 = radius * radius;
//    i32 x2 = r2;
//
//    i32 Sx1 = center_x - radius;
//    i32 Ex1 = center_x + radius;
//    i32 Sy1 = center_y * width;
//    i32 Ey1 = Sy1;
//
//    i32 Sx2 = center_x;
//    i32 Ex2 = center_x;
//    i32 Sy2 = (center_y - radius) * width;
//    i32 Ey2 = (center_y + radius) * width;
//
//    i32 i, start, end;
//
//    while (y <= x) {
//        start = Sx1 > 0 ? Sx1 : 0;
//        end   = Ex1 < (width - 1) ? Ex1 : (width - 1);
//        if (Sy1 >= 0 && Sy1 < size) for (i = start; i <= end; i++) setPixel(canvas->float_pixels + (Sy1 + i), color, opacity, 0);
//        if (Ey1 >= 0 && Ey1 < size) for (i = start; i <= end; i++) setPixel(canvas->float_pixels + (Ey1 + i), color, opacity, 0);
//
//        start = Sx2 > 0 ? Sx2 : 0;
//        end   = Ex2 < (width - 1) ? Ex2 : (width - 1);
//        if (Sy2 >= 0 && Sy2 < size) for (i = start; i <= end; i++) setPixel(canvas->float_pixels + (Sy2 + i), color, opacity, 0);
//        if (Ey2 >= 0 && Ey2 < size) for (i = start; i <= end; i++) setPixel(canvas->float_pixels + (Ey2 + i), color, opacity, 0);
//
//        if ((x2 + y2) > r2) {
//            x -= 1;
//            x2 = x * x;
//
//            Sx1 += 1;
//            Ex1 -= 1;
//
//            Sy2 += width;
//            Ey2 -= width;
//        }
//
//        y += 1;
//        y2 = y * y;
//
//        Sy1 -= width;
//        Ey1 += width;
//
//        Sx2 -= 1;
//        Ex2 += 1;
//    }
//}