#pragma once

#include "../core/base.h"
//
//void fillTriangle(PixelGrid *canvas, vec3 color, f32 opacity, f32 *X, f32 *Y) {
//    u16 W = canvas->dimensions.width;
//    u16 H = canvas->dimensions.height;
//    f32 dx1, x1, y1, xs,
//            dx2, x2, y2, xe,
//            dx3, x3, y3, dy;
//    i32 offset,
//            x, x1i, y1i, x2i, xsi, ysi = 0,
//            y, y2i, x3i, y3i, xei, yei = 0;
//    for (u8 i = 1; i <= 2; i++) {
//        if (Y[i] < Y[ysi]) ysi = i;
//        if (Y[i] > Y[yei]) yei = i;
//    }
//    u8 id[3];
//    if (ysi) {
//        if (ysi == 1) {
//            id[0] = 1;
//            id[1] = 2;
//            id[2] = 0;
//        } else {
//            id[0] = 2;
//            id[1] = 0;
//            id[2] = 1;
//        }
//    } else {
//        id[0] = 0;
//        id[1] = 1;
//        id[2] = 2;
//    }
//    x1 = X[id[0]]; y1 = Y[id[0]]; x1i = (i32)x1; y1i = (i32)y1;
//    x2 = X[id[1]]; y2 = Y[id[1]]; x2i = (i32)x2; y2i = (i32)y2;
//    x3 = X[id[2]]; y3 = Y[id[2]]; x3i = (i32)x3; y3i = (i32)y3;
//    dx1 = x1i == x2i || y1i == y2i ? 0 : (x2 - x1) / (y2 - y1);
//    dx2 = x2i == x3i || y2i == y3i ? 0 : (x3 - x2) / (y3 - y2);
//    dx3 = x1i == x3i || y1i == y3i ? 0 : (x3 - x1) / (y3 - y1);
//    dy = 1 - (y1 - (f32)y1);
//    xs = dx3 ? x1 + dx3 * dy : x1; ysi = (i32)Y[ysi];
//    xe = dx1 ? x1 + dx1 * dy : x1; yei = (i32)Y[yei];
//    offset = W * y1i;
//    for (y = ysi; y < yei; y++) {
//        if (y == y3i) xs = dx2 ? (x3 + dx2 * (1 - (y3 - (f32)y3i))) : x3;
//        if (y == y2i) xe = dx2 ? (x2 + dx2 * (1 - (y2 - (f32)y2i))) : x2;
//        xsi = (i32)xs;
//        xei = (i32)xe;
//        for (x = xsi; x < xei; x++) {
//            if (x > 0 && x < W && y > 0 && y < H)
//                setPixel(canvas->float_pixels + (offset + x), color, opacity, 0);
//        }
//        offset += W;
//        xs += y < y3i ? dx3 : dx2;
//        xe += y < y2i ? dx1 : dx2;
//    }
//}