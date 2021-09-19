#pragma once

#ifdef __cplusplus
#include <cmath>
#else
#include <math.h>
#endif

#if defined(__clang__)
#define COMPILER_CLANG 1
#define COMPILER_CLANG_OR_GCC 1
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_GCC 1
    #define COMPILER_CLANG_OR_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#endif

#ifndef NDEBUG
#define INLINE
#elif defined(COMPILER_MSVC)
#define INLINE inline __forceinline
#elif defined(COMPILER_CLANG_OR_GCC)
    #define INLINE inline __attribute__((always_inline))
#else
    #define INLINE inline
#endif

#ifdef COMPILER_CLANG
#define ENABLE_FP_CONTRACT \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
        _Pragma("STDC FP_CONTRACT ON") \
        _Pragma("clang diagnostic pop")
#else
#define ENABLE_FP_CONTRACT
#endif

#ifdef FP_FAST_FMAF
#define fast_mul_add(a, b, c) fmaf(a, b, c)
#else
ENABLE_FP_CONTRACT
#define fast_mul_add(a, b, c) ((a) * (b) + (c))
#endif

#ifdef __cplusplus
    #define null nullptr
#else
    #define null 0
    typedef unsigned char      bool;
 #endif

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef signed   short     i16;
typedef signed   int       i32;

typedef float  f32;
typedef double f64;

typedef void* (*CallbackWithInt)(u64 size);
typedef void (*CallbackWithBool)(bool on);
typedef void (*CallbackWithCharPtr)(char* str);

#define TAU 6.28f
#define SQRT2_OVER_2 0.70710678118f
#define SQRT3 1.73205080757f
#define COLOR_COMPONENT_TO_FLOAT 0.00392156862f
#define FLOAT_TO_COLOR_COMPONENT 255.0f

#define MAX_COLOR_VALUE 0xFF

#define MAX_WIDTH 3840
#define MAX_HEIGHT 2160

#define BOX__ALL_SIDES (Top | Bottom | Left | Right | Front | Back)
#define BOX__VERTEX_COUNT 8
#define BOX__EDGE_COUNT 12
#define GRID__MAX_SEGMENTS 101

#define IS_VISIBLE ((u8)1)
#define IS_TRANSLATED ((u8)2)
#define IS_ROTATED ((u8)4)
#define IS_SCALED ((u8)8)
#define IS_SCALED_NON_UNIFORMLY ((u8)16)
#define ALL_FLAGS (IS_VISIBLE | IS_TRANSLATED | IS_ROTATED | IS_SCALED | IS_SCALED_NON_UNIFORMLY)

#define CAMERA_DEFAULT__FOCAL_LENGTH 2.0f
#define CAMERA_DEFAULT__TARGET_DISTANCE 10

#define NAVIGATION_DEFAULT__MAX_VELOCITY 5
#define NAVIGATION_DEFAULT__ACCELERATION 10
#define NAVIGATION_SPEED_DEFAULT__TURN   1
#define NAVIGATION_SPEED_DEFAULT__ORIENT 0.002f
#define NAVIGATION_SPEED_DEFAULT__ORBIT  0.002f
#define NAVIGATION_SPEED_DEFAULT__ZOOM   0.003f
#define NAVIGATION_SPEED_DEFAULT__DOLLY  1
#define NAVIGATION_SPEED_DEFAULT__PAN    0.03f

#define VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE 0.1f
#define VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE 1000.0f

typedef struct u8_3 { u8 x, y, z; } u8_3;
typedef struct vec2i { i32 x, y; } vec2i;
typedef union vec2 { struct {f32 x, y;        }; struct {f32 u, v;       }; f32 components[2]; } vec2;
typedef union vec3 { struct {f32 x, y, z;     }; struct {f32 u, v, w;    }; struct {f32 r, g, b; }; struct {f32 red, green, blue; }; f32 components[3]; } vec3;
typedef union vec4 { struct {f32 x, y, z, w;  }; struct {f32 r, g, b, a; }; struct {vec3 v3; f32 _; }; f32 components[4]; } vec4;
typedef union mat2 { struct {vec2 X, Y;       }; vec2 axis[2]; } mat2;
typedef union mat3 { struct {vec3 X, Y, Z;    }; vec3 axis[3]; } mat3;
typedef union mat4 { struct {vec4 X, Y, Z, W; }; vec4 axis[4]; } mat4;
typedef struct AABB { vec3 min, max;   } AABB;
typedef struct quat { vec3 axis; f32 amount; } quat;
typedef struct Edge { vec3 from, to;  } Edge;
typedef struct Rect { vec2i min, max; } Rect;
typedef struct RGBA { u8 B, G, R, A; } RGBA;
typedef struct FloatPixel { vec3 color; f32 opacity; f64 depth; } FloatPixel;
typedef union Pixel { RGBA color; u32 value; } Pixel;

#define PIXEL_SIZE (sizeof(Pixel) + sizeof(FloatPixel) + (sizeof(f64)))
#define RENDER_SIZE (PIXEL_SIZE * MAX_WIDTH * MAX_HEIGHT)

INLINE vec2i Vec2i(i32 x, i32 y) {
    vec2i out;
    out.x = x;
    out.y = y;
    return out;
}
INLINE vec2 Vec2(f32 x, f32 y) {
    vec2 out;
    out.x = x;
    out.y = y;
    return out;
}
INLINE vec3 Vec3(f32 x, f32 y, f32 z) {
    vec3 out;
    out.x = x;
    out.y = y;
    out.z = z;
    return out;
}
INLINE vec3 Vec3fromVec4(vec4 v4) {
    vec3 out;
    out.x = v4.x;
    out.y = v4.y;
    out.z = v4.z;
    return out;
}
INLINE vec4 Vec4(f32 x, f32 y, f32 z, f32 w) {
    vec4 out;
    out.x = x;
    out.y = y;
    out.z = z;
    out.w = w;
    return out;
}
INLINE vec4 Vec4fromVec3(vec3 v3, f32 w) {
    vec4 out;
    out.x = v3.x;
    out.y = v3.y;
    out.z = v3.z;
    out.w = w;
    return out;
}
INLINE quat Quat(vec3 axis, f32 amout) {
    quat out;
    out.axis = axis;
    out.amount = amout;
    return out;
}

f32 smoothstep(f32 from, f32 to, f32 t) {
    t = (t - from) / (to - from);
    return t * t * (3 - 2 * t);
}

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define MEMORY_SIZE Gigabytes(1)
#define MEMORY_BASE Terabytes(2)

typedef struct Memory {
    u8* address;
    u64 occupied, capacity;
} Memory;

void initMemory(Memory *memory, u8* address, u64 capacity) {
    memory->address = (u8*)address;
    memory->capacity = capacity;
    memory->occupied = 0;
}

void* allocateMemory(Memory *memory, u64 size) {
    if (!memory->address) return null;
    memory->occupied += size;
    if (memory->occupied > memory->capacity) return null;

    void* address = memory->address;
    memory->address += size;
    return address;
}

typedef struct MouseButton {
    vec2i down_pos, up_pos, double_click_pos;
    bool is_pressed, is_handled;
} MouseButton;

typedef struct Mouse {
    MouseButton middle_button, right_button, left_button;
    vec2i pos, pos_raw_diff, movement;
    f32 wheel_scroll_amount;
    bool moved, is_captured,
         move_handled,
         double_clicked,
         double_clicked_handled,
         wheel_scrolled,
         wheel_scroll_handled,
         raw_movement_handled;
} Mouse;

void resetMouseChanges(Mouse *mouse) {
    if (mouse->move_handled) {
        mouse->move_handled = false;
        mouse->moved = false;
    }
    if (mouse->double_clicked_handled) {
        mouse->double_clicked_handled = false;
        mouse->double_clicked = false;
    }
    if (mouse->raw_movement_handled) {
        mouse->raw_movement_handled = false;
        mouse->pos_raw_diff.x = 0;
        mouse->pos_raw_diff.y = 0;
    }
    if (mouse->wheel_scroll_handled) {
        mouse->wheel_scroll_handled = false;
        mouse->wheel_scrolled = false;
        mouse->wheel_scroll_amount = 0;
    }
}

typedef struct Dimensions {
    u16 width, height;
    u32 width_times_height;
    f32 height_over_width,
        width_over_height,
        f_height, f_width,
        h_height, h_width;
} Dimensions;

void updateDimensions(Dimensions *dimensions, u16 width, u16 height) {
    dimensions->width = width;
    dimensions->height = height;
    dimensions->width_times_height = dimensions->width * dimensions->height;
    dimensions->f_width  =      (f32)dimensions->width;
    dimensions->f_height =      (f32)dimensions->height;
    dimensions->h_width  =           dimensions->f_width  / 2;
    dimensions->h_height =           dimensions->f_height / 2;
    dimensions->width_over_height  = dimensions->f_width  / dimensions->f_height;
    dimensions->height_over_width  = dimensions->f_height / dimensions->f_width;
}

typedef struct PixelGrid {
    Dimensions dimensions;
    Pixel* pixels;
    FloatPixel* float_pixels;
    bool gamma_corrected_blending;
} PixelGrid;

void swap(i32 *a, i32 *b) {
    i32 t = *a;
    *a = *b;
    *b = t;
}

void subRange(i32 from, i32 to, i32 end, i32 start, i32 *first, i32 *last) {
    *first = from;
    *last  = to;
    if (to < from) swap(first, last);
    *first = *first > start ? *first : start;
    *last  = (*last < end ? *last : end) - 1;
}

INLINE bool inRange(i32 value, i32 end, i32 start) {
    return value >= start && value < end;
}

INLINE f32 clampValueToBetween(f32 value, f32 from, f32 to) {
    f32 mn = value < to ? value : to;
    return mn > from ? mn : from;
}

INLINE f32 clampValue(f32 value) {
    f32 mn = value < 1.0f ? value : 1.0f;
    return mn > 0.0f ? mn : 0.0f;
}

INLINE f32 approach(f32 src, f32 trg, f32 diff) {
    f32 out;

    out = src + diff; if (trg > out) return out;
    out = src - diff; if (trg < out) return out;

    return trg;
}

INLINE vec2 getPointOnUnitCircle(f32 t) {
    f32 t_squared = t * t;
    f32 factor = 1 / (1 + t_squared);

    vec2 xy = {factor - factor * t_squared, factor * 2 * t};

    return xy;
}

enum ColorID {
    Black,
    White,
    Grey,

    Red,
    Green,
    Blue,

    Cyan,
    Magenta,
    Yellow,

    DarkRed,
    DarkGreen,
    DarkBlue,
    DarkGrey,

    BrightRed,
    BrightGreen,
    BrightBlue,
    BrightGrey,

    BrightCyan,
    BrightMagenta,
    BrightYellow,

    DarkCyan,
    DarkMagenta,
    DarkYellow
};

INLINE RGBA ColorOf(enum ColorID color_id) {
    RGBA color;
    color.A = MAX_COLOR_VALUE;

    switch (color_id) {
        case Black:
            color.R = 0;
            color.G = 0;
            color.B = 0;
            break;
        case White:
            color.R = MAX_COLOR_VALUE;
            color.G = MAX_COLOR_VALUE;
            color.B = MAX_COLOR_VALUE;

            break;
        case Grey:
                color.R = MAX_COLOR_VALUE/2;
                color.G = MAX_COLOR_VALUE/2;
                color.B = MAX_COLOR_VALUE/2;
                break;
        case DarkGrey:
            color.R = MAX_COLOR_VALUE/4;
            color.G = MAX_COLOR_VALUE/4;
            color.B = MAX_COLOR_VALUE/4;
            break;
        case BrightGrey:
            color.R = MAX_COLOR_VALUE*3/4;
            color.G = MAX_COLOR_VALUE*3/4;
            color.B = MAX_COLOR_VALUE*3/4;
            break;

        case Red:
            color.R = MAX_COLOR_VALUE;
            color.G = 0;
            color.B = 0;
            break;
        case Green:
            color.R = 0;
            color.G = MAX_COLOR_VALUE;
            color.B = 0;
            break;
        case Blue:
            color.R = 0;
            color.G = 0;
            color.B = MAX_COLOR_VALUE;
            break;

        case DarkRed:
            color.R = MAX_COLOR_VALUE/2;
            color.G = 0;
            color.B = 0;
            break;
        case DarkGreen:
            color.R = 0;
            color.G = MAX_COLOR_VALUE/2;
            color.B = 0;
            break;
        case DarkBlue:
            color.R = 0;
            color.G = 0;
            color.B = MAX_COLOR_VALUE/2;
            break;

        case DarkCyan:
            color.R = 0;
            color.G = MAX_COLOR_VALUE/2;
            color.B = MAX_COLOR_VALUE/2;
            break;
        case DarkMagenta:
            color.R = MAX_COLOR_VALUE/2;
            color.G = 0;
            color.B = MAX_COLOR_VALUE/2;
            break;
        case DarkYellow:
            color.R = MAX_COLOR_VALUE/2;
            color.G = MAX_COLOR_VALUE/2;
            color.B = 0;
            break;

        case BrightRed:
            color.R = MAX_COLOR_VALUE;
            color.G = MAX_COLOR_VALUE/2;
            color.B = MAX_COLOR_VALUE/2;
            break;
        case BrightGreen:
            color.R = MAX_COLOR_VALUE/2;
            color.G = MAX_COLOR_VALUE;
            color.B = MAX_COLOR_VALUE/2;
            break;
        case BrightBlue:
            color.R = MAX_COLOR_VALUE/2;
            color.G = MAX_COLOR_VALUE/2;
            color.B = MAX_COLOR_VALUE;
            break;

        case Cyan:
            color.R = 0;
            color.G = MAX_COLOR_VALUE;
            color.B = MAX_COLOR_VALUE;
            break;
        case Magenta:
            color.R = MAX_COLOR_VALUE;
            color.G = 0;
            color.B = MAX_COLOR_VALUE;
            break;
        case Yellow:
            color.R = MAX_COLOR_VALUE;
            color.G = MAX_COLOR_VALUE;
            color.B = 0;
            break;

        case BrightCyan:
            color.R = 0;
            color.G = MAX_COLOR_VALUE*3/4;
            color.B = MAX_COLOR_VALUE*3/4;
            break;
        case BrightMagenta:
            color.R = MAX_COLOR_VALUE*3/4;
            color.G = 0;
            color.B = MAX_COLOR_VALUE*3/4;
            break;
        case BrightYellow:
            color.R = MAX_COLOR_VALUE*3/4;
            color.G = MAX_COLOR_VALUE*3/4;
            color.B = 0;
            break;
    }

    return color;
}

INLINE vec3 Color(enum ColorID color_id) {
    vec3 color;

    switch (color_id) {
        case Black:
            color.r = 0.0f;
            color.g = 0.0f;
            color.b = 0.0f;
            break;
        case White:
            color.r = (f32)MAX_COLOR_VALUE;
            color.g = (f32)MAX_COLOR_VALUE;
            color.b = (f32)MAX_COLOR_VALUE;

            break;
        case Grey:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;
        case DarkGrey:
            color.r = (f32)MAX_COLOR_VALUE / 4.0f;
            color.g = (f32)MAX_COLOR_VALUE / 4.0f;
            color.b = (f32)MAX_COLOR_VALUE / 4.0f;
            break;
        case BrightGrey:
            color.r = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.g = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.b = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            break;

        case Red:
            color.r = (f32)MAX_COLOR_VALUE;
            color.g = 0.0f;
            color.b = 0.0f;
            break;
        case Green:
            color.r = 0.0f;
            color.g = (f32)MAX_COLOR_VALUE;
            color.b = 0.0f;
            break;
        case Blue:
            color.r = 0.0f;
            color.g = 0.0f;
            color.b = (f32)MAX_COLOR_VALUE;
            break;

        case DarkRed:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = 0.0f;
            color.b = 0.0f;
            break;
        case DarkGreen:
            color.r = 0.0f;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = 0.0f;
            break;
        case DarkBlue:
            color.r = 0.0f;
            color.g = 0.0f;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;

        case DarkCyan:
            color.r = 0.0f;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;
        case DarkMagenta:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = 0.0f;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;
        case DarkYellow:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = 0.0f;
            break;

        case BrightRed:
            color.r = (f32)MAX_COLOR_VALUE;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;
        case BrightGreen:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = (f32)MAX_COLOR_VALUE;
            color.b = (f32)MAX_COLOR_VALUE / 2.0f;
            break;
        case BrightBlue:
            color.r = (f32)MAX_COLOR_VALUE / 2.0f;
            color.g = (f32)MAX_COLOR_VALUE / 2.0f;
            color.b = (f32)MAX_COLOR_VALUE;
            break;

        case Cyan:
            color.r = 0.0f;
            color.g = (f32)MAX_COLOR_VALUE;
            color.b = (f32)MAX_COLOR_VALUE;
            break;
        case Magenta:
            color.r = (f32)MAX_COLOR_VALUE;
            color.g = 0.0f;
            color.b = (f32)MAX_COLOR_VALUE;
            break;
        case Yellow:
            color.r = (f32)MAX_COLOR_VALUE;
            color.g = (f32)MAX_COLOR_VALUE;
            color.b = 0.0f;
            break;

        case BrightCyan:
            color.r = 0.0f;
            color.g = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.b = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            break;
        case BrightMagenta:
            color.r = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.g = 0.0f;
            color.b = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            break;
        case BrightYellow:
            color.r = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.g = (f32)MAX_COLOR_VALUE * 3.0f / 4.0f;
            color.b = 0.0f;
            break;
    }

    return color;
}

vec3 getColorInBetween(vec3 from, vec3 to, f32 t) {
    return Vec3(
            clampValueToBetween(fast_mul_add(to.r - from.r, t, from.r), 0, (f32)MAX_COLOR_VALUE),
            clampValueToBetween(fast_mul_add(to.g - from.g, t, from.g), 0, (f32)MAX_COLOR_VALUE),
            clampValueToBetween(fast_mul_add(to.b - from.b, t, from.b), 0, (f32)MAX_COLOR_VALUE)
   );
}

void copyPixels(PixelGrid *src, PixelGrid *trg, i32 width, i32 height, i32 trg_x, i32 trg_y) {
    Pixel *trg_pixels = trg->pixels;
    Pixel *src_pixels = src->pixels;
    Dimensions *trg_dim = &trg->dimensions;
    Dimensions *src_dim = &src->dimensions;
    i32 trg_index;
    for (i32 y = 0; y < height; y++) {
        for (i32 x = 0; x < width; x++) {
            trg_index = y + trg_y;
            trg_index *= trg_dim->width;
            trg_index += x + trg_x;
            trg_pixels[trg_index] = src_pixels[src_dim->width * y + x];
        }
    }
}

INLINE void setPixel(FloatPixel *pixel, vec3 color, f32 opacity, f64 z, bool blend_with_gamma_correction) {
    FloatPixel foreground, background = *pixel;
    foreground.depth = z;
    foreground.opacity = opacity;
    foreground.color.r = color.r * opacity;
    foreground.color.g = color.g * opacity;
    foreground.color.b = color.b * opacity;

    if (foreground.depth > background.depth) {
        *pixel = foreground;
        foreground = background;
        background = *pixel;
    }

    if (blend_with_gamma_correction) {
        background.color.r *= background.color.r;
        background.color.g *= background.color.g;
        background.color.b *= background.color.b;
        foreground.color.r *= foreground.color.r;
        foreground.color.g *= foreground.color.g;
        foreground.color.b *= foreground.color.b;
    }

    opacity = 1.0f - foreground.opacity;
    pixel->color.r = fast_mul_add(background.color.r, opacity, foreground.color.r);
    pixel->color.g = fast_mul_add(background.color.g, opacity, foreground.color.g);
    pixel->color.b = fast_mul_add(background.color.b, opacity, foreground.color.b);

    if (blend_with_gamma_correction) {
        pixel->color.r = sqrtf(pixel->color.r);
        pixel->color.g = sqrtf(pixel->color.g);
        pixel->color.b = sqrtf(pixel->color.b);
    }

    pixel->opacity = foreground.opacity + background.opacity * opacity;
    pixel->depth = foreground.depth;
}

void preparePixelGridForDisplay(PixelGrid *canvas) {
    FloatPixel *float_pixel = canvas->float_pixels;
    Pixel *pixel = canvas->pixels;
    for (u32 pixel_index = 0; pixel_index < canvas->dimensions.width_times_height; pixel_index++, float_pixel++, pixel++) {
        pixel->color.R = (u8)(clampValueToBetween(float_pixel->color.r, 0, MAX_COLOR_VALUE));
        pixel->color.G = (u8)(clampValueToBetween(float_pixel->color.g, 0, MAX_COLOR_VALUE));
        pixel->color.B = (u8)(clampValueToBetween(float_pixel->color.b, 0, MAX_COLOR_VALUE));
        pixel->color.A = (u8)(clampValue(float_pixel->opacity) * FLOAT_TO_COLOR_COMPONENT);
    }
}

typedef struct String {
    u32 length;
    char *char_ptr;
} String;

typedef struct NumberString {
    char _buffer[13];
    String string;
} NumberString;

typedef struct HUDLine {
    String title, alternate_value;
    NumberString value;
    enum ColorID title_color, value_color, alternate_value_color;
    bool invert_alternate_use, *use_alternate;
} HUDLine;

typedef struct HUD {
    vec2i position;
    u32 line_count;
    f32 line_height;
    HUDLine *lines;
} HUD;