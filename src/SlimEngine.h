#pragma once

#define SLIM_ENGINE_SHF

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
    #ifndef signbit
        #define signbit std::signbit
    #endif
#else
#define null 0
typedef unsigned char      bool;
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long int  u32;
typedef unsigned long long u64;
typedef signed   short     i16;
typedef signed   long int  i32;

typedef float  f32;
typedef double f64;

#ifndef false
#define false (u8)0
#endif

#ifndef true
#define true (u8)1
#endif

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

#define VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE 0.001f
#define VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE 1000.0f

// Culling flags:
// ======================
#define IS_NEAR  0b00000001
#define IS_FAR   0b00000010
#define IS_BELOW 0b00000100
#define IS_ABOVE 0b00001000
#define IS_RIGHT 0b00010000
#define IS_LEFT  0b00100000
#define IS_OUT   0b00111111
#define IS_NDC   0b01000000

// Clipping flags:
// ===============
#define CULL    0b00000000
#define INSIDE  0b00000010

#define LAMBERT 1
#define PHONG 2
#define BLINN 4

typedef struct u8_3 { u8 x, y, z; } u8_3;
typedef struct vec2i { i32 x, y; } vec2i;
typedef struct vec2u { u32 x, y; } vec2u;
typedef union vec2 { struct {f32 x, y;        }; struct {f32 u, v;       }; f32 components[2]; } vec2;
typedef union vec3 { struct {f32 x, y, z;     }; struct {f32 u, v, W;    }; struct {vec2 v2; f32 _; }; struct {f32 r, g, b; }; struct {f32 red, green, blue; }; struct {f32 A, B, C; }; f32 components[3]; } vec3;
typedef union vec4 { struct {f32 x, y, z, w;  }; struct {f32 r, g, b, a; }; struct {vec3 v3; f32 _; }; f32 components[4]; } vec4;
typedef union mat2 { struct {vec2 X, Y;       }; vec2 axis[2]; } mat2;
typedef union mat3 { struct {vec3 X, Y, Z;    }; vec3 axis[3]; } mat3;
typedef union mat4 { struct {vec4 X, Y, Z, W; }; vec4 axis[4]; } mat4;
typedef struct AABB { vec3 min, max;   } AABB;
typedef struct quat { vec3 axis; f32 amount; } quat;
typedef struct Edge { vec3 from, to;  } Edge;
typedef struct Rect { vec2i min, max; } Rect;
typedef struct RGBA { u8 B, G, R, A; } RGBA;
typedef struct Pixel { vec3 color; f32 opacity; f64 depth; } Pixel;
typedef union PixelQuad { Pixel quad[2][2]; struct { Pixel TL, TR, BL, BR; }; } PixelQuad;

#define FRAME_BUFFER_MEMORY_SIZE (sizeof(PixelQuad) * MAX_WIDTH * MAX_HEIGHT)

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
    u16 width, height, stride;
    u32 width_times_height;
    f32 height_over_width,
            width_over_height,
            f_height, f_width,
            h_height, h_width;
} Dimensions;

void updateDimensions(Dimensions *dimensions, u16 width, u16 height, u16 stride) {
    dimensions->width = width;
    dimensions->height = height;
    dimensions->stride = stride;
    dimensions->width_times_height = dimensions->width * dimensions->height;
    dimensions->f_width  =      (f32)dimensions->width;
    dimensions->f_height =      (f32)dimensions->height;
    dimensions->h_width  =           dimensions->f_width  / 2;
    dimensions->h_height =           dimensions->f_height / 2;
    dimensions->width_over_height  = dimensions->f_width  / dimensions->f_height;
    dimensions->height_over_width  = dimensions->f_height / dimensions->f_width;
}

INLINE void swap(i32 *a, i32 *b) {
    i32 t = *a;
    *a = *b;
    *b = t;
}

INLINE void subRange(i32 from, i32 to, i32 end, i32 start, i32 *first, i32 *last) {
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
    if (value != value || value > to)
        value = value;
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

INLINE vec3 getColorInBetween(vec3 from, vec3 to, f32 t) {
    return Vec3(
            clampValueToBetween(fast_mul_add(to.r - from.r, t, from.r), 0, (f32)MAX_COLOR_VALUE),
            clampValueToBetween(fast_mul_add(to.g - from.g, t, from.g), 0, (f32)MAX_COLOR_VALUE),
            clampValueToBetween(fast_mul_add(to.b - from.b, t, from.b), 0, (f32)MAX_COLOR_VALUE)
    );
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


void setString(String *string, char *char_ptr) {
    string->char_ptr = char_ptr;
    string->length = 0;
    if (char_ptr)
        while (char_ptr[string->length])
            string->length++;
}

u32 getStringLength(char *string) {
    char *char_ptr = string;
    u32 length = 0;
    if (char_ptr) while (char_ptr[length]) length++;
    return length;
}

u32 getDirectoryLength(char *path) {
    u32 path_len = getStringLength(path);
    u32 dir_len = path_len;
    while (path[dir_len] != '/' && path[dir_len] != '\\') dir_len--;
    return dir_len + 1;
}

void copyToString(String *string, char* char_ptr, u32 offset) {
    string->length = offset;
    char *source_char = char_ptr;
    char *string_char = string->char_ptr + offset;
    while (source_char[0]) {
        *string_char = *source_char;
        string_char++;
        source_char++;
        string->length++;
    }
    *string_char = 0;
}

void initNumberString(NumberString *number_string) {
    number_string->string.char_ptr = number_string->_buffer;
    number_string->string.length = 1;
    number_string->_buffer[12] = 0;
    for (u8 i = 0; i < 12; i++)
        number_string->_buffer[i] = ' ';
}

void mergeString(String *string, char* first, char* second, u32 offset) {
    copyToString(string, first, 0);
    copyToString(string, second, offset);
}

void printNumberIntoString(i32 number, NumberString *number_string) {
    initNumberString(number_string);
    char *buffer = number_string->_buffer;
    buffer[12] = 0;

    bool is_negative = number < 0;
    if (is_negative) number = -number;

    if (number) {
        u32 temp;
        buffer += 11;
        number_string->string.char_ptr = buffer;
        number_string->string.length = 0;

        for (u8 i = 0; i < 11; i++) {
            temp = number;
            number /= 10;
            number_string->string.length++;
            *buffer-- = (char)('0' + temp - number * 10);
            if (!number) {
                if (is_negative) {
                    *buffer = '-';
                    number_string->string.char_ptr--;
                    number_string->string.length++;
                }

                break;
            }
            number_string->string.char_ptr--;
        }
    } else {
        buffer[11] = '0';
        number_string->string.length = 1;
        number_string->string.char_ptr = buffer + 11;
    }
}

void printFloatIntoString(f32 number, NumberString *number_string, u8 float_digits_count) {
    f32 factor = 1;
    for (u8 i = 0; i < float_digits_count; i++) factor *= 10;
    i32 int_num = (i32)(number * factor);
    if (int_num == 0) {
        printNumberIntoString((i32)factor, number_string);
        number_string->string.length++;
        number_string->string.char_ptr[0] = '.';
        number_string->string.char_ptr--;
        number_string->string.char_ptr[0] = '0';
        return;
    }

    bool is_negative = number < 0;
    bool is_fraction = is_negative ? number > -1 : number < 1;

    printNumberIntoString(int_num, number_string);

    if (is_fraction) {
        u32 len = number_string->string.length;
        number_string->string.length++;
        number_string->string.char_ptr--;
        if (is_negative) {
            number_string->string.char_ptr[0] = '-';
            number_string->string.char_ptr[1] = '0';
        } else {
            number_string->string.char_ptr[0] = '0';
        }
        if (len < float_digits_count) {
            for (u32 i = 0; i < (float_digits_count - len); i++) {
                number_string->string.length++;
                number_string->string.char_ptr--;
                number_string->string.char_ptr[0] = '0';
            }
        }
    }

    static char tmp[13];
    tmp[number_string->string.length + 1] = 0;
    for (u8 i = 0; i < (u8)number_string->string.length; i++) {
        u8 char_count_from_right_to_left = (u8)number_string->string.length - i - 1;
        if (char_count_from_right_to_left >= float_digits_count) tmp[i] = number_string->string.char_ptr[i];
        else                                                     tmp[i + 1] = number_string->string.char_ptr[i];
    }
    tmp[number_string->string.length - float_digits_count] = '.';
    copyToString(&number_string->string, tmp, 0);
    if (is_negative) number_string->string.length++;
}



#define FONT_WIDTH 18
#define FONT_HEIGHT 24
#define LINE_HEIGHT 30
#define FIRST_CHARACTER_CODE 32
#define LAST_CHARACTER_CODE 126

// Header File for SSD1306 characters
// Generated with TTF2BMH
// Font JetBrains Mono Regular
// Font Size: 24
u8 bitmap_32[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_33[] = {0,0,0,0,0,0,62,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,14,14,4,0,0,0,0,0,0,0,0};
u8 bitmap_34[] = {0,0,0,0,254,254,0,0,0,254,254,254,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_35[] = {0,0,192,192,192,252,254,192,192,192,192,252,254,192,192,0,0,0,0,96,96,240,255,111,96,96,96,240,255,111,96,96,0,0,0,0,0,0,12,15,1,0,0,0,12,15,1,0,0,0,0,0,0,0};
u8 bitmap_36[] = {0,0,112,248,252,14,6,255,255,6,14,60,56,32,0,0,0,0,0,0,128,129,3,7,6,255,255,12,28,248,240,192,0,0,0,0,0,0,3,7,14,12,12,255,255,12,12,15,7,1,0,0,0,0};
u8 bitmap_37[] = {0,252,206,2,2,6,252,120,0,0,0,128,192,64,96,0,0,0,0,192,97,49,17,25,13,4,246,59,9,9,24,240,224,0,0,0,0,0,0,0,0,0,0,0,7,14,8,8,12,7,3,0,0,0};
u8 bitmap_38[] = {0,0,0,252,252,142,6,6,6,14,28,24,0,0,0,0,0,0,0,248,252,254,3,3,15,28,120,240,224,248,60,12,4,0,0,0,0,1,7,7,14,12,12,14,6,7,1,7,14,12,8,0,0,0};
u8 bitmap_39[] = {0,0,0,0,0,0,126,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_40[] = {0,0,0,0,0,240,252,62,7,3,1,1,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,7,15,28,56,112,96,96,0,0,0,0,0};
u8 bitmap_41[] = {0,0,0,0,1,1,3,15,254,248,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,96,112,48,56,30,15,3,0,0,0,0,0,0,0,0};
u8 bitmap_42[] = {0,0,0,0,0,0,0,240,240,0,0,0,0,0,0,0,0,0,0,3,3,131,198,246,124,15,61,118,230,199,131,3,2,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0};
u8 bitmap_43[] = {0,0,0,0,0,0,0,224,224,0,0,0,0,0,0,0,0,0,0,0,24,24,24,24,24,255,255,24,24,24,24,24,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0};
u8 bitmap_44[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,255,63,15,1,0,0,0,0,0,0,0,0};
u8 bitmap_45[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_46[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,128,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,15,7,0,0,0,0,0,0,0,0};
u8 bitmap_47[] = {0,0,0,0,0,0,0,0,224,248,127,15,1,0,0,0,0,0,0,0,0,0,0,224,248,127,15,1,0,0,0,0,0,0,0,0,0,0,96,120,127,15,1,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_48[] = {0,0,240,248,60,14,6,134,134,14,12,252,248,192,0,0,0,0,0,0,255,255,128,0,31,63,63,0,0,255,255,127,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_49[] = {0,0,0,56,24,28,14,254,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,12,12,12,12,12,15,15,15,12,12,12,12,0,0,0,0};
u8 bitmap_50[] = {0,0,56,60,28,14,6,6,6,14,28,252,248,0,0,0,0,0,0,0,0,0,128,192,224,112,48,28,15,7,3,0,0,0,0,0,0,0,14,15,15,13,12,12,12,12,12,12,12,12,0,0,0,0};
u8 bitmap_51[] = {0,0,6,6,6,6,134,198,102,54,62,30,14,0,0,0,0,0,0,0,128,128,0,3,3,3,3,7,14,254,252,96,0,0,0,0,0,0,1,3,7,14,12,12,12,14,7,7,3,0,0,0,0,0};
u8 bitmap_52[] = {0,0,0,0,0,128,192,240,120,30,14,2,0,0,0,0,0,0,0,0,240,252,254,199,195,192,192,192,192,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,0,0,0,0,0};
u8 bitmap_53[] = {0,0,254,254,254,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,7,7,7,6,3,3,3,3,7,254,252,240,0,0,0,0,0,0,1,7,7,14,12,12,12,14,14,7,3,0,0,0,0,0};
u8 bitmap_54[] = {0,0,0,0,128,224,240,60,30,6,2,0,0,0,0,0,0,0,0,0,248,254,15,5,2,3,3,3,7,254,252,240,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_55[] = {0,0,62,62,6,6,6,6,6,6,230,254,126,14,0,0,0,0,0,0,0,0,0,0,128,224,252,63,15,1,0,0,0,0,0,0,0,0,0,0,8,14,15,7,1,0,0,0,0,0,0,0,0,0};
u8 bitmap_56[] = {0,0,112,248,252,14,6,6,6,14,12,252,248,0,0,0,0,0,0,0,240,249,29,15,6,6,6,7,15,253,248,224,0,0,0,0,0,0,1,7,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_57[] = {0,0,240,248,28,14,6,6,6,6,12,252,248,224,0,0,0,0,0,0,3,7,15,14,12,140,204,244,126,31,7,1,0,0,0,0,0,0,0,0,0,8,14,15,3,1,0,0,0,0,0,0,0,0};
u8 bitmap_58[] = {0,0,0,0,0,0,224,224,224,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,15,6,0,0,0,0,0,0,0,0};
u8 bitmap_59[] = {0,0,0,0,0,0,224,224,224,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,255,63,7,1,0,0,0,0,0,0,0,0};
u8 bitmap_60[] = {0,0,0,0,0,128,128,192,192,224,96,112,48,0,0,0,0,0,0,0,14,15,27,27,49,49,96,96,192,192,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};
u8 bitmap_61[] = {0,0,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_62[] = {0,0,48,112,96,96,192,192,128,128,0,0,0,0,0,0,0,0,0,0,128,192,192,224,96,112,49,57,27,27,14,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_63[] = {0,0,0,0,6,6,6,6,6,12,252,248,224,0,0,0,0,0,0,0,0,0,0,124,124,124,12,14,7,3,0,0,0,0,0,0,0,0,0,0,0,14,14,14,4,0,0,0,0,0,0,0,0,0};
u8 bitmap_64[] = {0,192,240,120,28,14,6,6,134,134,142,12,252,248,192,0,0,0,0,255,255,0,0,0,252,255,7,1,1,3,253,255,255,0,0,0,0,15,63,120,224,192,193,131,135,134,6,2,1,7,7,0,0,0};
u8 bitmap_65[] = {0,0,0,0,128,240,254,14,62,254,224,0,0,0,0,0,0,0,0,0,128,248,255,127,97,96,96,99,127,254,224,0,0,0,0,0,0,8,15,15,1,0,0,0,0,0,0,3,15,15,0,0,0,0};
u8 bitmap_66[] = {0,0,254,254,254,6,6,6,6,14,14,252,248,32,0,0,0,0,0,0,255,255,255,6,6,6,6,6,15,253,249,240,0,0,0,0,0,0,15,15,15,12,12,12,12,12,14,7,3,1,0,0,0,0};
u8 bitmap_67[] = {0,0,224,248,252,14,6,6,6,6,14,60,56,32,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,128,128,128,0,0,0,0,0,0,0,3,7,14,12,12,12,12,14,7,3,0,0,0,0,0};
u8 bitmap_68[] = {0,0,254,254,254,6,6,6,6,14,28,252,248,192,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,127,0,0,0,0,0,0,15,15,15,12,12,12,12,14,7,7,3,0,0,0,0,0};
u8 bitmap_69[] = {0,0,254,254,254,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,255,255,255,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,15,15,15,12,12,12,12,12,12,12,12,12,0,0,0,0};
u8 bitmap_70[] = {0,0,254,254,254,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,255,255,255,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,15,15,15,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_71[] = {0,0,240,248,60,14,6,6,6,14,14,60,56,32,0,0,0,0,0,0,255,255,128,0,0,12,12,12,12,252,252,124,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_72[] = {0,0,254,254,254,0,0,0,0,0,0,254,254,0,0,0,0,0,0,0,255,255,255,6,6,6,6,6,6,255,255,0,0,0,0,0,0,0,15,15,15,0,0,0,0,0,0,15,15,0,0,0,0,0};
u8 bitmap_73[] = {0,0,6,6,6,6,6,254,254,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,12,12,12,12,12,15,15,12,12,12,12,0,0,0,0,0};
u8 bitmap_74[] = {0,0,0,6,6,6,6,6,6,6,6,254,254,0,0,0,0,0,0,128,128,128,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,3,7,14,12,12,12,12,14,7,3,1,0,0,0,0,0};
u8 bitmap_75[] = {0,0,254,254,254,0,0,0,128,224,240,124,30,6,2,0,0,0,0,0,255,255,255,6,6,6,31,127,248,224,128,0,0,0,0,0,0,0,15,15,15,0,0,0,0,0,1,7,15,14,8,0,0,0};
u8 bitmap_76[] = {0,0,0,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,12,12,12,12,12,12,12,12,12,0,0,0,0};
u8 bitmap_77[] = {0,0,254,254,14,248,192,0,0,224,124,230,254,254,0,0,0,0,0,0,255,255,0,0,7,14,15,3,0,255,255,255,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0,15,15,15,0,0,0,0};
u8 bitmap_78[] = {0,0,254,254,14,124,240,128,0,0,0,254,254,0,0,0,0,0,0,0,255,255,0,0,3,15,126,240,128,255,255,0,0,0,0,0,0,0,15,15,0,0,0,0,0,3,15,15,15,0,0,0,0,0};
u8 bitmap_79[] = {0,0,240,248,60,14,6,6,6,14,12,252,248,192,0,0,0,0,0,0,255,255,128,0,0,0,0,0,0,255,255,127,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_80[] = {0,0,254,254,254,6,6,6,6,6,14,28,252,248,0,0,0,0,0,0,255,255,255,12,12,12,12,12,14,7,7,1,0,0,0,0,0,0,15,15,15,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_81[] = {0,0,240,248,60,14,6,6,6,14,12,252,248,192,0,0,0,0,0,0,255,255,128,0,0,0,0,0,0,255,255,127,0,0,0,0,0,0,1,3,7,14,12,12,12,62,254,231,131,0,0,0,0,0};
u8 bitmap_82[] = {0,0,254,254,254,6,6,6,6,6,14,188,248,240,0,0,0,0,0,0,255,255,255,12,12,12,28,252,254,199,3,1,0,0,0,0,0,0,15,15,15,0,0,0,0,0,3,15,15,12,0,0,0,0};
u8 bitmap_83[] = {0,0,96,248,252,14,6,6,6,6,14,60,56,32,0,0,0,0,0,0,128,129,3,7,7,6,14,12,28,248,248,224,0,0,0,0,0,0,1,7,7,14,12,12,12,12,14,7,3,1,0,0,0,0};
u8 bitmap_84[] = {0,6,6,6,6,6,6,254,254,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0,0,0};
u8 bitmap_85[] = {0,0,254,254,254,0,0,0,0,0,0,254,254,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_86[] = {0,2,62,254,224,0,0,0,0,0,128,248,254,30,0,0,0,0,0,0,0,3,63,254,224,0,128,248,255,15,0,0,0,0,0,0,0,0,0,0,0,3,15,14,15,15,0,0,0,0,0,0,0,0};
u8 bitmap_87[] = {0,254,254,0,0,0,252,62,254,224,0,0,240,254,62,0,0,0,0,3,255,255,0,252,63,0,1,255,224,0,255,255,0,0,0,0,0,0,15,15,8,15,0,0,0,3,15,15,15,0,0,0,0,0};
u8 bitmap_88[] = {0,2,6,30,124,240,192,0,128,224,248,62,14,2,0,0,0,0,0,0,0,0,192,241,127,31,63,249,224,192,0,0,0,0,0,0,0,8,14,15,3,1,0,0,0,0,1,7,15,12,8,0,0,0};
u8 bitmap_89[] = {0,6,30,124,240,192,0,0,0,128,224,248,62,14,2,0,0,0,0,0,0,0,1,7,31,252,252,15,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0,0,0};
u8 bitmap_90[] = {0,0,6,6,6,6,6,6,134,230,246,62,14,0,0,0,0,0,0,0,0,0,192,240,124,30,7,1,0,0,0,0,0,0,0,0,0,0,14,15,15,12,12,12,12,12,12,12,12,0,0,0,0,0};
u8 bitmap_91[] = {0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,63,48,48,48,48,0,0,0,0,0,0};
u8 bitmap_92[] = {0,0,0,3,31,254,240,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,31,254,240,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,31,126,112,64,0,0,0,0};
u8 bitmap_93[] = {0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,48,48,48,48,63,63,0,0,0,0,0,0,0,0};
u8 bitmap_94[] = {0,0,0,0,224,248,62,7,15,60,240,192,0,0,0,0,0,0,0,0,4,7,3,0,0,0,0,0,1,7,6,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_95[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0};
u8 bitmap_96[] = {0,0,0,0,0,0,1,7,6,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_97[] = {0,0,0,192,192,224,96,96,96,224,192,192,128,0,0,0,0,0,0,0,225,241,57,24,24,24,24,24,24,255,255,0,0,0,0,0,0,0,3,7,14,12,12,12,12,6,3,15,15,0,0,0,0,0};
u8 bitmap_98[] = {0,0,254,254,254,192,64,96,96,224,224,192,128,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,124,0,0,0,0,0,0,15,15,15,6,12,12,12,14,14,7,3,0,0,0,0,0};
u8 bitmap_99[] = {0,0,0,128,192,224,96,96,96,96,224,192,128,0,0,0,0,0,0,0,254,255,255,0,0,0,0,0,0,131,131,130,0,0,0,0,0,0,0,3,7,14,12,12,12,12,14,7,3,0,0,0,0,0};
u8 bitmap_100[] = {0,0,0,192,192,224,96,96,96,192,128,254,254,0,0,0,0,0,0,0,255,255,131,0,0,0,0,0,1,255,255,0,0,0,0,0,0,0,1,7,7,14,12,12,12,6,3,15,15,0,0,0,0,0};
u8 bitmap_101[] = {0,0,0,128,192,224,96,96,96,96,224,192,128,0,0,0,0,0,0,0,255,255,155,24,24,24,24,24,24,31,31,28,0,0,0,0,0,0,1,3,7,14,12,12,12,12,14,7,3,0,0,0,0,0};
u8 bitmap_102[] = {0,0,128,128,128,128,252,254,142,134,134,134,134,134,0,0,0,0,0,0,1,1,1,1,255,255,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_103[] = {0,0,0,192,192,224,96,96,96,192,128,224,224,0,0,0,0,0,0,0,255,255,129,0,0,0,0,0,129,255,255,0,0,0,0,0,0,0,0,3,135,135,134,134,134,194,225,255,127,0,0,0,0,0};
u8 bitmap_104[] = {0,0,254,254,254,192,64,96,96,224,224,192,128,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,1,255,255,0,0,0,0,0,0,0,15,15,15,0,0,0,0,0,0,15,15,0,0,0,0,0};
u8 bitmap_105[] = {0,0,0,96,96,96,98,103,231,231,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,12,12,12,12,12,12,15,15,12,12,12,12,0,0,0,0};
u8 bitmap_106[] = {0,0,0,96,96,96,96,96,103,231,231,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,128,128,128,128,128,192,224,127,63,0,0,0,0,0,0,0};
u8 bitmap_107[] = {0,0,254,254,254,0,0,0,0,0,128,192,224,96,32,0,0,0,0,0,255,255,255,48,48,48,124,254,199,3,0,0,0,0,0,0,0,0,15,15,15,0,0,0,0,1,3,7,14,12,8,0,0,0};
u8 bitmap_108[] = {0,6,6,6,6,6,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,7,14,12,12,12,12,12,0,0,0,0};
u8 bitmap_109[] = {0,0,224,224,64,96,224,192,0,192,96,224,224,192,0,0,0,0,0,0,255,255,0,0,0,255,255,0,0,0,255,255,0,0,0,0,0,0,15,15,0,0,0,15,15,0,0,0,15,15,0,0,0,0};
u8 bitmap_110[] = {0,0,224,224,224,192,64,96,96,224,224,192,128,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,1,255,255,0,0,0,0,0,0,0,15,15,15,0,0,0,0,0,0,15,15,0,0,0,0,0};
u8 bitmap_111[] = {0,0,0,128,192,224,96,96,96,224,192,192,128,0,0,0,0,0,0,0,255,255,131,0,0,0,0,0,0,255,255,124,0,0,0,0,0,0,1,3,7,14,12,12,12,14,6,7,3,0,0,0,0,0};
u8 bitmap_112[] = {0,0,224,224,224,192,64,96,96,224,224,192,128,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,124,0,0,0,0,0,0,255,255,255,6,12,12,12,14,14,7,3,0,0,0,0,0};
u8 bitmap_113[] = {0,0,0,128,192,224,96,96,96,192,128,224,224,224,0,0,0,0,0,0,254,255,255,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,3,7,14,12,12,12,6,2,255,255,255,0,0,0,0};
u8 bitmap_114[] = {0,0,0,224,224,128,192,96,96,96,224,224,192,128,0,0,0,0,0,0,0,255,255,1,0,0,0,0,0,1,7,7,0,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_115[] = {0,0,0,192,192,224,96,96,96,96,224,192,128,0,0,0,0,0,0,0,3,15,31,24,24,24,56,48,48,241,225,0,0,0,0,0,0,0,3,7,7,14,12,12,12,12,14,7,3,0,0,0,0,0};
u8 bitmap_116[] = {0,0,96,96,96,96,254,254,96,96,96,96,96,96,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,14,12,12,12,12,0,0,0,0,0};
u8 bitmap_117[] = {0,0,224,224,224,0,0,0,0,0,0,224,224,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,3,7,14,12,12,12,14,7,7,3,0,0,0,0,0};
u8 bitmap_118[] = {0,32,224,224,128,0,0,0,0,0,0,192,224,96,0,0,0,0,0,0,0,7,63,252,224,0,128,240,254,31,3,0,0,0,0,0,0,0,0,0,0,1,15,15,15,7,0,0,0,0,0,0,0,0};
u8 bitmap_119[] = {0,224,224,0,0,0,192,224,224,0,0,0,128,224,96,0,0,0,0,3,255,252,0,224,255,1,15,254,0,128,255,127,0,0,0,0,0,0,3,15,8,15,0,0,0,7,15,15,15,0,0,0,0,0};
u8 bitmap_120[] = {0,0,32,224,224,128,0,0,0,0,192,224,96,32,0,0,0,0,0,0,0,0,129,199,255,124,126,239,195,1,0,0,0,0,0,0,0,8,12,14,7,3,0,0,0,1,3,15,14,8,0,0,0,0};
u8 bitmap_121[] = {0,32,224,224,128,0,0,0,0,0,0,192,224,96,0,0,0,0,0,0,0,7,31,254,240,128,128,240,254,31,3,0,0,0,0,0,0,0,0,0,0,128,243,255,63,7,0,0,0,0,0,0,0,0};
u8 bitmap_122[] = {0,0,96,96,96,96,96,96,96,96,96,224,224,0,0,0,0,0,0,0,0,0,192,224,112,56,28,14,7,3,1,0,0,0,0,0,0,0,14,15,15,13,12,12,12,12,12,12,12,12,0,0,0,0};
u8 bitmap_123[] = {0,0,0,0,0,0,0,135,255,255,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,7,253,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,31,31,56,48,48,0,0,0,0,0};
u8 bitmap_124[] = {0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,127,0,0,0,0,0,0,0,0,0};
u8 bitmap_125[] = {0,0,0,0,0,0,255,255,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,255,7,3,3,3,3,0,0,0,0,0,0,0,48,48,48,56,31,15,0,0,0,0,0,0,0,0,0,0};
u8 bitmap_126[] = {0,0,0,128,128,128,128,0,0,0,0,0,128,128,0,0,0,0,0,0,15,15,1,1,3,7,14,12,12,14,15,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 *char_addr[] = {bitmap_32,bitmap_33,bitmap_34,bitmap_35,bitmap_36,bitmap_37,bitmap_38,bitmap_39,bitmap_40,bitmap_41,bitmap_42,bitmap_43,bitmap_44,bitmap_45,bitmap_46,bitmap_47,bitmap_48,bitmap_49,bitmap_50,bitmap_51,bitmap_52,bitmap_53,bitmap_54,bitmap_55,bitmap_56,bitmap_57,bitmap_58,bitmap_59,bitmap_60,bitmap_61,bitmap_62,bitmap_63,bitmap_64,bitmap_65,bitmap_66,bitmap_67,bitmap_68,bitmap_69,bitmap_70,bitmap_71,bitmap_72,bitmap_73,bitmap_74,bitmap_75,bitmap_76,bitmap_77,bitmap_78,bitmap_79,bitmap_80,bitmap_81,bitmap_82,bitmap_83,bitmap_84,bitmap_85,bitmap_86,bitmap_87,bitmap_88,bitmap_89,bitmap_90,bitmap_91,bitmap_92,bitmap_93,bitmap_94,bitmap_95,bitmap_96,bitmap_97,bitmap_98,bitmap_99,bitmap_100,bitmap_101,bitmap_102,bitmap_103,bitmap_104,bitmap_105,bitmap_106,bitmap_107,bitmap_108,bitmap_109,bitmap_110,bitmap_111,bitmap_112,bitmap_113,bitmap_114,bitmap_115,bitmap_116,bitmap_117,bitmap_118,bitmap_119,bitmap_120,bitmap_121,bitmap_122,bitmap_123,bitmap_124,bitmap_125,bitmap_126};



typedef struct KeyMap      { u8 ctrl, alt, shift, space, tab; } KeyMap;
typedef struct IsPressed { bool ctrl, alt, shift, space, tab; } IsPressed;
typedef struct Controls {
    IsPressed is_pressed;
    KeyMap key_map;
    Mouse mouse;
} Controls;

typedef u64 (*GetTicks)();

typedef struct PerTick {
    f64 seconds, milliseconds, microseconds, nanoseconds;
} PerTick;

typedef struct Ticks {
    PerTick per_tick;
    u64 per_second;
} Ticks;

typedef struct Timer {
    GetTicks getTicks;
    Ticks *ticks;
    f32 delta_time;
    u64 ticks_before,
            ticks_after,
            ticks_diff,
            accumulated_ticks,
            accumulated_frame_count,
            ticks_of_last_report,
            seconds,
            milliseconds,
            microseconds,
            nanoseconds;
    f64 average_frames_per_tick,
            average_ticks_per_frame;
    u16 average_frames_per_second,
            average_milliseconds_per_frame,
            average_microseconds_per_frame,
            average_nanoseconds_per_frame;
} Timer;

typedef struct Timers {
    Timer update, render, aux;
} Timers;

typedef struct Time {
    Timers timers;
    Ticks ticks;
    GetTicks getTicks;
} Time;

typedef struct Curve {
    f32 thickness;
    u32 revolution_count;
} Curve;

typedef enum BoxSide {
    NoSide = 0,
    Top    = 1,
    Bottom = 2,
    Left   = 4,
    Right  = 8,
    Front  = 16,
    Back   = 32
} BoxSide;

typedef struct BoxCorners {
    vec3 front_top_left,
            front_top_right,
            front_bottom_left,
            front_bottom_right,
            back_top_left,
            back_top_right,
            back_bottom_left,
            back_bottom_right;
} BoxCorners;

typedef union BoxVertices {
    BoxCorners corners;
    vec3 buffer[BOX__VERTEX_COUNT];
} BoxVertices;

typedef struct BoxEdgeSides {
    Edge front_top,
            front_bottom,
            front_left,
            front_right,
            back_top,
            back_bottom,
            back_left,
            back_right,
            left_bottom,
            left_top,
            right_bottom,
            right_top;
} BoxEdgeSides;

typedef union BoxEdges {
    BoxEdgeSides sides;
    Edge buffer[BOX__EDGE_COUNT];
} BoxEdges;

typedef struct Box {
    BoxVertices vertices;
    BoxEdges edges;
} Box;

typedef struct GridUVEdges {
    Edge u[GRID__MAX_SEGMENTS];
    Edge v[GRID__MAX_SEGMENTS];
} GridUVEdges;

typedef union GridEdges {
    GridUVEdges uv;
    Edge buffer[2][GRID__MAX_SEGMENTS];
} GridEdges;

typedef struct GridSideVertices {
    vec3 from[GRID__MAX_SEGMENTS];
    vec3 to[  GRID__MAX_SEGMENTS];
} GridSideVertices;

typedef struct GridUVVertices {
    GridSideVertices u, v;
} GridUVVertices;

typedef union GridVertices {
    GridUVVertices uv;
    vec3 buffer[2][2][GRID__MAX_SEGMENTS];
} GridVertices;

typedef struct Grid {
    GridEdges edges;
    GridVertices vertices;
    u8 u_segments,
            v_segments;
} Grid;

enum PrimitiveType {
    PrimitiveType_None = 0,
    PrimitiveType_Mesh,
    PrimitiveType_Grid,
    PrimitiveType_Box,
    PrimitiveType_Helix,
    PrimitiveType_Coil,
    PrimitiveType_Tetrahedron
};

typedef struct Primitive {
    quat rotation;
    vec3 position, scale;
    u32 id;
    enum PrimitiveType type;
    enum ColorID color;
    u8 flags, material_id;
} Primitive;

typedef struct xform3 {
    mat3 matrix,
            yaw_matrix,
            pitch_matrix,
            roll_matrix,
            rotation_matrix,
            rotation_matrix_inverted;
    quat rotation,
            rotation_inverted;
    vec3 position, scale,
            *up_direction,
            *right_direction,
            *forward_direction;
} xform3;

typedef struct Camera {
    xform3 transform;
    vec3 current_velocity;
    f32 focal_length, zoom, dolly, target_distance;
} Camera;

typedef struct NavigationSpeedSettings {
    f32 turn, zoom, dolly, pan, orbit, orient;
} NavigationSpeedSettings;

typedef struct NavigationSettings {
    NavigationSpeedSettings speeds;
    f32 max_velocity, acceleration;
} NavigationSettings;

typedef struct NavigationTurn {
    bool right, left;
} NavigationTurn;

typedef struct NavigationMove {
    bool right, left, up, down, forward, backward;
} NavigationMove;

typedef struct Navigation {
    NavigationSettings settings;
    NavigationMove move;
    NavigationTurn turn;
    bool zoomed, moved, turned;
} Navigation;

typedef struct ProjectionPlane {
    vec3 start, right, down;
} ProjectionPlane;


typedef struct ViewportSettings {
    Pixel background;
    f32 near_clipping_plane_distance,
            far_clipping_plane_distance;
    u32 hud_line_count;
    HUDLine *hud_lines;
    enum ColorID hud_default_color;
    bool show_hud, use_cube_NDC, flip_z, antialias, cull_back_faces, show_wire_frame;
} ViewportSettings;

typedef struct Viewport {
    ViewportSettings settings;
    Dimensions dimensions;
    Navigation navigation;
    ProjectionPlane projection_plane;
    HUD hud;
    Box default_box;
    vec2i position;
    mat4 projection_matrix;
    Camera *camera;
    PixelQuad *pixels;
} Viewport;

typedef struct Ray {
    vec3 origin, direction;
} Ray;

typedef struct RayHit {
    vec3 position, normal;
    f32 distance, distance_squared;
    u32 object_id, object_type;
    bool from_behind;
} RayHit;

// Mesh:
// =====
typedef struct EdgeVertexIndices { u32 from, to; } EdgeVertexIndices;
typedef union TriangleVertexIndices { u32 ids[3]; struct { u32 v1, v2, v3; }; } TriangleVertexIndices;
typedef struct Mesh {
    AABB aabb;
    u32 triangle_count, vertex_count, edge_count, normals_count, uvs_count;
    vec3 *vertex_positions, *vertex_normals;
    vec2 *vertex_uvs;
    TriangleVertexIndices *vertex_position_indices;
    TriangleVertexIndices *vertex_normal_indices;
    TriangleVertexIndices *vertex_uvs_indices;
    EdgeVertexIndices     *edge_vertex_indices;
} Mesh;

typedef union TexelQuad {
    struct {vec4 TL, TR, BL, BR;};
    vec4 quadrants[4];
} TexelQuad;

typedef struct TextureMip {
    u16 width, height;
    vec4 *texels;
    TexelQuad *texel_quads;
    TexelQuad **texel_quad_lines;
} TextureMip;

typedef struct Texture {
    u16 width, height;
    u8 mip_count;
    bool wrap, mipmap, filter;
    TextureMip *mips;
} Texture;

// Materials:
// =========
typedef struct Material Material;

enum BRDFType {
    phong,
    ggx
};
typedef struct Scene Scene;
typedef struct Shaded Shaded;
typedef struct PixelShaderInputs {
    vec2 UV, dUV;
    vec2u coords;
    f32 depth;
} PixelShaderInputs;

typedef struct PixelShaderOutputs {
    vec3 color;
    f32 opacity;
    f64 z;
} PixelShaderOutputs;

typedef struct Rasterizer {
    Mesh default_cube_mesh;
    mat4 model_to_world_inverted_transposed, model_to_world, world_to_clip;
    vec4 *clip_space_vertex_positions;
    vec3 *world_space_vertex_positions, *world_space_vertex_normals;
    u8 *vertex_flags;
} Rasterizer;

typedef void (*PixelShader)(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs);
typedef u8 (  *MeshShader )(Mesh *mesh, Rasterizer *rasterizer);

typedef struct Material {
    vec3 ambient, diffuse, specular;
    f32 shininess, roughness;
    u8 texture_count, texture_ids[16];
    PixelShader pixel_shader;
    MeshShader mesh_shader;
    enum BRDFType brdf;
    u8 flags;
} Material;

typedef struct MaterialHas {
    bool diffuse, specular;
} MaterialHas;

typedef struct MaterialUses {
    bool blinn, phong;
} MaterialUses;

INLINE void decodeMaterialSpec(u8 flags, MaterialHas *has, MaterialUses *uses) {
    uses->phong = flags & (u8)PHONG;
    uses->blinn = flags & (u8)BLINN;
    has->diffuse = flags & (u8)LAMBERT;
    has->specular = uses->phong || uses->blinn;
}

typedef struct Shaded {
    Primitive *primitive;
    Material *material;
    MaterialHas has;
    MaterialUses uses;
    vec3 position, normal, viewing_direction, viewing_origin, reflected_direction, light_direction, diffuse;
} Shaded;

// Lights:
// ======
typedef struct AmbientLight{
    vec3 color;
} AmbientLight;

typedef struct Light {
    vec3 attenuation, position_or_direction, color;
    f32 intensity;
    bool is_directional;
} Light;
#define CUBE__UV_COUNT 4
#define CUBE__NORMAL_COUNT 6
#define CUBE__VERTEX_COUNT 8
#define CUBE__TRIANGLE_COUNT 12

static const vec3 CUBE__VERTEX_POSITIONS[CUBE__VERTEX_COUNT] = {
        {-1, -1, -1},
        {1, -1, -1},
        {1, 1, -1},
        {-1, 1, -1},
        {-1, -1, 1},
        {1, -1, 1},
        {1, 1, 1},
        {-1, 1, 1}
};

static const TriangleVertexIndices CUBE__VERTEX_POSITION_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 1, 2},
        {1, 5, 6},
        {5, 4, 7},
        {4, 0, 3},
        {3, 2, 6},
        {1, 0, 4},
        {0, 2, 3},
        {1, 6, 2},
        {5, 7, 6},
        {4, 3, 7},
        {3, 6, 7},
        {1, 4, 5}
};

static const vec3 CUBE__VERTEX_NORMALS[CUBE__NORMAL_COUNT] = {
        {0, 0, -1},
        {1, 0, 0},
        {0, 0, 1},
        {-1, 0, 0},
        {0, 1, 0},
        {0, -1, 0}
};
static const TriangleVertexIndices CUBE__VERTEX_NORMAL_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 0, 0},
        {1, 1, 1},
        {2, 2, 2},
        {3, 3, 3},
        {4, 4, 4},
        {5, 5, 5},
        {0, 0, 0},
        {1, 1, 1},
        {2, 2, 2},
        {3, 3, 3},
        {4, 4, 4},
        {5, 5, 5}
};

static const vec2 CUBE__VERTEX_UVS[CUBE__UV_COUNT] = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0},
};
static const TriangleVertexIndices CUBE__VERTEX_UV_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3}
};

void setMeshToCube(Mesh *mesh) {
    mesh->triangle_count = CUBE__TRIANGLE_COUNT;
    mesh->vertex_count   = CUBE__VERTEX_COUNT;
    mesh->normals_count  = CUBE__NORMAL_COUNT;
    mesh->uvs_count      = CUBE__UV_COUNT;

    mesh->vertex_uvs       = (vec2*)CUBE__VERTEX_UVS;
    mesh->vertex_normals   = (vec3*)CUBE__VERTEX_NORMALS;
    mesh->vertex_positions = (vec3*)CUBE__VERTEX_POSITIONS;

    mesh->vertex_uvs_indices      = (TriangleVertexIndices*)CUBE__VERTEX_UV_INDICES;
    mesh->vertex_normal_indices   = (TriangleVertexIndices*)CUBE__VERTEX_NORMAL_INDICES;
    mesh->vertex_position_indices = (TriangleVertexIndices*)CUBE__VERTEX_POSITION_INDICES;
}
typedef struct Selection {
    quat object_rotation;
    vec3 transformation_plane_origin,
            transformation_plane_normal,
            transformation_plane_center,
            object_scale,
            world_offset,
            *world_position;
    Box box;
    Ray ray, local_ray;
    RayHit hit, local_hit;
    Primitive *primitive;
    enum BoxSide box_side;
    f32 object_distance;
    u32 object_type, object_id;
    bool changed;
} Selection;

typedef struct SceneSettings {
    u32 cameras, primitives, meshes, curves, boxes, grids, lights, materials, textures;
    String file, *mesh_files, *texture_files;
} SceneSettings;

typedef struct Scene {
    SceneSettings settings;
    AmbientLight ambient_light;
    Light *lights;
    Material *materials;
    Texture *textures;
    Selection *selection;
    Camera *cameras;
    Mesh *meshes;
    Primitive *primitives;
    Curve *curves;
    Grid *grids;
    Box *boxes;
    u64 last_io_ticks;
    bool last_io_is_save;
} Scene;

typedef struct AppCallbacks {
    void (*sceneReady)(Scene *scene);
    void (*viewportReady)(Viewport *viewport);
    void (*windowRedraw)();
    void (*windowResize)(u16 width, u16 height);
    void (*keyChanged)(  u8 key, bool pressed);
    void (*mouseButtonUp)(  MouseButton *mouse_button);
    void (*mouseButtonDown)(MouseButton *mouse_button);
    void (*mouseButtonDoubleClicked)(MouseButton *mouse_button);
    void (*mouseWheelScrolled)(f32 amount);
    void (*mousePositionSet)(i32 x, i32 y);
    void (*mouseMovementSet)(i32 x, i32 y);
    void (*mouseRawMovementSet)(i32 x, i32 y);
} AppCallbacks;

typedef void* (*CallbackForFileOpen)(const char* file_path);
typedef bool  (*CallbackForFileRW)(void *out, unsigned long, void *handle);
typedef void  (*CallbackForFileClose)(void *handle);

typedef struct Platform {
    GetTicks             getTicks;
    CallbackWithInt      getMemory;
    CallbackWithCharPtr  setWindowTitle;
    CallbackWithBool     setWindowCapture;
    CallbackWithBool     setCursorVisibility;
    CallbackForFileClose closeFile;
    CallbackForFileOpen  openFileForReading;
    CallbackForFileOpen  openFileForWriting;
    CallbackForFileRW    readFromFile;
    CallbackForFileRW    writeToFile;
    u64 ticks_per_second;
} Platform;

typedef struct Settings {
    SceneSettings scene;
    ViewportSettings viewport;
    NavigationSettings navigation;
} Settings;

typedef struct Defaults {
    char* title;
    u16 width, height;
    u64 additional_memory_size;
    Settings settings;
} Defaults;

typedef struct App {
    Memory memory;
    Platform platform;
    Controls controls;
    Rasterizer rasterizer;
    u32 *window_content;
    AppCallbacks on;
    Time time;
    Scene scene;
    Viewport viewport;
    bool is_running;
    void *user_data;
} App;


void setBoxEdgesFromVertices(BoxEdges *edges, BoxVertices *vertices) {
    edges->sides.front_top.from    = vertices->corners.front_top_left;
    edges->sides.front_top.to      = vertices->corners.front_top_right;
    edges->sides.front_bottom.from = vertices->corners.front_bottom_left;
    edges->sides.front_bottom.to   = vertices->corners.front_bottom_right;
    edges->sides.front_left.from   = vertices->corners.front_bottom_left;
    edges->sides.front_left.to     = vertices->corners.front_top_left;
    edges->sides.front_right.from  = vertices->corners.front_bottom_right;
    edges->sides.front_right.to    = vertices->corners.front_top_right;

    edges->sides.back_top.from     = vertices->corners.back_top_left;
    edges->sides.back_top.to       = vertices->corners.back_top_right;
    edges->sides.back_bottom.from  = vertices->corners.back_bottom_left;
    edges->sides.back_bottom.to    = vertices->corners.back_bottom_right;
    edges->sides.back_left.from    = vertices->corners.back_bottom_left;
    edges->sides.back_left.to      = vertices->corners.back_top_left;
    edges->sides.back_right.from   = vertices->corners.back_bottom_right;
    edges->sides.back_right.to     = vertices->corners.back_top_right;

    edges->sides.left_bottom.from  = vertices->corners.front_bottom_left;
    edges->sides.left_bottom.to    = vertices->corners.back_bottom_left;
    edges->sides.left_top.from     = vertices->corners.front_top_left;
    edges->sides.left_top.to       = vertices->corners.back_top_left;
    edges->sides.right_bottom.from = vertices->corners.front_bottom_right;
    edges->sides.right_bottom.to   = vertices->corners.back_bottom_right;
    edges->sides.right_top.from    = vertices->corners.front_top_right;
    edges->sides.right_top.to      = vertices->corners.back_top_right;
}

void setGridEdgesFromVertices(Edge *edges, u8 edge_count, vec3 *from, vec3 *to) {
    for (u8 i = 0; i < edge_count; i++) {
        edges[i].from = from[i];
        edges[i].to   = to[i];
    }
}

INLINE void setPixel(i32 x, i32 y, f64 depth, vec3 color, f32 opacity, Viewport *viewport) {
    Pixel *pixel;
    PixelQuad *pixel_quad;
    if (viewport->settings.antialias) {
        pixel_quad = viewport->pixels + (viewport->dimensions.stride * (y >> 1)) + (x >> 1);
        pixel = &pixel_quad->quad[y & 1][x & 1];
    } else {
        pixel_quad = viewport->pixels + (viewport->dimensions.stride * y) + x;
        pixel = &pixel_quad->TL;
    }

    Pixel new_pixel;
    new_pixel.opacity = opacity;
    new_pixel.color = color;
    new_pixel.depth = depth;

    if (!(opacity == 1 && depth == 0)) {
        Pixel background, foreground, old_pixel = *pixel;

        if (old_pixel.depth < new_pixel.depth) {
            background = new_pixel;
            foreground = old_pixel;
        } else {
            background = old_pixel;
            foreground = new_pixel;
        }
        if (foreground.opacity != 1) {
            f32 one_minus_foreground_opacity = 1.0f - foreground.opacity;
            opacity = foreground.opacity + background.opacity * one_minus_foreground_opacity;
            f32 one_over_opacity = opacity ? 1.0f / opacity : 1;
            f32 background_factor = background.opacity * one_over_opacity * one_minus_foreground_opacity;
            f32 foreground_factor = foreground.opacity * one_over_opacity;

            pixel->color.r = fast_mul_add(foreground.color.r, foreground_factor, background.color.r * background_factor);
            pixel->color.g = fast_mul_add(foreground.color.g, foreground_factor, background.color.g * background_factor);
            pixel->color.b = fast_mul_add(foreground.color.b, foreground_factor, background.color.b * background_factor);
            pixel->opacity = opacity;
            pixel->depth   = foreground.depth;
        } else *pixel = foreground;
    } else *pixel = new_pixel;

    if (!viewport->settings.antialias) pixel_quad->BR = pixel_quad->BL = pixel_quad->TR = pixel_quad->TL;
}



void drawText(char *str, i32 x, i32 y, vec3 color, f32 opacity, Viewport *viewport) {
    if (x < 0 || x > viewport->dimensions.width  - FONT_WIDTH ||
        y < 0 || y > viewport->dimensions.height - FONT_HEIGHT)
        return;

    color.r *= color.r;
    color.g *= color.g;
    color.b *= color.b;
    bool AA = viewport->settings.antialias;
    u16 current_x = (u16)x;
    u16 current_y = (u16)y;
    u16 pixel_x, sub_pixel_x;
    u16 pixel_y, sub_pixel_y;
    u16 t_offset;
    u8* byte;
    char character = *str;
    while (character) {
        if (character == '\n') {
            if (current_y + FONT_HEIGHT > viewport->dimensions.height)
                break;

            current_x = (u16)x;
            current_y += LINE_HEIGHT;
        } else if (character == '\t') {
            t_offset = FONT_WIDTH * (4 - ((current_x / FONT_WIDTH) & 3));
            current_x += t_offset;
        } else if (character >= FIRST_CHARACTER_CODE &&
                   character <= LAST_CHARACTER_CODE) {
            byte = char_addr[character - FIRST_CHARACTER_CODE];
            for (int i = 1; i < 4; i++) {
                pixel_x = current_x;
                pixel_y = current_y + i * FONT_HEIGHT / 3;
                for (int w = 0; w < FONT_WIDTH ; w++) {
                    for (int h = 0; h < FONT_HEIGHT/3; h++) {
                        /* skip background bits */
                        if (*byte & (0x80  >> h)) {
                            if (AA) {
                                sub_pixel_x = pixel_x << 1;
                                sub_pixel_y = pixel_y << 1;
                                setPixel(sub_pixel_x + 0, sub_pixel_y + 0, 0, color, opacity, viewport);
                                setPixel(sub_pixel_x + 1, sub_pixel_y + 0, 0, color, opacity, viewport);
                                setPixel(sub_pixel_x + 0, sub_pixel_y + 1, 0, color, opacity, viewport);
                                setPixel(sub_pixel_x + 1, sub_pixel_y + 1, 0, color, opacity, viewport);
                            } else
                                setPixel(pixel_x, pixel_y, 0, color, opacity, viewport);
                        }

                        pixel_y--;
                    }
                    byte++;
                    pixel_y += FONT_HEIGHT / 3;
                    pixel_x++;
                }
            }
            current_x += FONT_WIDTH;
            if (current_x + FONT_WIDTH > viewport->dimensions.width)
                return;
        }
        character = *++str;
    }
}

void drawNumber(i32 number, i32 x, i32 y, vec3 color, f32 opacity, Viewport *viewport) {
    static NumberString number_string;
    printNumberIntoString(number, &number_string);
    drawText(number_string.string.char_ptr, x - number_string.string.length * FONT_WIDTH, y, color, opacity, viewport);
}



void accumulateTimer(Timer* timer) {
    timer->ticks_diff = timer->ticks_after - timer->ticks_before;
    timer->accumulated_ticks += timer->ticks_diff;
    timer->accumulated_frame_count++;

    timer->seconds      = (u64)(timer->ticks->per_tick.seconds      * (f64)(timer->ticks_diff));
    timer->milliseconds = (u64)(timer->ticks->per_tick.milliseconds * (f64)(timer->ticks_diff));
    timer->microseconds = (u64)(timer->ticks->per_tick.microseconds * (f64)(timer->ticks_diff));
    timer->nanoseconds  = (u64)(timer->ticks->per_tick.nanoseconds  * (f64)(timer->ticks_diff));
}

void averageTimer(Timer *timer) {
    timer->average_frames_per_tick = (f64)timer->accumulated_frame_count / (f64)timer->accumulated_ticks;
    timer->average_ticks_per_frame = (f64)timer->accumulated_ticks / (f64)timer->accumulated_frame_count;
    timer->average_frames_per_second = (u16)(timer->average_frames_per_tick      * (f64)timer->ticks->per_second);
    timer->average_milliseconds_per_frame = (u16)(timer->average_ticks_per_frame * timer->ticks->per_tick.milliseconds);
    timer->average_microseconds_per_frame = (u16)(timer->average_ticks_per_frame * timer->ticks->per_tick.microseconds);
    timer->average_nanoseconds_per_frame = (u16)(timer->average_ticks_per_frame  * timer->ticks->per_tick.nanoseconds);
    timer->accumulated_ticks = timer->accumulated_frame_count = 0;
}

INLINE void beginFrameTimer(Timer *timer) {
    timer->ticks_after = timer->ticks_before;
    timer->ticks_before = timer->getTicks();
    timer->ticks_diff = timer->ticks_before - timer->ticks_after;
    timer->delta_time = (f32)((f64)timer->ticks_diff * timer->ticks->per_tick.seconds);
}

INLINE void endFrameTimer(Timer *timer) {
    timer->ticks_after = timer->getTicks();
    accumulateTimer(timer);
    if (timer->accumulated_ticks >= timer->ticks->per_second / 4)
        averageTimer(timer);
}

void beginFrame(Timer *timer) {
    beginFrameTimer(timer);
}

void endFrame(Timer *timer, Mouse *mouse) {
    resetMouseChanges(mouse);
    endFrameTimer(timer);
}



void initMouse(Mouse *mouse) {
    mouse->is_captured = false;

    mouse->moved = false;
    mouse->move_handled = false;

    mouse->double_clicked = false;
    mouse->double_clicked_handled = false;

    mouse->wheel_scrolled = false;
    mouse->wheel_scroll_amount = 0;
    mouse->wheel_scroll_handled = false;

    mouse->pos.x = 0;
    mouse->pos.y = 0;
    mouse->pos_raw_diff.x = 0;
    mouse->pos_raw_diff.y = 0;
    mouse->raw_movement_handled = false;

    mouse->middle_button.is_pressed = false;
    mouse->middle_button.is_handled = false;
    mouse->middle_button.up_pos.x = 0;
    mouse->middle_button.down_pos.x = 0;

    mouse->right_button.is_pressed = false;
    mouse->right_button.is_handled = false;
    mouse->right_button.up_pos.x = 0;
    mouse->right_button.down_pos.x = 0;

    mouse->left_button.is_pressed = false;
    mouse->left_button.is_handled = false;
    mouse->left_button.up_pos.x = 0;
    mouse->left_button.down_pos.x = 0;
}

void initTimer(Timer *timer, GetTicks getTicks, Ticks *ticks) {
    timer->getTicks = getTicks;
    timer->ticks    = ticks;

    timer->delta_time = 0;
    timer->ticks_before = 0;
    timer->ticks_after = 0;
    timer->ticks_diff = 0;

    timer->accumulated_ticks = 0;
    timer->accumulated_frame_count = 0;

    timer->ticks_of_last_report = 0;

    timer->seconds = 0;
    timer->milliseconds = 0;
    timer->microseconds = 0;
    timer->nanoseconds = 0;

    timer->average_frames_per_tick = 0;
    timer->average_ticks_per_frame = 0;
    timer->average_frames_per_second = 0;
    timer->average_milliseconds_per_frame = 0;
    timer->average_microseconds_per_frame = 0;
    timer->average_nanoseconds_per_frame = 0;
}

void initTime(Time *time, GetTicks getTicks, u64 ticks_per_second) {
    time->getTicks = getTicks;
    time->ticks.per_second = ticks_per_second;

    time->ticks.per_tick.seconds      = 1          / (f64)(time->ticks.per_second);
    time->ticks.per_tick.milliseconds = 1000       / (f64)(time->ticks.per_second);
    time->ticks.per_tick.microseconds = 1000000    / (f64)(time->ticks.per_second);
    time->ticks.per_tick.nanoseconds  = 1000000000 / (f64)(time->ticks.per_second);

    initTimer(&time->timers.update, getTicks, &time->ticks);
    initTimer(&time->timers.render, getTicks, &time->ticks);
    initTimer(&time->timers.aux,    getTicks, &time->ticks);

    time->timers.update.ticks_before = time->timers.update.ticks_of_last_report = getTicks();
}

void initXform3(xform3 *xform) {
    mat3 I;
    I.X.x = 1; I.Y.x = 0; I.Z.x = 0;
    I.X.y = 0; I.Y.y = 1; I.Z.y = 0;
    I.X.z = 0; I.Y.z = 0; I.Z.z = 1;
    xform->matrix = xform->yaw_matrix = xform->pitch_matrix = xform->roll_matrix = xform->rotation_matrix = xform->rotation_matrix_inverted = I;
    xform->right_direction   = &xform->rotation_matrix.X;
    xform->up_direction      = &xform->rotation_matrix.Y;
    xform->forward_direction = &xform->rotation_matrix.Z;
    xform->scale.x = 1;
    xform->scale.y = 1;
    xform->scale.z = 1;
    xform->position.x = 0;
    xform->position.y = 0;
    xform->position.z = 0;
    xform->rotation.axis.x = 0;
    xform->rotation.axis.y = 0;
    xform->rotation.axis.z = 0;
    xform->rotation.amount = 1;
    xform->rotation_inverted = xform->rotation;
}

void initCamera(Camera* camera) {
    camera->focal_length = camera->zoom = CAMERA_DEFAULT__FOCAL_LENGTH;
    camera->target_distance = CAMERA_DEFAULT__TARGET_DISTANCE;
    camera->dolly = 0;
    camera->current_velocity.x = 0;
    camera->current_velocity.y = 0;
    camera->current_velocity.z = 0;
    initXform3(&camera->transform);
}

void initBox(Box *box) {
    box->vertices.corners.front_top_left.x    = -1;
    box->vertices.corners.back_top_left.x     = -1;
    box->vertices.corners.front_bottom_left.x = -1;
    box->vertices.corners.back_bottom_left.x  = -1;

    box->vertices.corners.front_top_right.x    = 1;
    box->vertices.corners.back_top_right.x     = 1;
    box->vertices.corners.front_bottom_right.x = 1;
    box->vertices.corners.back_bottom_right.x  = 1;


    box->vertices.corners.front_bottom_left.y  = -1;
    box->vertices.corners.front_bottom_right.y = -1;
    box->vertices.corners.back_bottom_left.y   = -1;
    box->vertices.corners.back_bottom_right.y  = -1;

    box->vertices.corners.front_top_left.y  = 1;
    box->vertices.corners.front_top_right.y = 1;
    box->vertices.corners.back_top_left.y   = 1;
    box->vertices.corners.back_top_right.y  = 1;


    box->vertices.corners.front_top_left.z     = 1;
    box->vertices.corners.front_top_right.z    = 1;
    box->vertices.corners.front_bottom_left.z  = 1;
    box->vertices.corners.front_bottom_right.z = 1;

    box->vertices.corners.back_top_left.z     = -1;
    box->vertices.corners.back_top_right.z    = -1;
    box->vertices.corners.back_bottom_left.z  = -1;
    box->vertices.corners.back_bottom_right.z = -1;

    setBoxEdgesFromVertices(&box->edges, &box->vertices);
}

void initHUD(HUD *hud, HUDLine *lines, u32 line_count, f32 line_height, enum ColorID default_color, i32 position_x, i32 position_y) {
    hud->lines = lines;
    hud->line_count = line_count;
    hud->line_height = line_height;
    hud->position.x = position_x;
    hud->position.y = position_y;

    if (lines) {
        HUDLine *line = lines;
        for (u32 i = 0; i < line_count; i++, line++) {
            line->use_alternate = null;
            line->invert_alternate_use = false;
            line->title_color = line->value_color = line->alternate_value_color = default_color;
            initNumberString(&line->value);
            line->title.char_ptr = line->alternate_value.char_ptr = (char*)("");
            line->title.length = line->alternate_value.length = 0;
        }
    }
}

void setDefaultNavigationSettings(NavigationSettings *settings) {
    settings->max_velocity  = NAVIGATION_DEFAULT__MAX_VELOCITY;
    settings->acceleration  = NAVIGATION_DEFAULT__ACCELERATION;
    settings->speeds.turn   = NAVIGATION_SPEED_DEFAULT__TURN;
    settings->speeds.orient = NAVIGATION_SPEED_DEFAULT__ORIENT;
    settings->speeds.orbit  = NAVIGATION_SPEED_DEFAULT__ORBIT;
    settings->speeds.zoom   = NAVIGATION_SPEED_DEFAULT__ZOOM;
    settings->speeds.dolly  = NAVIGATION_SPEED_DEFAULT__DOLLY;
    settings->speeds.pan    = NAVIGATION_SPEED_DEFAULT__PAN;
}

void initNavigation(Navigation *navigation, NavigationSettings *navigation_settings) {
    navigation->settings = *navigation_settings;

    navigation->turned = false;
    navigation->moved = false;
    navigation->zoomed = false;

    navigation->move.right = false;
    navigation->move.left = false;
    navigation->move.up = false;
    navigation->move.down = false;
    navigation->move.forward = false;
    navigation->move.backward = false;

    navigation->turn.right = false;
    navigation->turn.left = false;
}

void setDefaultViewportSettings(ViewportSettings *settings) {
    settings->near_clipping_plane_distance = VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE;
    settings->far_clipping_plane_distance  = VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE;
    settings->hud_default_color = White;
    settings->hud_line_count = 0;
    settings->hud_lines = null;
    settings->show_hud = false;
    settings->use_cube_NDC = false;
    settings->flip_z = false;
    settings->antialias = false;
    settings->cull_back_faces = true;
    settings->show_wire_frame = false;
    settings->background.color = Color(Black);
    settings->background.opacity = 0;
    settings->background.depth = INFINITY;
}

void setProjectionMatrix(Viewport *viewport) {
    const f32 n = viewport->settings.near_clipping_plane_distance;
    const f32 f = viewport->settings.far_clipping_plane_distance;
    const f32 fl = viewport->camera->focal_length;
    const f32 ar = viewport->dimensions.height_over_width;
    const f32 gl = viewport->settings.use_cube_NDC;
    mat4 M;

    M.X.y = M.X.z = M.X.w = M.Y.x = M.Y.z = M.Y.w = M.W.x = M.W.y = M.W.w = M.Z.x = M.Z.y = 0;
    M.Z.w = 1.0f;
    M.X.x = fl * ar;
    M.Y.y = fl;
    M.Z.z = M.W.z = 1.0f / (f - n);
    M.Z.z *= gl ? (f + n) : f;
    M.W.z *= gl ? (-2 * f * n) : (-n * f);

    viewport->projection_matrix = M;
}

void initViewport(Viewport *viewport, ViewportSettings *viewport_settings, NavigationSettings *navigation_settings, Camera *camera, PixelQuad *pixels) {
    viewport->pixels = pixels;
    viewport->camera = camera;
    viewport->settings = *viewport_settings;
    viewport->position.x = 0;
    viewport->position.y = 0;
    initBox(&viewport->default_box);
    initHUD(&viewport->hud, viewport_settings->hud_lines, viewport_settings->hud_line_count, 1, viewport_settings->hud_default_color, 0, 0);
    initNavigation(&viewport->navigation, navigation_settings);
    setProjectionMatrix(viewport);
    updateDimensions(&viewport->dimensions, MAX_WIDTH, MAX_HEIGHT, MAX_WIDTH);
}

void setDefaultSceneSettings(SceneSettings *settings) {
    settings->cameras = 1;
    settings->primitives = 0;
    settings->materials = 0;
    settings->lights = 0;
    settings->textures = 0;
    settings->meshes = 0;
    settings->mesh_files = null;
    settings->texture_files = null;
    settings->file.char_ptr = null;
    settings->file.length = 0;
}

void initCurve(Curve *curve) {
    curve->thickness = 0.1f;
    curve->revolution_count = 1;
}

void initPrimitive(Primitive *primitive) {
    primitive->id = 0;
    primitive->type = PrimitiveType_None;
    primitive->color = White;
    primitive->flags = ALL_FLAGS;
    primitive->scale.x = 1;
    primitive->scale.y = 1;
    primitive->scale.z = 1;
    primitive->position.x = 0;
    primitive->position.y = 0;
    primitive->position.z = 0;
    primitive->rotation.axis.x = 0;
    primitive->rotation.axis.y = 0;
    primitive->rotation.axis.z = 0;
    primitive->rotation.amount = 1;
}

bool initGrid(Grid *grid, u8 u_segments, u8 v_segments) {
    if (!u_segments || u_segments > GRID__MAX_SEGMENTS ||
        !v_segments || v_segments > GRID__MAX_SEGMENTS)
        return false;

    grid->u_segments = u_segments;
    grid->v_segments = v_segments;

    f32 u_step = u_segments > 1 ? (2.0f / (u_segments - 1)) : 0;
    f32 v_step = v_segments > 1 ? (2.0f / (v_segments - 1)) : 0;

    for (u8 u = 0; u < grid->u_segments; u++) {
        grid->vertices.uv.u.from[u].y = grid->vertices.uv.u.to[u].y = 0;
        grid->vertices.uv.u.from[u].x = grid->vertices.uv.u.to[u].x = -1 + u * u_step;
        grid->vertices.uv.u.from[u].z = -1;
        grid->vertices.uv.u.to[  u].z = +1;
    }
    for (u8 v = 0; v < grid->v_segments; v++) {
        grid->vertices.uv.v.from[v].y = grid->vertices.uv.v.to[v].y = 0;
        grid->vertices.uv.v.from[v].z = grid->vertices.uv.v.to[v].z = -1 + v * v_step;
        grid->vertices.uv.v.from[v].x = -1;
        grid->vertices.uv.v.to[  v].x = +1;
    }

    setGridEdgesFromVertices(grid->edges.uv.u, grid->u_segments, grid->vertices.uv.u.from, grid->vertices.uv.u.to);
    setGridEdgesFromVertices(grid->edges.uv.v, grid->v_segments, grid->vertices.uv.v.from, grid->vertices.uv.v.to);

    return true;
}

INLINE bool isEqualVec2(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

INLINE vec2 clampVec2ToZero(vec2 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    return v;
}

INLINE vec2 clampVec2ToUpper(vec2 v, vec2 upper) {
    v.x = v.x < upper.x ? v.x : upper.x;
    v.y = v.y < upper.y ? v.y : upper.y;
    return v;
}

INLINE vec2 clampVec2(vec2 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;

    v.x = v.x < 1.0f ? v.x : 1.0f;
    v.y = v.y < 1.0f ? v.y : 1.0f;

    return v;
}

INLINE vec2 clampVec2To(vec2 v, const f32 min_value, const f32 max_value) {
    v.x = v.x > min_value ? v.x : min_value;
    v.y = v.y > min_value ? v.y : min_value;

    v.x = v.x < max_value ? v.x : max_value;
    v.y = v.y < max_value ? v.y : max_value;

    return v;
}

INLINE vec2 getVec2Of(f32 value) {
    vec2 out;

    out.x = out.y = value;

    return out;
}

INLINE vec2 invertedVec2(vec2 in) {
    vec2 out;

    out.x = -in.x;
    out.y = -in.y;

    return out;
}

INLINE vec2 oneOverVec2(vec2 v) {
    vec2 out;

    out.x = 1.0f / v.x;
    out.y = 1.0f / v.y;

    return out;
}

INLINE vec2 approachVec2(vec2 src, vec2 trg, f32 diff) {
    vec2 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);

    return out;
}

INLINE bool nonZeroVec2(vec2 v) {
    return v.x != 0 ||
           v.y != 0;
}

INLINE vec2 minVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x < b.x ? a.x : b.x;
    out.y = a.y < b.y ? a.y : b.y;

    return out;
}

INLINE vec2 maxVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x > b.x ? a.x : b.x;
    out.y = a.y > b.y ? a.y : b.y;

    return out;
}

INLINE f32 minCoordVec2(vec2 v) {
    f32 out = v.x;
    if (v.y < out) out = v.y;
    return out;
}

INLINE f32 maxCoordVec2(vec2 v) {
    f32 out = v.x;
    if (v.y > out) out = v.y;
    return out;
}

INLINE vec2 subVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;

    return out;
}

INLINE vec2 addVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;

    return out;
}

INLINE vec2 mulVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;

    return out;
}

INLINE vec2 mulAddVec2(vec2 v, vec2 factors, vec2 to_be_added) {
    vec2 out;

    out.x = fast_mul_add(v.x, factors.x, to_be_added.x);
    out.y = fast_mul_add(v.y, factors.y, to_be_added.y);

    return out;
}

INLINE vec2 scaleAddVec2(vec2 v, f32 factor, vec2 to_be_added) {
    vec2 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);

    return out;
}

INLINE vec2 scaleVec2(vec2 a, f32 factor) {
    vec2 out;

    out.x = a.x * factor;
    out.y = a.y * factor;

    return out;
}

INLINE vec2 mulVec2Mat2(vec2 in, mat2 m) {
    vec2 out;

    out.x = in.x * m.X.x + in.y * m.Y.x;
    out.y = in.x * m.X.y + in.y * m.Y.y;

    return out;
}

INLINE f32 dotVec2(vec2 a, vec2 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y)
    );
}

INLINE f32 squaredLengthVec2(vec2 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y)
    );
}

INLINE f32 lengthVec2(vec2 v) {
    return sqrtf(squaredLengthVec2(v));
}

INLINE vec2 normVec2(vec2 v) {
    return scaleVec2(v, 1.0f / lengthVec2(v));
}

INLINE f32 DotVec2(vec2 a, vec2 b) { return clampValue(dotVec2(a, b)); }

INLINE mat2 outerVec2(vec2 a, vec2 b) {
    mat2 out;

    out.X = scaleVec2(a, b.x);
    out.Y = scaleVec2(a, b.y);

    return out;
}

INLINE vec2 reflectVec2(vec2 V, vec2 N) {
    vec2 out = scaleVec2(N, -2 * dotVec2(N, V));
    out = addVec2(out, V);
    return out;
}



INLINE vec3 clampVec3ToZero(vec3 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    v.z = v.z > 0.0f ? v.z : 0.0f;
    return v;
}

INLINE vec3 clampVec3ToUpper(vec3 v, vec3 upper) {
    v.x = v.x < upper.x ? v.x : upper.x;
    v.y = v.y < upper.y ? v.y : upper.y;
    v.z = v.z < upper.z ? v.z : upper.z;
    return v;
}

INLINE vec3 clampVec3(vec3 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    v.z = v.z > 0.0f ? v.z : 0.0f;

    v.x = v.x < 1.0f ? v.x : 1.0f;
    v.y = v.y < 1.0f ? v.y : 1.0f;
    v.z = v.z < 1.0f ? v.z : 1.0f;

    return v;
}

INLINE vec3 clampVec3To(vec3 v, const f32 min_value, const f32 max_value) {
    v.x = v.x > min_value ? v.x : min_value;
    v.y = v.y > min_value ? v.y : min_value;
    v.z = v.z > min_value ? v.z : min_value;

    v.x = v.x < max_value ? v.x : max_value;
    v.y = v.y < max_value ? v.y : max_value;
    v.z = v.z < max_value ? v.z : max_value;

    return v;
}

INLINE bool isEqualVec3(vec3 a, vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

INLINE vec3 getVec3Of(f32 value) {
    vec3 out;

    out.x = out.y = out.z = value;

    return out;
}

INLINE vec3 invertedVec3(vec3 in) {
    vec3 out;

    out.x = -in.x;
    out.y = -in.y;
    out.z = -in.z;

    return out;
}

INLINE vec3 oneOverVec3(vec3 v) {
    vec3 out;

    out.x = 1.0f / v.x;
    out.y = 1.0f / v.y;
    out.z = 1.0f / v.z;

    return out;
}

INLINE vec3 approachVec3(vec3 src, vec3 trg, f32 diff) {
    vec3 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);
    out.z = approach(src.z, trg.z, diff);

    return out;
}

INLINE bool nonZeroVec3(vec3 v) {
    return v.x != 0 ||
           v.y != 0 ||
           v.z != 0;
}

INLINE vec3 minVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x < b.x ? a.x : b.x;
    out.y = a.y < b.y ? a.y : b.y;
    out.z = a.z < b.z ? a.z : b.z;

    return out;
}

INLINE vec3 maxVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x > b.x ? a.x : b.x;
    out.y = a.y > b.y ? a.y : b.y;
    out.z = a.z > b.z ? a.z : b.z;

    return out;
}

INLINE f32 minCoordVec3(vec3 v) {
    f32 out = v.x;
    if (v.y < out) out = v.y;
    if (v.z < out) out = v.z;
    return out;
}

INLINE f32 maxCoordVec3(vec3 v) {
    f32 out = v.x;
    if (v.y > out) out = v.y;
    if (v.z > out) out = v.z;
    return out;
}

INLINE vec3 setPointOnUnitSphere(f32 s, f32 t) {
    vec3 out;

    f32 t_squared = t * t;
    f32 s_squared = s * s;
    f32 factor = 1 / ( t_squared + s_squared + 1);

    out.x = 2*s * factor;
    out.y = 2*t * factor;
    out.z = (t_squared + s_squared - 1) * t_squared;

    return out;
}

INLINE vec3 subVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;

    return out;
}

INLINE vec3 addVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;

    return out;
}

INLINE vec3 mulVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;

    return out;
}

INLINE vec3 mulAddVec3(vec3 v, vec3 factors, vec3 to_be_added) {
    vec3 out;

    out.x = fast_mul_add(v.x, factors.x, to_be_added.x);
    out.y = fast_mul_add(v.y, factors.y, to_be_added.y);
    out.z = fast_mul_add(v.z, factors.z, to_be_added.z);

    return out;
}

INLINE vec3 scaleAddVec3(vec3 v, f32 factor, vec3 to_be_added) {
    vec3 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);
    out.z = fast_mul_add(v.z, factor, to_be_added.z);

    return out;
}

INLINE vec3 scaleVec3(vec3 a, f32 factor) {
    vec3 out;

    out.x = a.x * factor;
    out.y = a.y * factor;
    out.z = a.z * factor;

    return out;
}

INLINE vec3 mulVec3Mat3(vec3 in, mat3 m) {
    vec3 out;

    out.x = in.x * m.X.x + in.y * m.Y.x + in.z * m.Z.x;
    out.y = in.x * m.X.y + in.y * m.Y.y + in.z * m.Z.y;
    out.z = in.x * m.X.z + in.y * m.Y.z + in.z * m.Z.z;

    return out;
}

INLINE f32 dotVec3(vec3 a, vec3 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y) +
            (a.z * b.z)
    );
}

INLINE vec3 crossVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = (a.y * b.z) - (a.z * b.y);
    out.y = (a.z * b.x) - (a.x * b.z);
    out.z = (a.x * b.y) - (a.y * b.x);

    return out;
}

INLINE f32 squaredLengthVec3(vec3 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y) +
            (v.z * v.z)
    );
}

INLINE f32 lengthVec3(vec3 v) {
    return sqrtf(squaredLengthVec3(v));
}

INLINE vec3 normVec3(vec3 v) {
    return scaleVec3(v, 1.0f / lengthVec3(v));
}

INLINE f32 DotVec3(vec3 a, vec3 b) { return clampValue(dotVec3(a, b)); }

INLINE mat3 outerVec3(vec3 a, vec3 b) {
    mat3 out;

    out.X = scaleVec3(a, b.x);
    out.Y = scaleVec3(a, b.y);
    out.Z = scaleVec3(a, b.z);

    return out;
}

INLINE vec3 reflectVec3(vec3 V, vec3 N) {
    vec3 out = scaleVec3(N, -2 * dotVec3(N, V));
    out = addVec3(out, V);
    return out;
}

INLINE vec3 lerpVec3(vec3 from, vec3 to, f32 by) {
    return scaleAddVec3(subVec3(to, from), by, from);
}



INLINE vec4 getVec4Of(f32 value) {
    vec4 out;

    out.x = out.y = out.z = out.w = value;

    return out;
}

INLINE vec4 invertedVec4(vec4 in) {
    vec4 out;

    out.x = -in.x;
    out.y = -in.y;
    out.z = -in.z;
    out.w = -in.w;

    return out;
}

INLINE vec4 approachVec4(vec4 src, vec4 trg, f32 diff) {
    vec4 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);
    out.z = approach(src.z, trg.z, diff);
    out.w = approach(src.w, trg.w, diff);

    return out;
}

INLINE bool nonZeroVec4(vec4 v) {
    return v.x != 0 ||
           v.y != 0 ||
           v.z != 0 ||
           v.w != 0;
}

INLINE vec4 subVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    out.w = a.w - b.w;

    return out;
}

INLINE vec4 addVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    out.w = a.w + b.w;

    return out;
}

INLINE vec4 mulVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;
    out.w = a.w * b.w;

    return out;
}

INLINE vec4 scaleVec4(vec4 a, f32 factor) {
    vec4 out;

    out.x = a.x * factor;
    out.y = a.y * factor;
    out.z = a.z * factor;
    out.w = a.w * factor;

    return out;
}
INLINE vec4 scaleAddVec4(vec4 v, f32 factor, vec4 to_be_added) {
    vec4 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);
    out.z = fast_mul_add(v.z, factor, to_be_added.z);
    out.w = fast_mul_add(v.w, factor, to_be_added.w);

    return out;
}

INLINE vec4 mulVec4Mat4(vec4 in, mat4 m) {
    vec4 out;

    out.x = in.x * m.X.x + in.y * m.Y.x + in.z * m.Z.x + in.w * m.W.x;
    out.y = in.x * m.X.y + in.y * m.Y.y + in.z * m.Z.y + in.w * m.W.y;
    out.z = in.x * m.X.z + in.y * m.Y.z + in.z * m.Z.z + in.w * m.W.z;
    out.w = in.x * m.X.w + in.y * m.Y.w + in.z * m.Z.w + in.w * m.W.w;

    return out;
}

INLINE f32 dotVec4(vec4 a, vec4 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y) +
            (a.z * b.z) +
            (a.w * b.w)
    );
}

INLINE f32 squaredLengthVec4(vec4 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y) +
            (v.z * v.z) +
            (v.w * v.w)
    );
}

INLINE f32 lengthVec4(vec4 v) {
    return sqrtf(squaredLengthVec4(v));
}

INLINE vec4 norm4(vec4 v) {
    return scaleVec4(v, 1.0f / lengthVec4(v));
}

INLINE f32 mulVec3Mat4(vec3 in, f32 w, mat4 M, vec3 *out) {
    vec4 v4 = mulVec4Mat4(Vec4fromVec3(in, w), M);
    *out = Vec3fromVec4(v4);
    return v4.w;
}

INLINE vec4 lerpVec4(vec4 from, vec4 to, f32 by) {
    return scaleAddVec4(subVec4(to, from), by, from);
}



INLINE quat getIdentityQuaternion() {
    quat out;

    out.axis = getVec3Of(0);
    out.amount = 1;

    return out;
}

INLINE quat normQuat(quat q) {
    quat out;

    f32 factor = 1.0f / sqrtf(q.axis.x * q.axis.x + q.axis.y * q.axis.y + q.axis.z * q.axis.z + q.amount * q.amount);
    out.axis = scaleVec3(q.axis, factor);
    out.amount = q.amount * factor;

    return out;
}

INLINE vec3 mulVec3Quat(const vec3 v, quat q) {
    vec3 out = crossVec3(q.axis, v);
    vec3 qqv = crossVec3(q.axis, out);
    out = scaleAddVec3(out, q.amount, qqv);
    out = scaleAddVec3(out, 2, v);
    return out;
}

INLINE quat mulQuat(quat a, quat b) {
    quat out;

    out.amount = a.amount * b.amount - a.axis.x * b.axis.x - a.axis.y * b.axis.y - a.axis.z * b.axis.z;
    out.axis.x = a.amount * b.axis.x + a.axis.x * b.amount + a.axis.y * b.axis.z - a.axis.z * b.axis.y;
    out.axis.y = a.amount * b.axis.y - a.axis.x * b.axis.z + a.axis.y * b.amount + a.axis.z * b.axis.x;
    out.axis.z = a.amount * b.axis.z + a.axis.x * b.axis.y - a.axis.y * b.axis.x + a.axis.z * b.amount;

    return out;
}

INLINE quat conjugate(quat q) {
    quat out;

    out.amount = q.amount;
    out.axis = invertedVec3(q.axis);

    return out;
}

INLINE quat convertRotationMatrixToQuaternion(mat3 rotation_matrix) {
    quat out;

    f32 fourXSquaredMinus1 = rotation_matrix.X.x - rotation_matrix.Y.y - rotation_matrix.Z.z;
    f32 fourYSquaredMinus1 = rotation_matrix.Y.y - rotation_matrix.X.x - rotation_matrix.Z.z;
    f32 fourZSquaredMinus1 = rotation_matrix.Z.z - rotation_matrix.X.x - rotation_matrix.Y.y;
    f32 fourWSquaredMinus1 = rotation_matrix.X.x + rotation_matrix.Y.y + rotation_matrix.Z.z;

    int biggestIndex = 0;
    f32 fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }

    f32 biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    f32 mult = 0.25f / biggestVal;

    switch(biggestIndex) {
        case 0:
            out.amount = biggestVal;
            out.axis.x = (rotation_matrix.Y.z - rotation_matrix.Z.y) * mult;
            out.axis.y = (rotation_matrix.Z.x - rotation_matrix.X.z) * mult;
            out.axis.z = (rotation_matrix.X.y - rotation_matrix.Y.x) * mult;
            break;
        case 1:
            out.amount = (rotation_matrix.Y.z - rotation_matrix.Z.y) * mult;
            out.axis.x = biggestVal;
            out.axis.y = (rotation_matrix.X.y + rotation_matrix.Y.x) * mult;
            out.axis.z = (rotation_matrix.Z.x + rotation_matrix.X.z) * mult;
            break;
        case 2:
            out.amount = (rotation_matrix.Z.x - rotation_matrix.X.z) * mult;
            out.axis.x = (rotation_matrix.X.y + rotation_matrix.Y.x) * mult;
            out.axis.y = biggestVal;
            out.axis.z = (rotation_matrix.Y.z + rotation_matrix.Z.y) * mult;
            break;
        case 3:
            out.amount = (rotation_matrix.X.y - rotation_matrix.Y.x) * mult;
            out.axis.x = (rotation_matrix.Z.x + rotation_matrix.X.z) * mult;
            out.axis.y = (rotation_matrix.Y.z + rotation_matrix.Z.y) * mult;
            out.axis.z = biggestVal;
            break;
    }

    return out;
}

INLINE mat3 convertQuaternionToRotationMatrix(quat q) {
    mat3 out;

    f32 q0 = q.amount;
    f32 q1 = q.axis.x;
    f32 q2 = q.axis.y;
    f32 q3 = q.axis.z;

    out.X.x = 2 * (q0 * q0 + q1 * q1) - 1;
    out.X.y = 2 * (q1 * q2 - q0 * q3);
    out.X.z = 2 * (q1 * q3 + q0 * q2);

    out.Y.x = 2 * (q1 * q2 + q0 * q3);
    out.Y.y = 2 * (q0 * q0 + q2 * q2) - 1;
    out.Y.z = 2 * (q2 * q3 - q0 * q1);

    out.Z.x = 2 * (q1 * q3 - q0 * q2);
    out.Z.y = 2 * (q2 * q3 + q0 * q1);
    out.Z.z = 2 * (q0 * q0 + q3 * q3) - 1;

    return out;
}

INLINE quat getRotationAroundAxis(vec3 axis, f32 amount) {
    vec2 sin_cos = getPointOnUnitCircle(amount);
    quat out;
    out.axis = scaleVec3(axis, sin_cos.y);
    out.amount = sin_cos.x;

    return normQuat(out);
}

INLINE quat getRotationAroundAxisBySinCon(vec3 axis, vec2 sin_cos) {
    quat out;
    out.axis = scaleVec3(axis, sin_cos.y);
    out.amount = sin_cos.x;

    return normQuat(out);
}

INLINE quat rotateAroundAxisBySinCos(quat q, vec3 axis, vec2 sin_cos) {
    quat rotation = getRotationAroundAxisBySinCon(axis, sin_cos);
    return mulQuat(q, rotation);
}

INLINE quat rotateAroundAxis(quat q, vec3 axis, f32 amount) {
    quat rotation = getRotationAroundAxis(axis, amount);
    return mulQuat(q, rotation);
}



INLINE mat2 getMat2Identity() {
    mat2 out;

    out.X.x = 1; out.X.y = 0;
    out.Y.x = 0; out.Y.y = 1;

    return out;
}

INLINE mat2 addMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;

    return out;
}

INLINE mat2 subMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;

    return out;
}

INLINE mat2 scaleMat2(mat2 m, f32 factor) {
    mat2 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;

    return out;
}

INLINE mat2 transposedMat2(mat2 m) {
    mat2 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;

    return out;
}

INLINE mat2 mulMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y; // Row 1 | Column 2

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y; // Row 2 | Column 2

    return out;
}

INLINE mat2 invMat2(mat2 m) {
    mat2 out;

    f32 a = m.X.x,  b = m.X.y,
            c = m.Y.x,  d = m.Y.y;

    f32 det = a*d - b*c;
    f32 one_over_det = 1.0f / det;

    out.X.x = +d * one_over_det;
    out.X.y = -b * one_over_det;
    out.Y.x = -c * one_over_det;
    out.Y.y = +a * one_over_det;

    return out;
}

INLINE bool safeInvertMat2(mat2 *m) {
    f32 a = m->X.x,  b = m->X.y,
            c = m->Y.x,  d = m->Y.y;

    f32 det = a*d - b*c;
    if (!det) return false;
    f32 one_over_det = 1.0f / det;

    m->X.x = +d * one_over_det;
    m->X.y = -b * one_over_det;
    m->Y.x = -c * one_over_det;
    m->Y.y = +a * one_over_det;

    return true;
}

INLINE void rotateMat2(f32 amount, mat2* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec2 X = out->X;
    vec2 Y = out->Y;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
}

INLINE void setRotationMat2(f32 roll, mat2* roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}



INLINE mat3 getMat3Identity() {
    mat3 out;

    out.X.x = 1; out.X.y = 0; out.X.z = 0;
    out.Y.x = 0; out.Y.y = 1; out.Y.z = 0;
    out.Z.x = 0; out.Z.y = 0; out.Z.z = 1;

    return out;
}

INLINE mat3 addMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;
    out.X.z = a.X.z + b.X.z;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;
    out.Y.z = a.Y.z + b.Y.z;

    out.Z.x = a.Z.x + b.Z.x;
    out.Z.y = a.Z.y + b.Z.y;
    out.Z.z = a.Z.x + b.Z.x;

    return out;
}

INLINE mat3 subMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;
    out.X.z = a.X.z - b.X.z;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;
    out.Y.z = a.Y.z - b.Y.z;

    out.Z.x = a.Z.x - b.Z.x;
    out.Z.y = a.Z.y - b.Z.y;
    out.Z.z = a.Z.x - b.Z.x;

    return out;
}

INLINE mat3 scaleMat3(mat3 m, f32 factor) {
    mat3 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;
    out.X.z = m.X.z * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;
    out.Y.z = m.Y.z * factor;

    out.Z.x = m.Z.x * factor;
    out.Z.y = m.Z.y * factor;
    out.Z.z = m.Z.z * factor;

    return out;
}

INLINE mat3 transposedMat3(mat3 m) {
    mat3 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;  out.X.z = m.Z.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;  out.Y.z = m.Z.y;
    out.Z.x = m.X.z;  out.Z.y = m.Y.z;  out.Z.z = m.Z.z;

    return out;
}

INLINE mat3 mulMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x + a.X.z*b.Z.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y + a.X.z*b.Z.y; // Row 1 | Column 2
    out.X.z = a.X.x*b.X.z + a.X.y*b.Y.z + a.X.z*b.Z.z; // Row 1 | Column 3

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x + a.Y.z*b.Z.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y + a.Y.z*b.Z.y; // Row 2 | Column 2
    out.Y.z = a.Y.x*b.X.z + a.Y.y*b.Y.z + a.Y.z*b.Z.z; // Row 2 | Column 3

    out.Z.x = a.Z.x*b.X.x + a.Z.y*b.Y.x + a.Z.z*b.Z.x; // Row 3 | Column 1
    out.Z.y = a.Z.x*b.X.y + a.Z.y*b.Y.y + a.Z.z*b.Z.y; // Row 3 | Column 2
    out.Z.z = a.Z.x*b.X.z + a.Z.y*b.Y.z + a.Z.z*b.Z.z; // Row 3 | Column 3

    return out;
}

INLINE mat3 invMat3(mat3 m) {
    mat3 out;

    f32 one_over_determinant = 1.0f / (
            + m.X.x * (m.Y.y * m.Z.z - m.Z.y * m.Y.z)
            - m.Y.x * (m.X.y * m.Z.z - m.Z.y * m.X.z)
            + m.Z.x * (m.X.y * m.Y.z - m.Y.y * m.X.z)
    );

    out.X.x = + (m.Y.y * m.Z.z - m.Z.y * m.Y.z) * one_over_determinant;
    out.Y.x = - (m.Y.x * m.Z.z - m.Z.x * m.Y.z) * one_over_determinant;
    out.Z.x = + (m.Y.x * m.Z.y - m.Z.x * m.Y.y) * one_over_determinant;
    out.X.y = - (m.X.y * m.Z.z - m.Z.y * m.X.z) * one_over_determinant;
    out.Y.y = + (m.X.x * m.Z.z - m.Z.x * m.X.z) * one_over_determinant;
    out.Z.y = - (m.X.x * m.Z.y - m.Z.x * m.X.y) * one_over_determinant;
    out.X.z = + (m.X.y * m.Y.z - m.Y.y * m.X.z) * one_over_determinant;
    out.Y.z = - (m.X.x * m.Y.z - m.Y.x * m.X.z) * one_over_determinant;
    out.Z.z = + (m.X.x * m.Y.y - m.Y.x * m.X.y) * one_over_determinant;

    return out;
}

INLINE bool safeInvertMat3(mat3 *m) {
    f32 m11 = m->X.x,  m12 = m->X.y,  m13 = m->X.z,
            m21 = m->Y.x,  m22 = m->Y.y,  m23 = m->Y.z,
            m31 = m->Z.x,  m32 = m->Z.y,  m33 = m->Z.z,

            c11 = m22*m33 -
                  m23*m32,

            c12 = m13*m32 -
                  m12*m33,

            c13 = m12*m23 -
                  m13*m22,


            c21 = m23*m31 -
                  m21*m33,

            c22 = m11*m33 -
                  m13*m31,

            c23 = m13*m21 -
                  m11*m23,


            c31 = m21*m32 -
                  m22*m31,

            c32 = m12*m31 -
                  m11*m32,

            c33 = m11*m22 -
                  m12*m21,

            d = c11 + c12 + c13 +
                c21 + c22 + c23 +
                c31 + c32 + c33;

    if (!d) return false;

    d = 1 / d;

    m->X.x = d * c11;  m->X.y = d * c12;  m->X.z = d * c13;
    m->Y.x = d * c21;  m->Y.y = d * c22;  m->Y.z = d * c23;
    m->Z.x = d * c31;  m->Z.y = d * c32;  m->Z.z = d * c33;

    return true;
}

INLINE void yawMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.x = xy.x * X.x - xy.y * X.z;
    out->Y.x = xy.x * Y.x - xy.y * Y.z;
    out->Z.x = xy.x * Z.x - xy.y * Z.z;

    out->X.z = xy.x * X.z + xy.y * X.x;
    out->Y.z = xy.x * Y.z + xy.y * Y.x;
    out->Z.z = xy.x * Z.z + xy.y * Z.x;
}

INLINE void pitchMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.y = xy.x * X.y + xy.y * X.z;
    out->Y.y = xy.x * Y.y + xy.y * Y.z;
    out->Z.y = xy.x * Z.y + xy.y * Z.z;

    out->X.z = xy.x * X.z - xy.y * X.y;
    out->Y.z = xy.x * Y.z - xy.y * Y.y;
    out->Z.z = xy.x * Z.z - xy.y * Z.y;
}

INLINE void rollMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;
    out->Z.x = xy.x * Z.x + xy.y * Z.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
    out->Z.y = xy.x * Z.y - xy.y * Z.x;
}

INLINE void setYawMat3(f32 yaw, mat3* yaw_matrix) {
    vec2 xy = getPointOnUnitCircle(yaw);

    yaw_matrix->X.x = yaw_matrix->Z.z = xy.x;
    yaw_matrix->X.z = +xy.y;
    yaw_matrix->Z.x = -xy.y;
}

INLINE void setPitchMat3(f32 pitch, mat3* pitch_matrix) {
    vec2 xy = getPointOnUnitCircle(pitch);

    pitch_matrix->Z.z = pitch_matrix->Y.y = xy.x;
    pitch_matrix->Y.z = -xy.y;
    pitch_matrix->Z.y = +xy.y;
}

INLINE void setRollMat3(f32 roll, mat3* roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}



INLINE mat4 getMat4Identity() {
    mat4 out;

    out.X.x = 1; out.X.y = 0; out.X.z = 0; out.X.w = 0;
    out.Y.x = 0; out.Y.y = 1; out.Y.z = 0; out.Y.w = 0;
    out.Z.x = 0; out.Z.y = 0; out.Z.z = 1; out.Z.w = 0;
    out.W.x = 0; out.W.y = 0; out.W.z = 0; out.W.w = 1;

    return out;
}

INLINE mat4 transposeMat4(mat4 m) {
    mat4 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;  out.X.z = m.Z.x;  out.X.w = m.W.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;  out.Y.z = m.Z.y;  out.Y.w = m.W.y;
    out.Z.x = m.X.z;  out.Z.y = m.Y.z;  out.Z.z = m.Z.z;  out.Z.w = m.W.z;
    out.W.x = m.X.w;  out.W.y = m.Y.w;  out.W.z = m.Z.w;  out.W.w = m.W.w;

    return out;
}

INLINE mat4 addMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;
    out.X.z = a.X.z + b.X.z;
    out.X.w = a.X.w + b.X.w;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;
    out.Y.z = a.Y.z + b.Y.z;
    out.Y.w = a.Y.w + b.Y.w;

    out.Z.x = a.Z.x + b.Z.x;
    out.Z.y = a.Z.y + b.Z.y;
    out.Z.z = a.Z.x + b.Z.x;
    out.Z.w = a.Z.w + b.Z.w;

    out.W.x = a.W.x + b.W.x;
    out.W.y = a.W.y + b.W.y;
    out.W.z = a.W.x + b.W.x;
    out.W.w = a.W.w + b.W.w;

    return out;
}

INLINE mat4 subMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;
    out.X.z = a.X.z - b.X.z;
    out.X.w = a.X.w - b.X.w;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;
    out.Y.z = a.Y.z - b.Y.z;
    out.Y.w = a.Y.w - b.Y.w;

    out.Z.x = a.Z.x - b.Z.x;
    out.Z.y = a.Z.y - b.Z.y;
    out.Z.z = a.Z.x - b.Z.x;
    out.Z.w = a.Z.w - b.Z.w;

    out.W.x = a.W.x - b.W.x;
    out.W.y = a.W.y - b.W.y;
    out.W.z = a.W.x - b.W.x;
    out.W.w = a.W.w - b.W.w;

    return out;
}

INLINE mat4 scaleMat4(mat4 m, f32 factor) {
    mat4 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;
    out.X.z = m.X.z * factor;
    out.X.w = m.X.w * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;
    out.Y.z = m.Y.z * factor;
    out.Y.w = m.Y.w * factor;

    out.Z.x = m.Z.x * factor;
    out.Z.y = m.Z.y * factor;
    out.Z.z = m.Z.z * factor;
    out.Z.w = m.Z.w * factor;

    out.W.x = m.W.x * factor;
    out.W.y = m.W.y * factor;
    out.W.z = m.W.z * factor;
    out.W.w = m.W.w * factor;

    return out;
}

INLINE mat4 mulMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x + a.X.z*b.Z.x + a.X.w*b.W.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y + a.X.z*b.Z.y + a.X.w*b.W.y; // Row 1 | Column 2
    out.X.z = a.X.x*b.X.z + a.X.y*b.Y.z + a.X.z*b.Z.z + a.X.w*b.W.z; // Row 1 | Column 3
    out.X.w = a.X.x*b.X.w + a.X.y*b.Y.w + a.X.z*b.Z.w + a.X.w*b.W.w; // Row 1 | Column 4

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x + a.Y.z*b.Z.x + a.Y.w*b.W.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y + a.Y.z*b.Z.y + a.Y.w*b.W.y; // Row 2 | Column 2
    out.Y.z = a.Y.x*b.X.z + a.Y.y*b.Y.z + a.Y.z*b.Z.z + a.Y.w*b.W.z; // Row 2 | Column 3
    out.Y.w = a.Y.x*b.X.w + a.Y.y*b.Y.w + a.Y.z*b.Z.w + a.Y.w*b.W.w; // Row 2 | Column 4

    out.Z.x = a.Z.x*b.X.x + a.Z.y*b.Y.x + a.Z.z*b.Z.x + a.Z.w*b.W.x; // Row 3 | Column 1
    out.Z.y = a.Z.x*b.X.y + a.Z.y*b.Y.y + a.Z.z*b.Z.y + a.Z.w*b.W.y; // Row 3 | Column 2
    out.Z.z = a.Z.x*b.X.z + a.Z.y*b.Y.z + a.Z.z*b.Z.z + a.Z.w*b.W.z; // Row 3 | Column 3
    out.Z.w = a.Z.x*b.X.w + a.Z.y*b.Y.w + a.Z.z*b.Z.w + a.Z.w*b.W.w; // Row 3 | Column 4

    out.W.x = a.W.x*b.X.x + a.W.y*b.Y.x + a.W.z*b.Z.x + a.W.w*b.W.x; // Row 4 | Column 1
    out.W.y = a.W.x*b.X.y + a.W.y*b.Y.y + a.W.z*b.Z.y + a.W.w*b.W.y; // Row 4 | Column 2
    out.W.z = a.W.x*b.X.z + a.W.y*b.Y.z + a.W.z*b.Z.z + a.W.w*b.W.z; // Row 4 | Column 3
    out.W.w = a.W.x*b.X.w + a.W.y*b.Y.w + a.W.z*b.Z.w + a.W.w*b.W.w; // Row 4 | Column 4

    return out;
}

INLINE mat4 invMat4(mat4 m) {
    mat4 out;

    f32 m11 = m.X.x,  m12 = m.X.y,  m13 = m.X.z, m14 = m.X.w,
            m21 = m.Y.x,  m22 = m.Y.y,  m23 = m.Y.z, m24 = m.Y.w,
            m31 = m.Z.x,  m32 = m.Z.y,  m33 = m.Z.z, m34 = m.Z.w,
            m41 = m.W.x,  m42 = m.W.y,  m43 = m.W.z, m44 = m.W.w;

    out.X.x = +m22*m33*m44 - m22*m34*m43 - m32*m23*m44 + m32*m24*m43 + m42*m23*m34 - m42*m24*m33;
    out.X.y = -m12*m33*m44 + m12*m34*m43 + m32*m13*m44 - m32*m14*m43 - m42*m13*m34 + m42*m14*m33;
    out.X.z = +m12*m23*m44 - m12*m24*m43 - m22*m13*m44 + m22*m14*m43 + m42*m13*m24 - m42*m14*m23;
    out.X.w = -m12*m23*m34 + m12*m24*m33 + m22*m13*m34 - m22*m14*m33 - m32*m13*m24 + m32*m14*m23;

    out.Y.x = -m21*m33*m44 + m21*m34*m43 + m31*m23*m44 - m31*m24*m43 - m41*m23*m34 + m41*m24*m33;
    out.Y.y = +m11*m33*m44 - m11*m34*m43 - m31*m13*m44 + m31*m14*m43 + m41*m13*m34 - m41*m14*m33;
    out.Y.z = -m11*m23*m44 + m11*m24*m43 + m21*m13*m44 - m21*m14*m43 - m41*m13*m24 + m41*m14*m23;
    out.Y.w = +m11*m23*m34 - m11*m24*m33 - m21*m13*m34 + m21*m14*m33 + m31*m13*m24 - m31*m14*m23;

    out.Z.x = +m21*m32*m44 - m21*m34*m42 - m31*m22*m44 + m31*m24*m42 + m41*m22*m34 - m41*m24*m32;
    out.Z.y = -m11*m32*m44 + m11*m34*m42 + m31*m12*m44 - m31*m14*m42 - m41*m12*m34 + m41*m14*m32;
    out.Z.z = +m11*m22*m44 - m11*m24*m42 - m21*m12*m44 + m21*m14*m42 + m41*m12*m24 - m41*m14*m22;
    out.Z.w = -m11*m22*m34 + m11*m24*m32 + m21*m12*m34 - m21*m14*m32 - m31*m12*m24 + m31*m14*m22;

    out.W.x = -m21*m32*m43 + m21*m33*m42 + m31*m22*m43 - m31*m23*m42 - m41*m22*m33 + m41*m23*m32;
    out.W.y = +m11*m32*m43 - m11*m33*m42 - m31*m12*m43 + m31*m13*m42 + m41*m12*m33 - m41*m13*m32;
    out.W.z = -m11*m22*m43 + m11*m23*m42 + m21*m12*m43 - m21*m13*m42 - m41*m12*m23 + m41*m13*m22;
    out.W.w = +m11*m22*m33 - m11*m23*m32 - m21*m12*m33 + m21*m13*m32 + m31*m12*m23 - m31*m13*m22;

    f32 det = m11*out.X.x + m12*out.Y.x + m13*out.Z.x + m14*out.W.x;
    if (!det) return m;

    out = scaleMat4(out, 1.0f / det);

    return out;
}

INLINE void yawMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.x = xy.x * X.x - xy.y * X.z;
    out->Y.x = xy.x * Y.x - xy.y * Y.z;
    out->Z.x = xy.x * Z.x - xy.y * Z.z;

    out->X.z = xy.x * X.z + xy.y * X.x;
    out->Y.z = xy.x * Y.z + xy.y * Y.x;
    out->Z.z = xy.x * Z.z + xy.y * Z.x;
}

INLINE void pitchMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.y = xy.x * X.y + xy.y * X.z;
    out->Y.y = xy.x * Y.y + xy.y * Y.z;
    out->Z.y = xy.x * Z.y + xy.y * Z.z;

    out->X.z = xy.x * X.z - xy.y * X.y;
    out->Y.z = xy.x * Y.z - xy.y * Y.y;
    out->Z.z = xy.x * Z.z - xy.y * Z.y;
}

INLINE void rollMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;
    out->Z.x = xy.x * Z.x + xy.y * Z.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
    out->Z.y = xy.x * Z.y - xy.y * Z.x;
}

INLINE void setYawMat4(f32 yaw, mat4 *yaw_matrix) {
    vec2 xy = getPointOnUnitCircle(yaw);

    yaw_matrix->X.x = yaw_matrix->Z.z = xy.x;
    yaw_matrix->X.z = +xy.y;
    yaw_matrix->Z.x = -xy.y;
}

INLINE void setPitchMat4(f32 pitch, mat4 *pitch_matrix) {
    vec2 xy = getPointOnUnitCircle(pitch);

    pitch_matrix->Z.z = pitch_matrix->Y.y = xy.x;
    pitch_matrix->Y.z = -xy.y;
    pitch_matrix->Z.y = +xy.y;
}

INLINE void setRollMat4(f32 roll, mat4 *roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}

INLINE mat4 mat4fromMat3(mat3 m3) {
    mat4 out = getMat4Identity();
    for (u8 row = 0; row < 3; row++)
        for (u8 col = 0; col < 3; col++)
            out.axis[row].components[col] = m3.axis[row].components[col];

    return out;
}


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

INLINE f32 toneMappedBaked(f32 LinearColor) {
    f32 x = LinearColor - 0.004f;
    if (x < 0.0f) x = 0.0f;
    f32 x2_times_sholder_strength = x * x * 6.2f;
    return (x2_times_sholder_strength + x*0.5f)/(x2_times_sholder_strength + x*1.7f + 0.06f);
}

INLINE vec3 reflectWithDot(vec3 V, vec3 N, f32 NdotV) {
    return scaleAddVec3(N, -2 * NdotV, V);
}

INLINE vec3 shadePointOnSurface(Shaded *shaded, f32 NdotL) {
    if (shaded->has.specular) {
        vec3 half_vector, color;
        if (shaded->uses.blinn) {
            half_vector = normVec3(subVec3(shaded->light_direction, shaded->viewing_direction));
            color = scaleVec3(shaded->material->specular, powf(DotVec3(shaded->normal, half_vector), 16.0f * shaded->material->shininess));
        } else
            color = scaleVec3(shaded->material->specular, powf(DotVec3(shaded->reflected_direction, shaded->light_direction), 4.0f * shaded->material->shininess));

        if (shaded->has.diffuse)
            return scaleAddVec3(shaded->diffuse, clampValue(NdotL), color);
        else
            return color;
    } else
        return scaleVec3(shaded->diffuse, clampValue(NdotL));
}


u8 shadeMesh(Mesh *mesh, Rasterizer *rasterizer) {

    // Transform the mesh's vertex positions into clip space and world space:
    vec4 world_space;
    for (u32 i = 0; i < mesh->vertex_count; i++) {
        world_space = Vec4fromVec3(mesh->vertex_positions[i], 1.0f);
        world_space = mulVec4Mat4(world_space, rasterizer->model_to_world);
        rasterizer->clip_space_vertex_positions[i] = mulVec4Mat4(world_space, rasterizer->world_to_clip);
        rasterizer->world_space_vertex_positions[i] = world_space.v3;
    }

    // Transform the mesh's vertex normals into world space:
    for (u32 i = 0; i < mesh->normals_count; i++) {
        world_space = Vec4fromVec3(mesh->vertex_normals[i], 0.0f);
        world_space = mulVec4Mat4(world_space, rasterizer->model_to_world_inverted_transposed);
        rasterizer->world_space_vertex_normals[i] = world_space.v3;
    }

    return INSIDE;
}


INLINE vec4 sampleTextureMip(TextureMip *mip, vec2 UV, bool filter) {
    f32 u = UV.u;
    f32 v = UV.v;
    if (u > 1) u -= (f32)((u32)u);
    if (v > 1) v -= (f32)((u32)v);

    const f32 offset = filter ? 0.5f : -0.5f;

    const f32 U = u * (f32)mip->width  + offset;
    const f32 V = v * (f32)mip->height + offset;
    const u32 x = (u32)U;
    const u32 y = (u32)V;

    if (!filter) return mip->texels[mip->width * y + x];

    const f32 r = U - (f32)x;
    const f32 b = V - (f32)y;
    const f32 l = 1 - r;
    const f32 t = 1 - b;
    const f32 factors[4] = {
            t * l, t * r,
            b * l, b * r
    };
    const TexelQuad texel_quad = mip->texel_quad_lines[y][x];
    vec4 texel = getVec4Of(0);
    for (u8 i = 0; i < 4; i++) texel = scaleAddVec4(texel_quad.quadrants[i], factors[i], texel);

    return texel;
}

INLINE vec4 sampleTexture(Texture *texture, vec2 UV, vec2 dUV) {
    u8 mip_level = 0;
    if (texture->mipmap) {
        const f32 pixel_width  = dUV.u * (f32)texture->width;
        const f32 pixel_height = dUV.v * (f32)texture->height;
        f32 pixel_size = (f32)(pixel_width + pixel_height) * 0.5f;
        const u8 last_mip = texture->mip_count - 1;

        while (pixel_size > 1 && mip_level < last_mip) {
            pixel_size /= 2;
            mip_level += 1;
        }
    }

    return sampleTextureMip(texture->mips + mip_level, UV, texture->filter);
}

INLINE u8 getCheckerBoardPixelValueByUV(vec2 UV, f32 half_step_count) {
    f32 s = UV.u * half_step_count;
    f32 t = UV.v * half_step_count;
    s -= floorf(s);
    t -= floorf(t);
    return (s > 0.5f ? (u8)1 : (u8)0) ^ (t < 0.5f ? (u8)1 : (u8)0);
}

void shadePixelTextured(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = sampleTexture(scene->textures + shaded->material->texture_ids[0], inputs->UV, inputs->dUV).v3;
}

void shadePixelDepth(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(inputs->depth > 10 ? 1 : inputs->depth * 0.1f);
}

void shadePixelUV(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color.v2 = inputs->UV;
}

void shadePixelPosition(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleVec3(addVec3(shaded->position, getVec3Of(2.0f)), 0.5f);
}

void shadePixelNormal(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleAddVec3(shaded->normal, 0.5f, getVec3Of(0.5f));
}

void shadePixelCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(getCheckerBoardPixelValueByUV(inputs->UV, 4));
}

void shadePixelClassic(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    f32 NdotL, NdotRd, squared_distance;
    decodeMaterialSpec(shaded->material->flags, &shaded->has, &shaded->uses);

    shaded->diffuse = shaded->material->diffuse;

    if (shaded->material->texture_count) {
        shaded->diffuse = mulVec3(shaded->diffuse, sampleTexture(&scene->textures[shaded->material->texture_ids[0]], inputs->UV, inputs->dUV).v3);
        if (shaded->material->texture_count > 1) {
            vec3 normal = sampleTexture(&scene->textures[shaded->material->texture_ids[0]], inputs->UV, inputs->dUV).v3;
            normal = scaleAddVec3(normal, 0.5f, Vec3(-0.5f, -0.5f, -0.5f));
            shaded->normal = normVec3(addVec3(normal, shaded->normal));
        }
    }

    outputs->color = scene->ambient_light.color;
    shaded->viewing_direction = normVec3(subVec3(shaded->position, shaded->viewing_origin));
    if (shaded->uses.phong) {
        NdotRd = DotVec3(shaded->normal, shaded->viewing_direction);
        shaded->reflected_direction = reflectWithDot(shaded->viewing_direction, shaded->normal, NdotRd);
    }

    Light *light = scene->lights;
    for (u32 i = 0; i < scene->settings.lights; i++, light++) {
        shaded->light_direction = subVec3(light->position_or_direction, shaded->position);
        squared_distance = squaredLengthVec3(shaded->light_direction);
        shaded->light_direction = scaleVec3(shaded->light_direction, 1.0f / sqrtf(squared_distance));
        NdotL = dotVec3(shaded->normal, shaded->light_direction);
        if (NdotL > 0)
            outputs->color = mulAddVec3(
                    shadePointOnSurface(shaded, NdotL),
                    scaleVec3(light->color, light->intensity / squared_distance),
                    outputs->color);
    }

    outputs->color.x = toneMappedBaked(outputs->color.x);
    outputs->color.y = toneMappedBaked(outputs->color.y);
    outputs->color.z = toneMappedBaked(outputs->color.z);
}

void shadePixelClassicCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    shadePixelClassic(inputs, scene, shaded, outputs);

    if (!getCheckerBoardPixelValueByUV(inputs->UV, 4))
        outputs->color = scaleVec3(outputs->color, 0.5f);
}
INLINE mat4 getPrimitiveTransformationMatrix(Primitive *primitive) {
    mat3 rotation_matrix = transposedMat3(convertQuaternionToRotationMatrix(primitive->rotation));

    rotation_matrix.X = scaleVec3(rotation_matrix.X, primitive->scale.x);
    rotation_matrix.Y = scaleVec3(rotation_matrix.Y, primitive->scale.y);
    rotation_matrix.Z = scaleVec3(rotation_matrix.Z, primitive->scale.z);

    mat4 matrix = mat4fromMat3(rotation_matrix);
    matrix.W = Vec4fromVec3(primitive->position, 1);

    return matrix;
}
void rasterize(Scene *scene, Viewport *viewport, Rasterizer *rasterizer) {
    const f32 pz = viewport->projection_matrix.W.z;
    const f32 n = viewport->settings.near_clipping_plane_distance;
    const Dimensions dim = viewport->dimensions;
    static vec3 world_space_vertex_positions[6], normals[6];
    static vec4 positions[6];
    static vec2 uvs[6];
    static Shaded shaded;

    mat4 world_to_view = mat4fromMat3(viewport->camera->transform.rotation_matrix);
    world_to_view.W = Vec4fromVec3(viewport->camera->transform.position, 1);
    world_to_view = invMat4(world_to_view);
    rasterizer->world_to_clip = mulMat4(world_to_view, viewport->projection_matrix);

    bool last_UV_taken;
    f32 dot, t, one_minus_t, one_over_ABC, ABC, ABy, ABx, ACy, ACx, Cdx, Bdx, Cdy, Bdy,
            A, B, B_start, C, C_start, pixel_depth;
    u32 face_index, vertex_index, face_count, vertex_count, clipped_index,
            v1_index, out1_index, in1_index, first_x, last_x,
            v2_index, out2_index, in2_index, first_y, last_y,
            v3_index;
    u8 v1_flags, new_v2num, out1_num, in1_num,
            v2_flags, new_v1num, out2_num, in2_num,
            v3_flags, *vertex_flags = rasterizer->vertex_flags;

    vec2 pixel_min, pixel_max, uv1, uv2, uv3, lastUV, screen_transform;
    vec3 normal, attr_in, attr_out, new_v1, new_v2, pos1, pos2, pos3, norm1, norm2, norm3, ABCw, ABCp;
    vec4 v1, v2, v3, *clipped, in1, in2, out1, out2, *position;

    vec2 last_pixel_coord = Vec2(dim.f_width - 1, dim.f_height - 1);
    f32 width = dim.f_width, height = dim.f_height;
    i32 stride = dim.width;
    if (viewport->settings.antialias) {
        screen_transform.x = dim.f_width;
        screen_transform.y = dim.f_height;
        stride <<= 1;
        width *= 2;
        height *= 2;
        last_pixel_coord = Vec2(width - 1, height - 1);
    } else {
        screen_transform.x = dim.h_width;
        screen_transform.y = dim.h_height;
    }

    PixelShader pixel_shader;
    PixelShaderInputs  pixel_shader_inputs;
    PixelShaderOutputs pixel_shader_outputs;

    shaded.viewing_origin = viewport->camera->transform.position;

    bool mesh_has_normals, mesh_has_uvs, clipping_produced_an_extra_face;
    TriangleVertexIndices position_indices, normal_indices, uvs_indices;
    Pixel *pixel;
    PixelQuad *pixel_quad;
    Mesh *mesh;
    Primitive *primitive = scene->primitives;
    for (u32 primitive_id = 0; primitive_id < scene->settings.primitives; primitive_id++, primitive++) {
        if (primitive->type == PrimitiveType_Box)
            mesh = &rasterizer->default_cube_mesh;
        else if (primitive->type == PrimitiveType_Mesh)
            mesh = scene->meshes + primitive->id;
        else
            continue;

        shaded.primitive = primitive;
        shaded.material = scene->materials + primitive->material_id;
        vertex_count = mesh->vertex_count;

        // Prepare a matrix for converting from model space to clip space:
        rasterizer->model_to_world = getPrimitiveTransformationMatrix(primitive);
        rasterizer->model_to_world_inverted_transposed = transposeMat4(invMat4(rasterizer->model_to_world));

        // Execute mesh shader and skip this geometry if it got culled:
        if (!shaded.material->mesh_shader(mesh, rasterizer))
            continue;

        // Cull the vertices and skip this geometry if it's entirely outside the view frustum:


        // Check vertex positions against the frustum:
        // ----------------------------------------------------
        // For each vertex, check where it is in relation to the view frustum,
        // and collect the results into a flags array (single number bit-pattern per-vertex).
        // While doing so, keep track of which side(s) of the frustum are shared by all vertices.
        // When done, first analyze the results before continuing to phase 2.
        // Early bail-out if any of these conditions are met:
        // A. The entire mesh is outside the frustum - early return with the CULL flag.
        // B. The entire mesh is inside the frustum - early return with the INSIDE flag.
        // C. No geometric clipping is needed - early return with the INSIDE flag.
        bool needs_clipping = false;
        bool has_inside = false;

        u8 shared_directions = IS_OUT;
        u8        directions = IS_OUT;

        u8 *flags = rasterizer->vertex_flags;
        position = rasterizer->clip_space_vertex_positions;
        for (vertex_index = 0; vertex_index < vertex_count; vertex_index++, position++, flags++) {
            *flags = CULL;

            if (position->z < 0) {
                // Af at lease one vertex is outside the view frustum behind the near clipping plane,
                // the geometry needs to be checked for clipping
                needs_clipping = true;
                *flags = IS_NEAR;
                continue;
            } else directions = position->z > position->w ? IS_FAR : 0;

            if (     position->x >  position->w) directions |= IS_RIGHT;
            else if (position->x < -position->w) directions |= IS_LEFT;

            if (     position->y >  position->w) directions |= IS_ABOVE;
            else if (position->y < -position->w) directions |= IS_BELOW;

            if (directions) {
                // This vertex is outside of the view frustum.
                *flags = directions;
                // Note: This flag 'may' get removed from this vertex before the perspective-devide
                // (so it won't be skipped, essentially bringing it back) if it's still needed for culling/clipping.

                // Intersect the shared directions so-far, against this current out-direction:
                shared_directions &= directions;
                // Note: This will end-up beign zero if either:
                // A. All vertices are inside the frustum - no need for face clipping.
                // B. All vertices are outside the frustum in at least one direction shared by all.
                //   (All vertices are above and/or all vertices on the left and/or all vertices behind, etc.)
            } else {
                has_inside = true;
                *flags = IS_NDC;
            }
        }

        if (!has_inside && shared_directions)
            // All vertices are completely outside, and all share at least one out-region.
            // The entire mesh is completely outside the frustum and does not intersect it in any way.
            // It can be safely culled altogether.
            continue;

        // The mesh intersects the frustum in some way

        pixel_shader = shaded.material->pixel_shader;

        mesh_has_uvs     = mesh->uvs_count     != 0;
        mesh_has_normals = mesh->normals_count != 0;
        face_count = mesh->triangle_count;

        // Check its faces as well and check for clipping cases:
        for (face_index = 0; face_index < face_count; face_index++) {
            // Fetch the index and out-direction flags of each of the face's vertices:
            position_indices = mesh->vertex_position_indices[face_index];

            v1_index = position_indices.v1;
            v2_index = position_indices.v2;
            v3_index = position_indices.v3;

            v1_flags = vertex_flags[v1_index] & IS_OUT;
            v2_flags = vertex_flags[v2_index] & IS_OUT;
            v3_flags = vertex_flags[v3_index] & IS_OUT;

            positions[0] = rasterizer->clip_space_vertex_positions[v1_index];
            positions[1] = rasterizer->clip_space_vertex_positions[v2_index];
            positions[2] = rasterizer->clip_space_vertex_positions[v3_index];

            world_space_vertex_positions[0] = rasterizer->world_space_vertex_positions[v1_index];
            world_space_vertex_positions[1] = rasterizer->world_space_vertex_positions[v2_index];
            world_space_vertex_positions[2] = rasterizer->world_space_vertex_positions[v3_index];

            if (mesh_has_normals) {
                normal_indices = mesh->vertex_normal_indices[face_index];
                for (u8 i = 0; i < 3; i++) normals[i] = rasterizer->world_space_vertex_normals[normal_indices.ids[i]];
            }

            if (mesh_has_uvs) {
                uvs_indices = mesh->vertex_uvs_indices[face_index];
                for (u8 i = 0; i < 3; i++) uvs[i] = mesh->vertex_uvs[uvs_indices.ids[i]];
            }

            if (viewport->settings.cull_back_faces) {
                // Check face orientation "early" (before the perspective divide)
                // Compute a normal vector of the face from these 2 direction vectors:
                pos1 = positions[0].v3;
                pos2 = positions[1].v3;
                pos3 = positions[2].v3;
                normal = crossVec3(subVec3(pos3, pos1), subVec3(pos2, pos1));

                // Dot the vector from the face to the origin with the normal:
                dot = normal.z*(pz - pos1.z) - normal.y*pos1.y - normal.x*pos1.x;
                if (dot < 0.0001f) {
                    // if the angle is 90 the face is at grazing angle to the camera.
                    // if the angle is greater than 90 degrees the face faces away from the camera.
                    continue;
                }
            }

            clipping_produced_an_extra_face = false;
            if (needs_clipping) {
                if ((v1_flags | v2_flags) | v3_flags) {
                    // One or more vertices are outside - check edges for intersections:
                    if ((v1_flags & v2_flags) & v3_flags) {
                        // All vertices share one or more out-direction(s).
                        // The face is fully outside the frustum, and does not intersect it.
                        continue;
                        // Note: This includes the cases where "all" 3 vertices cross the near clipping plane.
                        // Below there are checks for when "any" of the vertices cross it (1 or 2, but not 3).
                    }

                    // One or more vertices are outside, and no out-direction is shared across them.
                    // The face is visible in the view frustum in some way.
                    vertex_flags[v1_index] |= IS_NDC;
                    vertex_flags[v2_index] |= IS_NDC;
                    vertex_flags[v3_index] |= IS_NDC;

                    // Check if any vertex crosses the near clipping plane:
                    if (v1_flags & IS_NEAR ||
                        v2_flags & IS_NEAR ||
                        v3_flags & IS_NEAR) {
                        // There is at least one vertex behind the near clipping plane.
                        // The face needs to be clipped
                        // Clipping is done only against the near clipping plane, so there are only 2 possible cases:
                        // 1: One vertex is inside the frustum and the other two are outside beyond the near clipping plane.
                        // 2: Two vertices are inside the frustum and the third is outside beyond the near clipping plane.

                        // Figure out which case applies to this current face, and which vertices are in/out:
                        in2_num   = out2_num = 0;
                        in2_index = out2_index = -1;
                        if (v1_flags & IS_NEAR) {
                            out1_index = v1_index;
                            out1_num = 1;
                            if (v2_flags & IS_NEAR) {
                                out2_index = v2_index;
                                out2_num = 2;
                                in1_index = v3_index;
                                in1_num = 3;
                            } else {
                                in1_index = v2_index;
                                in1_num = 2;
                                if (v3_flags & IS_NEAR) {
                                    out2_index = v3_index;
                                    out2_num = 3;
                                } else {
                                    in2_index = v3_index;
                                    in2_num = 3;
                                }
                            }
                        } else {
                            in1_index = v1_index;
                            in1_num = 1;
                            if (v2_flags & IS_NEAR) {
                                out1_index = v2_index;
                                out1_num = 2;
                                if (v3_flags & IS_NEAR) {
                                    out2_index = v3_index;
                                    out2_num = 3;
                                } else {
                                    in2_index = v3_index;
                                    in2_num = 3;
                                }
                            } else {
                                in2_index = v2_index;
                                in2_num = 2;
                                out1_index = v3_index;
                                out1_num = 3;
                            }
                        }
                        in1  = rasterizer->clip_space_vertex_positions[in1_index];
                        out1 = rasterizer->clip_space_vertex_positions[out1_index];

                        // Compute and store the (relative)amount by which the FIRST outside
                        // vertex would need to be moved 'inwards' towards the FIRST inside vertex:
                        t = out1.z / (out1.z - in1.z);
                        one_minus_t = 1 - t;
                        // Note:
                        // Clip space is set up such that a depth of 0 is where the near clipping plane is.
                        // So 'out1z' would be (negative)distance of the vertex from the near clipping plane,
                        // representing the 'amount' by which the 'outside' vertex would needs to be 'pushed' forward
                        // to land it on the near clipping plane. The denominator here is the 'full' depth-distance
                        // between the 2 vertices - the sum of distances of the 2 vertices from to the clipping plane.
                        // The ratio of the former from the latter is thus the (positive)interpolation amount 't' (0->1).
                        // Since 'out1z' would be negative here, 'in1z' is negated as well to get the full (negative)sum.
                        // Since both the numerator and the denominator here would be negative, the result is positive.
                        // The interpolation value 't' is with respect to the 'outside' vertex, and so would be multiplied
                        // by any attribute-value of the 'outside' vertex. The complement of that (1 - t) would be multiplied
                        // by any attribute-value of the 'inside' vertex, and the sum would be the interpolated value.
                        // *The same logic applies for the second interpolation in either of it's 2 cases below.

                        // Compute the index of the "unshared" position-value(s) of the 'clipped' vertex of this face:
                        clipped_index = out1_num - 1;
                        clipped = positions + clipped_index;

                        // Compute the new clip-space coordinates of the clipped-vertex:
                        new_v1.z = fast_mul_add(out1.z, one_minus_t, t*in1.z);
                        clipped->x = new_v1.x = fast_mul_add(out1.x, one_minus_t, t*in1.x);
                        clipped->y = new_v1.y = fast_mul_add(out1.y, one_minus_t, t*in1.y);
                        clipped->z = 0;
                        clipped->w = n;
                        // Note:
                        // The 'Z' coordinate of this new vertex position in clip-space is set to '0' since it has
                        // now been moved onto the clipping plane itself. Similarly, the 'W' coordinate is set to
                        // what the "original" Z-depth value this vertex "would have had", had it been on the
                        // near clipping plane in view-space in the first place.
                        // *The same logic applies for the second interpolation in either of it's 2 cases below.

                        attr_in  = rasterizer->world_space_vertex_positions[position_indices.ids[in1_num - 1]];
                        attr_out = rasterizer->world_space_vertex_positions[position_indices.ids[out1_num - 1]];
                        world_space_vertex_positions[clipped_index] = scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t));

                        if (mesh_has_normals) {
                            attr_in  = rasterizer->world_space_vertex_normals[normal_indices.ids[in1_num - 1]];
                            attr_out = rasterizer->world_space_vertex_normals[normal_indices.ids[out1_num - 1]];
                            normals[clipped_index] = normVec3(scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t)));
                        }

                        if (mesh_has_uvs) {
                            attr_in.v2  = mesh->vertex_uvs[uvs_indices.ids[in1_num - 1]];
                            attr_out.v2 = mesh->vertex_uvs[uvs_indices.ids[out1_num - 1]];
                            uvs[clipped_index] = scaleAddVec2(attr_out.v2, one_minus_t, scaleVec2(attr_in.v2, t));
                        }

                        if (out2_num) {
                            // One vertex is inside the frustum, and the other two are outside beyond the near clipping plane.
                            // The triangle just needs to get smaller by moving the 2 outside-vertices back to the near clipping plane.

                            // Compute and store the (relative)amount by which the SECOND outside
                            // vertex needs to be moved inwards towards the FIRST inside vertex:
                            out2 = rasterizer->clip_space_vertex_positions[out2_index];
                            t = out2.z / (out2.z - in1.z);
                            one_minus_t = 1 - t;

                            // Compute the index of the "unshared" position-value(s) of the 'clipped' vertex of this face:
                            clipped_index = out2_num - 1;
                            clipped = positions + clipped_index;

                            // Compute the new clip-space coordinates of the clipped-vertex:
                            clipped->x = fast_mul_add(out2.x, one_minus_t, t*in1.x);
                            clipped->y = fast_mul_add(out2.y, one_minus_t, t*in1.y);
                            clipped->z = 0;
                            clipped->w = n;

                            attr_in  = rasterizer->world_space_vertex_positions[position_indices.ids[in1_num - 1]];
                            attr_out = rasterizer->world_space_vertex_positions[position_indices.ids[out2_num - 1]];
                            world_space_vertex_positions[clipped_index] = scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t));

                            if (mesh_has_normals) {
                                attr_in  = rasterizer->world_space_vertex_normals[normal_indices.ids[in1_num - 1]];
                                attr_out = rasterizer->world_space_vertex_normals[normal_indices.ids[out2_num - 1]];
                                normals[clipped_index] = normVec3(scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t)));
                            }

                            if (mesh_has_uvs) {
                                attr_in.v2  = mesh->vertex_uvs[uvs_indices.ids[in1_num - 1]];
                                attr_out.v2 = mesh->vertex_uvs[uvs_indices.ids[out2_num - 1]];
                                uvs[clipped_index] = scaleAddVec2(attr_out.v2, one_minus_t, scaleVec2(attr_in.v2, t));
                            }
                        } else {
                            // Two vertices are inside the frustum, and the third one is behind the near clipping plane.
                            // Clipping forms a quad which needs to be split into 2 triangles.
                            // The first one is formed from the original one, by moving the vertex that is behind the
                            // clipping plane right up-to the near clipping plane itself (exactly as in the first case).
                            // The second triangle is a new triangle that needs to be created, from the 2 vertices that
                            // are inside, plus a new vertex that would need to be interpolated by moving the same vertex
                            // that is outside up-to the near clipping plane but towards the other vertex that is inside.

                            // Compute and store the (relative)amount by which the FIRST outside vertex
                            // needs to be moved inwards towards the SECOND inside vertex:
                            in2 = rasterizer->clip_space_vertex_positions[in2_index];
                            t = out1.z / (out1.z - in2.z);
                            one_minus_t = 1 - t;

                            new_v2 = scaleAddVec3(out1.v3, one_minus_t, scaleVec3(in2.v3, t));

                            // Determine orientation:
                            // Compute 2 direction vectors forming a plane for the face:
                            // Compute a normal vector of the face from these 2 direction vectors:
                            normal = crossVec3(subVec3(new_v1, in2.v3), subVec3(new_v2, in2.v3));

                            // Dot the vector from the face to the origin with the normal:
                            dot = normal.z*(pz - in2.z) - normal.y*in2.y - normal.x*in2.x;
                            if (dot > 0) {
                                // if the angle is greater than 90 degrees the face is facing the camera
                                new_v1num = 2;
                                new_v2num = 1;
                            } else {
                                // if the angle is 90 the face is at grazing angle to the camera.
                                // if the angle is greater than 90 degrees the face faces away from the camera.
                                new_v1num = 1;
                                new_v2num = 2;
                            }

                            // Since this vertex belongs to an 'extra' new face, the index is offset to that index-space
                            clipped_index = 3 + new_v2num;
                            positions[3] = in2;
                            positions[3 + new_v1num] = *clipped;
                            clipped = positions + clipped_index;

                            // Compute the new clip-space coordinates of the clipped-vertex:
                            clipped->x = new_v2.x;
                            clipped->y = new_v2.y;
                            clipped->z = 0;
                            clipped->w = n;

                            attr_in  = rasterizer->world_space_vertex_positions[position_indices.ids[in2_num - 1]];
                            attr_out = rasterizer->world_space_vertex_positions[position_indices.ids[out1_num - 1]];
                            world_space_vertex_positions[clipped_index] = scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t));
                            world_space_vertex_positions[3] = attr_in;
                            world_space_vertex_positions[3 + new_v1num] = world_space_vertex_positions[out1_num - 1];

                            if (mesh_has_normals) {
                                attr_in  = rasterizer->world_space_vertex_normals[normal_indices.ids[in2_num - 1]];
                                attr_out = rasterizer->world_space_vertex_normals[normal_indices.ids[out1_num - 1]];
                                normals[clipped_index] = normVec3(scaleAddVec3(attr_out, one_minus_t, scaleVec3(attr_in, t)));
                                normals[3] = attr_in;
                                normals[3 + new_v1num] = normals[out1_num - 1];
                            }

                            if (mesh_has_uvs) {
                                attr_in.v2  = mesh->vertex_uvs[uvs_indices.ids[in2_num - 1]];
                                attr_out.v2 = mesh->vertex_uvs[uvs_indices.ids[out1_num - 1]];
                                uvs[clipped_index] = scaleAddVec2(attr_out.v2, one_minus_t, scaleVec2(attr_in.v2, t));
                                uvs[3] = attr_in.v2;
                                uvs[3 + new_v1num] = uvs[out1_num - 1];
                            }

                            clipping_produced_an_extra_face = true;
                        }
                    }
                    // Even if no vertices are behind the view frustum the face is still visible.
                    // It either intersects the frustum in direction(s) other than the near clipping plane,
                    // or it may fully surround the whole view frustum.
                    // No geometric clipping is needed, but the face can not be culled.
                } // else: No vertices are outside the frustum (the face is fully within it).
            } // else: No vertex is behind the near clipping plane.
            // Since geometric clipping is done only against the near clipping plane, there is nothing to clip.
            // The other sides of the frustum would get raster-clipped later by clamping pixels outside the screen.

            vertex_count = clipping_produced_an_extra_face ? 6 : 3;
            position = positions;
            for (vertex_index = 0; vertex_index < vertex_count; vertex_index++, position++) {
                // The perspective divide should finalize normalizing the depth values
                // into the 0 -> 1 space, by dividing clip-space 'z' by clip-space 'w'
                // (coordinates that came out of the multiplication by the projection matrix)
                // screen_z = clip_z / clip_w = Z[i] / W[i]
                // However: The rasterizer is going to then need to convert this value
                // into a spectrum of values that are all divided by 'clip_w'
                // in order to linearly interpolate it there (from vertex values to a pixel value).

                // Store reciprocals for use in rasterization:
                position->w = 1.0f / position->w;
                position->v3 = scaleVec3(position->v3, position->w);
                // Scale the normalized screen to the pixel size:
                // (from normalized size of -1->1 horizontally and vertically having a width and height of 2)
                position->x *= screen_transform.x;
                position->y *= -screen_transform.y;

                // Move the screen up and to the right appropriately,
                // such that it goes 0->width horizontally and 0->height vertically:
                position->x += screen_transform.x;
                position->y += screen_transform.y;

                // Scale the normalized screen to the pixel size:
                // (from normalized size of -1->1 horizontally and vertically having a width and height of 2)
                // Then, move the screen up and to the right appropriately,
                // such that it goes 0->width horizontally and 0->height vertically:
//                position->x = fast_mul_add(position->x, screen_transform.x, screen_transform.x);
//                position->x = fast_mul_add(position->y, -screen_transform.y, screen_transform.y);
            }

            vertex_count = clipping_produced_an_extra_face ? 2 : 1;
            v1_index = 0;
            v2_index = 1;
            v3_index = 2;

            for (vertex_index = 0; vertex_index < vertex_count; vertex_index++, v1_index += 3, v2_index += 3, v3_index += 3) {
                if (pixel_shader) {
                    v1 = positions[v1_index];
                    v2 = positions[v2_index];
                    v3 = positions[v3_index];

                    // Cull this triangle against the edges of the viewport:
                    pixel_min = minVec2(v1.v3.v2, minVec2(v2.v3.v2, v3.v3.v2));
                    if (pixel_min.x >= width ||
                        pixel_min.y >= height)
                        continue;

                    pixel_max = maxVec2(v1.v3.v2, maxVec2(v2.v3.v2, v3.v3.v2));
                    if (pixel_max.x < 0 ||
                        pixel_max.y < 0)
                        continue;

                    // Clip the bounds of the triangle to the viewport:
                    pixel_min = clampVec2ToZero(pixel_min);
                    pixel_max = clampVec2ToUpper(pixel_max, last_pixel_coord);

                    // Compute area components:
                    ABy = v2.y - v1.y;
                    ABx = v2.x - v1.x;

                    ACy = v3.y - v1.y;
                    ACx = v3.x - v1.x;

                    ABC = ACx*ABy - ACy*ABx; // (Cx - Ax)(By - Ay) - (Cy - Ay)(Bx - Ax)

                    // Cull faces facing backwards:
                    if (ABC <= 0)
                        return;

                    // Floor bounds coordinates down to their integral component:
                    first_x = (u32)pixel_min.x;
                    first_y = (u32)pixel_min.y;
                    last_x  = (u32)pixel_max.x;
                    last_y  = (u32)pixel_max.y;

                    pixel_min.x = (f32)first_x;
                    pixel_min.y = (f32)first_y;
                    pixel_max.x = (f32)last_x;
                    pixel_max.y = (f32)last_y;

                    // Compute edge exclusions:
                    // Drawing: Top-down
                    // Origin: Top-left
                    // Shadow rules: Top/Left
                    // Winding: CW (Flipped vertically due to top-down drawing!)
//                    const bool exclude_edge_1 = ABy > 0;
//                    const bool exclude_edge_2 = v2.y > v3.y;
//                    const bool exclude_edge_3 = ACy < 0;

                    // Compute weight constants:
                    one_over_ABC = 1.0f / ABC;

                    Cdx =  ABy * one_over_ABC;
                    Bdx = -ACy * one_over_ABC;

                    Cdy = -ABx * one_over_ABC;
                    Bdy =  ACx * one_over_ABC;

                    // Compute initial areal coordinates for the first pixel center:
                    pixel_min = addVec2(pixel_min, Vec2(0.5f, 0.5f));
                    C_start = Cdx*pixel_min.x + Cdy*pixel_min.y + (v1.y*v2.x - v1.x*v2.y) * one_over_ABC;
                    B_start = Bdx*pixel_min.x + Bdy*pixel_min.y + (v3.y*v1.x - v3.x*v1.y) * one_over_ABC;

                    pos1 = world_space_vertex_positions[v1_index];
                    pos2 = world_space_vertex_positions[v2_index];
                    pos3 = world_space_vertex_positions[v3_index];

                    norm1 = normals[v1_index];
                    norm2 = normals[v2_index];
                    norm3 = normals[v3_index];

                    uv1 = uvs[v1_index];
                    uv2 = uvs[v2_index];
                    uv3 = uvs[v3_index];

                    // Scan the bounds:
                    for (u32 y = first_y; y <= last_y; y++, C_start += Cdy, B_start += Bdy) {
                        B = B_start;
                        C = C_start;

                        last_UV_taken = false;

                        for (u32 x = first_x; x <= last_x; x++, B += Bdx, C += Cdx) {
                            if (Bdx < 0 && B < 0 ||
                                Cdx < 0 && C < 0)
                                break;

                            A = 1 - B - C;

                            // Skip the pixel if it's outside:
                            if (fminf(A, fminf(B, C)) < 0)
                                continue;

                            // If the pixel is on a shadow-edge, skip it:
//                             if ((A == 0 && exclude_edge_1) ||
//                                 (B == 0 && exclude_edge_2) ||
//                                 (C == 0 && exclude_edge_3))
//                                 continue;

                            // Cull and test pixel based on its depth:
                            if (viewport->settings.antialias) {
                                pixel_quad = viewport->pixels + (viewport->dimensions.stride * (y >> 1)) + (x >> 1);
                                pixel = &pixel_quad->quad[y & 1][x & 1];
                            } else {
                                pixel_quad = viewport->pixels + (viewport->dimensions.stride * y) + x;
                                pixel = &pixel_quad->TL;
                            }
                            pixel_depth = A*v1.z + B*v2.z + C*v3.z;
                            if (pixel_depth < 0 ||
                                pixel_depth > 1 ||
                                pixel_depth > pixel->depth)
                                continue;

                            pixel->depth = pixel_depth;

                            ABCw = Vec3(A*v1.w, B*v2.w, C*v3.w);
                            pixel_shader_inputs.depth = 1.0f / (ABCw.A + ABCw.B + ABCw.C);
                            ABCp = scaleVec3(ABCw, pixel_shader_inputs.depth);

                            shaded.position = scaleAddVec3(pos1, ABCp.A, (scaleAddVec3(pos2, ABCp.B, scaleVec3(pos3, ABCp.C))));
                            if (mesh_has_normals) shaded.normal = normVec3(scaleAddVec3(norm1,   ABCp.A, (scaleAddVec3(norm2,   ABCp.B, scaleVec3(norm3,   ABCp.C)))));
                            if (mesh_has_uvs) {
                                pixel_shader_inputs.UV = scaleAddVec2(uv1, ABCp.A, (scaleAddVec2(uv2, ABCp.B, scaleVec2(uv3, ABCp.C))));

                                if (last_UV_taken) {
                                    pixel_shader_inputs.dUV = subVec2(pixel_shader_inputs.UV, lastUV);
                                } else {
                                    ABCp.B = B + Bdx;
                                    ABCp.C = C + Cdx;
                                    ABCp.A =  v1.w * (1 - ABCp.B - ABCp.C);
                                    ABCp.B *= v2.w;
                                    ABCp.C *= v3.w;
                                    ABCp = scaleVec3(ABCw, 1.0f / (ABCp.A + ABCp.B + ABCp.C));
                                    pixel_shader_inputs.dUV = subVec2(scaleAddVec2(uv1, ABCp.A, (scaleAddVec2(uv2, ABCp.B, scaleVec2(uv3, ABCp.C)))), pixel_shader_inputs.UV);
                                    last_UV_taken = true;
                                }

                                if (pixel_shader_inputs.dUV.u < 0) pixel_shader_inputs.dUV.u = -pixel_shader_inputs.dUV.u;
                                if (pixel_shader_inputs.dUV.v < 0) pixel_shader_inputs.dUV.v = -pixel_shader_inputs.dUV.v;
                                lastUV = pixel_shader_inputs.UV;
                            }
                            pixel_shader_inputs.coords.x = x;
                            pixel_shader_inputs.coords.y = y;
                            pixel_shader_outputs.color = getVec3Of(0);
                            pixel_shader_outputs.z = pixel_depth;
                            pixel_shader_outputs.opacity = 1;
                            pixel_shader(&pixel_shader_inputs, scene, &shaded, &pixel_shader_outputs);
                            pixel_shader_outputs.color.x *= FLOAT_TO_COLOR_COMPONENT;
                            pixel_shader_outputs.color.y *= FLOAT_TO_COLOR_COMPONENT;
                            pixel_shader_outputs.color.z *= FLOAT_TO_COLOR_COMPONENT;
                            pixel_shader_outputs.color.x *= pixel_shader_outputs.color.x;
                            pixel_shader_outputs.color.y *= pixel_shader_outputs.color.y;
                            pixel_shader_outputs.color.z *= pixel_shader_outputs.color.z;
                            setPixel((i32)x, (i32)y, pixel_shader_outputs.z, pixel_shader_outputs.color, pixel_shader_outputs.opacity, viewport);
                        }
                    }
                }
                if (viewport->settings.show_wire_frame || !pixel_shader) {
                    vec3 color = Color(vertex_index ? Red : White);
                    new_v1 = positions[v1_index].v3;
                    new_v2 = positions[v2_index].v3;
                    if (viewport->settings.antialias) {
                        new_v1.x *= 0.5f;
                        new_v1.y *= 0.5f;
                        new_v2.x *= 0.5f;
                        new_v2.y *= 0.5f;
                    }
                    new_v1.z -= 0.001f;
                    new_v2.z -= 0.001f;
                    drawLine(new_v1.x, new_v1.y, new_v1.z,
                             new_v2.x, new_v2.y, new_v2.z,
                             color, 1, 0, viewport);
                    new_v1 = positions[v2_index].v3;
                    new_v2 = positions[v3_index].v3;
                    if (viewport->settings.antialias) {
                        new_v1.x *= 0.5f;
                        new_v1.y *= 0.5f;
                        new_v2.x *= 0.5f;
                        new_v2.y *= 0.5f;
                    }
                    new_v1.z -= 0.001f;
                    new_v2.z -= 0.001f;
                    drawLine(new_v1.x, new_v1.y, new_v1.z,
                             new_v2.x, new_v2.y, new_v2.z,
                             color, 1, 0, viewport);
                    new_v1 = positions[v3_index].v3;
                    new_v2 = positions[v1_index].v3;
                    if (viewport->settings.antialias) {
                        new_v1.x *= 0.5f;
                        new_v1.y *= 0.5f;
                        new_v2.x *= 0.5f;
                        new_v2.y *= 0.5f;
                    }
                    new_v1.z -= 0.001f;
                    new_v2.z -= 0.001f;
                    drawLine(new_v1.x, new_v1.y, new_v1.z,
                             new_v2.x, new_v2.y, new_v2.z,
                             color, 1, 0, viewport);
                }
            }
        }
    }
}




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



bool cullAndClipEdge(Edge *edge, Viewport *viewport) {
    f32 distance = viewport->settings.near_clipping_plane_distance;
    vec3 A = edge->from;
    vec3 B = edge->to;

    u8 out = (A.z < distance) | ((B.z < distance) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, (distance - A.z) / (B.z - A.z));
        else         B = lerpVec3(B, A, (distance - B.z) / (A.z - B.z));
    }

    distance = viewport->settings.far_clipping_plane_distance;
    out = (A.z > distance) | ((B.z > distance) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, (A.z - distance) / (A.z - B.z));
        else         B = lerpVec3(B, A, (B.z - distance) / (B.z - A.z));
    }

    // Left plane (facing to the right):
    vec3 N = Vec3(viewport->camera->focal_length, 0, viewport->dimensions.width_over_height);
    f32 NdotA = dotVec3(N, A);
    f32 NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Right plane (facing to the left):
    N.x = -N.x;
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Bottom plane (facing up):
    N = Vec3(0, viewport->camera->focal_length, 1);
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Top plane (facing down):
    N.y = -N.y;
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    edge->from = A;
    edge->to   = B;

    return true;
}

INLINE bool projectEdge(Edge *edge, Viewport *viewport) {
    if (!cullAndClipEdge(edge, viewport))
        return false;

    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->projection_matrix);
    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->projection_matrix);

    // Project:
    edge->from = scaleVec3(Vec3fromVec4(A), 1.0f / A.w);
    edge->to   = scaleVec3(Vec3fromVec4(B), 1.0f / B.w);

    // NDC->screen:
    edge->from.x += 1; edge->from.x *= viewport->dimensions.h_width;
    edge->to.x   += 1; edge->to.x   *= viewport->dimensions.h_width;
    edge->from.y += 1; edge->from.y *= viewport->dimensions.h_height;
    edge->to.y   += 1; edge->to.y   *= viewport->dimensions.h_height;

    // Flip Y:
    edge->from.y = viewport->dimensions.f_height - edge->from.y;
    edge->to.y   = viewport->dimensions.f_height - edge->to.y;

    return true;
}

INLINE void drawEdge(Edge *edge, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    if (projectEdge(edge, viewport))
        drawLine(edge->from.x, edge->from.y, edge->from.z,
                 edge->to.x,   edge->to.y,   edge->to.z,
                 color, opacity, line_width, viewport);
}

INLINE void convertPositionAndDirectionToObjectSpace(
        vec3 position,
        vec3 dir,
        Primitive *primitive,
        vec3 *out_position,
        vec3 *out_direction
) {
    *out_position = primitive->flags & IS_TRANSLATED ?
                    subVec3(position, primitive->position) :
                    position;

    if (primitive->flags & IS_ROTATED) {
        quat inv_rotation = conjugate(primitive->rotation);
        *out_position = mulVec3Quat(*out_position, inv_rotation);
        *out_direction = mulVec3Quat(dir, inv_rotation);
    } else
        *out_direction = dir;

    if (primitive->flags & IS_SCALED) {
        vec3 inv_scale = oneOverVec3(primitive->scale);
        *out_position = mulVec3(*out_position, inv_scale);
        if (primitive->flags & IS_SCALED_NON_UNIFORMLY)
            *out_direction = normVec3(mulVec3(*out_direction, inv_scale));
    }
}

INLINE vec3 convertPositionToWorldSpace(vec3 position, Primitive *primitive) {
    if (primitive->flags & IS_SCALED)     position = mulVec3(    position, primitive->scale);
    if (primitive->flags & IS_ROTATED)    position = mulVec3Quat(position, primitive->rotation);
    if (primitive->flags & IS_TRANSLATED) position = addVec3(    position, primitive->position);
    return position;
}
INLINE vec3 convertPositionToObjectSpace(vec3 position, Primitive *primitive) {
    if (primitive->flags & IS_TRANSLATED) position = subVec3(    position, primitive->position);
    if (primitive->flags & IS_ROTATED)    position = mulVec3Quat(position, conjugate(primitive->rotation));
    if (primitive->flags & IS_SCALED)     position = mulVec3(position, oneOverVec3(primitive->scale));
    return position;
}

INLINE vec3 convertDirectionToWorldSpace(vec3 direction, Primitive *primitive) {
    if (primitive->flags & IS_SCALED_NON_UNIFORMLY) direction = mulVec3(direction, oneOverVec3(primitive->scale));
    if (primitive->flags & IS_ROTATED)              direction = mulVec3Quat(direction,         primitive->rotation);
    return direction;
}

AABB getPrimitiveAABB(Primitive *primitive) {
    AABB aabb;

    aabb.max = getVec3Of(primitive->type == PrimitiveType_Tetrahedron ? SQRT3 / 3 : 1);
    aabb.min = invertedVec3(aabb.max);

    return aabb;
}

void rotatePrimitive(Primitive *primitive, f32 yaw, f32 pitch, f32 roll) {
    quat rotation;
    if (roll) {
        rotation.amount = roll;
        rotation.axis.x = 0;
        rotation.axis.y = 0;
        rotation.axis.z = 1;
    } else
        rotation = getIdentityQuaternion();

    if (pitch) {
        quat pitch_rotation;
        pitch_rotation.amount = pitch;
        pitch_rotation.axis.x = 1;
        pitch_rotation.axis.y = 0;
        pitch_rotation.axis.z = 0;
        pitch_rotation = normQuat(pitch_rotation);
        rotation = mulQuat(rotation, pitch_rotation);
        rotation = normQuat(rotation);
    }

    if (yaw) {
        quat yaw_rotation;
        yaw_rotation.amount = yaw;
        yaw_rotation.axis.x = 0;
        yaw_rotation.axis.y = 1;
        yaw_rotation.axis.z = 0;
        yaw_rotation = normQuat(yaw_rotation);
        rotation = mulQuat(rotation, yaw_rotation);
        rotation = normQuat(rotation);
    }

    primitive->rotation = mulQuat(primitive->rotation, rotation);
    primitive->rotation = normQuat(primitive->rotation);
}

void transformAABB(AABB *aabb, Primitive *primitive) {
    f32 x = aabb->min.x;
    f32 y = aabb->min.y;
    f32 z = aabb->min.z;
    f32 X = aabb->max.x;
    f32 Y = aabb->max.y;
    f32 Z = aabb->max.z;

    vec3 v[8] = {
            {x, y, z},
            {x, y, Z},
            {x, Y, z},
            {x, Y, Z},
            {X, y, z},
            {X, y, Z},
            {X, Y, z},
            {X, Y, Z}
    };

    x = y = z = +INFINITY;
    X = Y = Z = -INFINITY;

    vec3 position;
    for (u8 i = 0; i < 8; i++) {
        position = convertPositionToWorldSpace(v[i], primitive);

        if (position.x > X) X = position.x;
        if (position.y > Y) Y = position.y;
        if (position.z > Z) Z = position.z;
        if (position.x < x) x = position.x;
        if (position.y < y) y = position.y;
        if (position.z < z) z = position.z;
    }

    aabb->min.x = x;
    aabb->min.y = y;
    aabb->min.z = z;
    aabb->max.x = X;
    aabb->max.y = Y;
    aabb->max.z = Z;
}



INLINE void rotateXform3(xform3 *xform, f32 yaw, f32 pitch, f32 roll) {
    if (yaw)   yawMat3(  yaw,   &xform->yaw_matrix);
    if (pitch) pitchMat3(pitch, &xform->pitch_matrix);
    if (roll)  rollMat3( roll,  &xform->roll_matrix);

    xform->rotation_matrix = mulMat3(mulMat3(xform->pitch_matrix, xform->yaw_matrix), xform->roll_matrix);
    xform->rotation_matrix_inverted = transposedMat3(xform->rotation_matrix);

    xform->rotation          = convertRotationMatrixToQuaternion(xform->rotation_matrix);
    xform->rotation_inverted = convertRotationMatrixToQuaternion(xform->rotation_matrix_inverted);

    xform->matrix = mulMat3(xform->matrix, xform->rotation_matrix);
}

u32 getTextureMemorySize(Texture *texture, char* file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    platform->readFromFile(&texture->width,  sizeof(u16),  file);
    platform->readFromFile(&texture->height, sizeof(u16),  file);
    platform->readFromFile(&texture->filter, sizeof(bool), file);
    platform->readFromFile(&texture->mipmap, sizeof(bool), file);

    u16 mip_width  = texture->width;
    u16 mip_height = texture->height;

    u32 memory_size = 0;

    do {
        memory_size += sizeof(TextureMip);
        memory_size += mip_width * mip_height * sizeof(vec4);
        if (texture->filter) {
            memory_size += (mip_width + 1) * (mip_height + 1) * sizeof(TexelQuad);
            memory_size += (mip_height + 1) * sizeof(TexelQuad*);
        }

        mip_width /= 2;
        mip_height /= 2;
    } while (texture->mipmap && mip_width > 2 && mip_height > 2);

    platform->closeFile(file);

    return memory_size;
}

void loadTextureFromFile(Texture *texture, char* file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path);
    platform->readFromFile(&texture->width,  sizeof(u16),  file);
    platform->readFromFile(&texture->height, sizeof(u16),  file);
    platform->readFromFile(&texture->filter, sizeof(bool), file);
    platform->readFromFile(&texture->mipmap, sizeof(bool), file);
    platform->readFromFile(&texture->wrap,   sizeof(bool), file);
    platform->readFromFile(&texture->mip_count, sizeof(u8), file);

    texture->mips = (TextureMip*)allocateMemory(memory, sizeof(TextureMip) * texture->mip_count);

    u32 size, height, stride;
    TextureMip *texture_mip = texture->mips;
    for (u8 mip_index = 0; mip_index < texture->mip_count; mip_index++, texture_mip++) {
        platform->readFromFile(&texture_mip->width,  sizeof(u16), file);
        platform->readFromFile(&texture_mip->height, sizeof(u16), file);

        size = sizeof(vec4) * texture_mip->width * texture_mip->height;
        texture_mip->texels = (vec4*)allocateMemory(memory, size);
        platform->readFromFile(texture_mip->texels, size, file);

        if (texture->filter) {
            height = texture_mip->height + 1;
            stride = texture_mip->width  + 1;

            size = sizeof(TexelQuad) * height * stride;
            texture_mip->texel_quads = (TexelQuad*)allocateMemory(memory, size);
            platform->readFromFile(texture_mip->texel_quads, size, file);

            size = sizeof(TexelQuad*) * height;
            texture_mip->texel_quad_lines = (TexelQuad**)allocateMemory(memory, size);
            TexelQuad *line_start = texture_mip->texel_quads;
            TexelQuad **line = texture_mip->texel_quad_lines;
            for (u16 y = 0; y < height; y++, line++, line_start += stride) *line = line_start;
        }
    }

    platform->closeFile(file);
}

u32 getMeshMemorySize(Mesh *mesh, char *file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
    platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

    u32 memory_size = sizeof(vec3) * mesh->vertex_count;
    memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    memory_size += sizeof(EdgeVertexIndices) * mesh->edge_count;

    if (mesh->uvs_count) {
        memory_size += sizeof(vec2) * mesh->uvs_count;
        memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    }
    if (mesh->normals_count) {
        memory_size += sizeof(vec3) * mesh->normals_count;
        memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    }

    platform->closeFile(file);

    return memory_size;
}

void loadMeshFromFile(Mesh *mesh, char *file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path);

    mesh->vertex_normals          = null;
    mesh->vertex_normal_indices   = null;
    mesh->vertex_uvs              = null;
    mesh->vertex_uvs_indices      = null;

    platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
    platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

    mesh->vertex_positions        = (vec3*                 )allocateMemory(memory, sizeof(vec3)                  * mesh->vertex_count);
    mesh->vertex_position_indices = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
    mesh->edge_vertex_indices     = (EdgeVertexIndices*    )allocateMemory(memory, sizeof(EdgeVertexIndices)     * mesh->edge_count);

    platform->readFromFile(mesh->vertex_positions,             sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->readFromFile(mesh->vertex_position_indices,      sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->readFromFile(mesh->edge_vertex_indices,          sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
    if (mesh->uvs_count) {
        mesh->vertex_uvs         = (vec2*                 )allocateMemory(memory, sizeof(vec2)                  * mesh->uvs_count);
        mesh->vertex_uvs_indices = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_uvs,               sizeof(vec2)                  * mesh->uvs_count,      file);
        platform->readFromFile(mesh->vertex_uvs_indices,       sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }
    if (mesh->normals_count) {
        mesh->vertex_normals          = (vec3*                 )allocateMemory(memory, sizeof(vec3)                  * mesh->normals_count);
        mesh->vertex_normal_indices   = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_normals,                sizeof(vec3)                  * mesh->normals_count,  file);
        platform->readFromFile(mesh->vertex_normal_indices,         sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }

    platform->closeFile(file);
}

void saveMeshToFile(Mesh *mesh, char* file_path, Platform *platform) {
    void *file = platform->openFileForWriting(file_path);

    platform->writeToFile(&mesh->aabb,           sizeof(AABB), file);
    platform->writeToFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->writeToFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->writeToFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->writeToFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->writeToFile(&mesh->normals_count,  sizeof(u32),  file);
    platform->writeToFile(mesh->vertex_positions,        sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->writeToFile(mesh->vertex_position_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->writeToFile(mesh->edge_vertex_indices,     sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
    if (mesh->uvs_count) {
        platform->writeToFile(mesh->vertex_uvs,          sizeof(vec2)                  * mesh->uvs_count,      file);
        platform->writeToFile(mesh->vertex_uvs_indices,  sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }
    if (mesh->normals_count) {
        platform->writeToFile(mesh->vertex_normals,        sizeof(vec3)                  * mesh->normals_count,  file);
        platform->writeToFile(mesh->vertex_normal_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }

    platform->closeFile(file);
}

void writeSceneSettingsToFile(SceneSettings *settings, void *file, Platform *platform) {
    platform->writeToFile(&settings->boxes, sizeof(u32), file);
    platform->writeToFile(&settings->cameras, sizeof(u32), file);
    platform->writeToFile(&settings->curves, sizeof(u32), file);
    platform->writeToFile(&settings->grids, sizeof(u32), file);
    platform->writeToFile(&settings->meshes, sizeof(u32), file);
    platform->writeToFile(&settings->primitives, sizeof(u32), file);
}

void readSceneSettingsFromFile(SceneSettings *settings, void *file, Platform *platform) {
    platform->readFromFile(&settings->boxes, sizeof(u32), file);
    platform->readFromFile(&settings->cameras, sizeof(u32), file);
    platform->readFromFile(&settings->curves, sizeof(u32), file);
    platform->readFromFile(&settings->grids, sizeof(u32), file);
    platform->readFromFile(&settings->meshes, sizeof(u32), file);
    platform->readFromFile(&settings->primitives, sizeof(u32), file);
}

void writeTransformToFile(xform3 *xform, void *file, Platform *platform) {
    platform->writeToFile(&xform->matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->yaw_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->pitch_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->roll_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->rotation_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->rotation_matrix_inverted, sizeof(mat3), file);

    platform->writeToFile(&xform->rotation, sizeof(quat), file);
    platform->writeToFile(&xform->rotation_inverted, sizeof(quat), file);

    platform->writeToFile(&xform->position, sizeof(vec3), file);
    platform->writeToFile(&xform->scale, sizeof(vec3), file);
}

void readTransformFromFile(xform3 *xform, void *file, Platform *platform) {
    platform->readFromFile(&xform->matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->yaw_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->pitch_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->roll_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->rotation_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->rotation_matrix_inverted, sizeof(mat3), file);

    platform->readFromFile(&xform->rotation, sizeof(quat), file);
    platform->readFromFile(&xform->rotation_inverted, sizeof(quat), file);

    platform->readFromFile(&xform->position, sizeof(vec3), file);
    platform->readFromFile(&xform->scale, sizeof(vec3), file);

    xform->right_direction   = &xform->rotation_matrix.X;
    xform->up_direction      = &xform->rotation_matrix.Y;
    xform->forward_direction = &xform->rotation_matrix.Z;
}

void loadSceneFromFile(Scene *scene, char* file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    readSceneSettingsFromFile(&scene->settings, file, platform);

    if (scene->cameras) {
        Camera *camera = scene->cameras;
        for (u32 i = 0; i < scene->settings.cameras; i++, camera++) {
            platform->readFromFile(&camera->focal_length, sizeof(f32), file);
            platform->readFromFile(&camera->zoom, sizeof(f32), file);
            platform->readFromFile(&camera->dolly, sizeof(f32), file);
            platform->readFromFile(&camera->target_distance, sizeof(f32), file);
            platform->readFromFile(&camera->current_velocity, sizeof(vec3), file);
            readTransformFromFile(&camera->transform, file, platform);
        }
    }

    if (scene->primitives)
        for (u32 i = 0; i < scene->settings.primitives; i++)
            platform->readFromFile(scene->primitives + i, sizeof(Primitive), file);

    if (scene->grids)
        for (u32 i = 0; i < scene->settings.grids; i++)
            platform->readFromFile(scene->grids + i, sizeof(Grid), file);

    if (scene->boxes)
        for (u32 i = 0; i < scene->settings.boxes; i++)
            platform->readFromFile(scene->boxes + i, sizeof(Box), file);

    if (scene->curves)
        for (u32 i = 0; i < scene->settings.curves; i++)
            platform->readFromFile(scene->curves + i, sizeof(Curve), file);

    if (scene->meshes) {
        Mesh *mesh = scene->meshes;
        for (u32 i = 0; i < scene->settings.meshes; i++, mesh++) {
            platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
            platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
            platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
            platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
            platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
            platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

            platform->readFromFile(mesh->vertex_positions,             sizeof(vec3)                  * mesh->vertex_count,   file);
            platform->readFromFile(mesh->vertex_position_indices,      sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            platform->readFromFile(mesh->edge_vertex_indices,          sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
            if (mesh->uvs_count) {
                platform->readFromFile(mesh->vertex_uvs,               sizeof(vec2)                  * mesh->uvs_count,      file);
                platform->readFromFile(mesh->vertex_uvs_indices,       sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
            if (mesh->normals_count) {
                platform->readFromFile(mesh->vertex_normals,                sizeof(vec3)                  * mesh->normals_count,  file);
                platform->readFromFile(mesh->vertex_normal_indices,         sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
        };
    }

    platform->closeFile(file);
}

void saveSceneToFile(Scene *scene, char* file_path, Platform *platform) {
    void *file = platform->openFileForWriting(file_path);

    writeSceneSettingsToFile(&scene->settings, file, platform);

    if (scene->cameras) {
        Camera *camera = scene->cameras;
        for (u32 i = 0; i < scene->settings.cameras; i++, camera++) {
            platform->writeToFile(&camera->focal_length, sizeof(f32), file);
            platform->writeToFile(&camera->zoom, sizeof(f32), file);
            platform->writeToFile(&camera->dolly, sizeof(f32), file);
            platform->writeToFile(&camera->target_distance, sizeof(f32), file);
            platform->writeToFile(&camera->current_velocity, sizeof(vec3), file);
            writeTransformToFile(&camera->transform, file, platform);
        }
    }

    if (scene->primitives)
        for (u32 i = 0; i < scene->settings.primitives; i++)
            platform->writeToFile(scene->primitives + i, sizeof(Primitive), file);

    if (scene->grids)
        for (u32 i = 0; i < scene->settings.grids; i++)
            platform->writeToFile(scene->grids + i, sizeof(Grid), file);

    if (scene->boxes)
        for (u32 i = 0; i < scene->settings.boxes; i++)
            platform->writeToFile(scene->boxes + i, sizeof(Box), file);

    if (scene->curves)
        for (u32 i = 0; i < scene->settings.curves; i++)
            platform->writeToFile(scene->curves + i, sizeof(Curve), file);

    if (scene->meshes) {
        Mesh *mesh = scene->meshes;
        for (u32 i = 0; i < scene->settings.meshes; i++, mesh++) {
            platform->writeToFile(&mesh->aabb,           sizeof(AABB), file);
            platform->writeToFile(&mesh->vertex_count,   sizeof(u32),  file);
            platform->writeToFile(&mesh->triangle_count, sizeof(u32),  file);
            platform->writeToFile(&mesh->edge_count,     sizeof(u32),  file);
            platform->writeToFile(&mesh->uvs_count,      sizeof(u32),  file);
            platform->writeToFile(&mesh->normals_count,  sizeof(u32),  file);

            platform->writeToFile(mesh->vertex_positions,        sizeof(vec3)                  * mesh->vertex_count,   file);
            platform->writeToFile(mesh->vertex_position_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            platform->writeToFile(mesh->edge_vertex_indices,     sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
            if (mesh->uvs_count) {
                platform->writeToFile(mesh->vertex_uvs,          sizeof(vec2)                  * mesh->uvs_count,      file);
                platform->writeToFile(mesh->vertex_uvs_indices,  sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
            if (mesh->normals_count) {
                platform->writeToFile(mesh->vertex_normals,        sizeof(vec3)                  * mesh->normals_count,  file);
                platform->writeToFile(mesh->vertex_normal_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
        }
    }

    platform->closeFile(file);
}



void transformBoxVerticesFromObjectToViewSpace(BoxVertices *vertices, BoxVertices *transformed_vertices, Primitive *primitive, Viewport *viewport) {
    vec3 position;
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        position = vertices->buffer[i];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(    position, viewport->camera->transform.position);
        position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
        transformed_vertices->buffer[i] = position;
    }
}

void drawBox(Box *box, u8 sides, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    static BoxVertices vertices;
    transformBoxVerticesFromObjectToViewSpace(&box->vertices, &vertices, primitive, viewport);

    // Distribute transformed vertices positions to edges:
    static BoxEdges edges;
    setBoxEdgesFromVertices(&edges, &vertices);

    if (sides == BOX__ALL_SIDES) {
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++)
            drawEdge(edges.buffer + i, color, opacity, line_width, viewport);
    } else {
        if (sides & Front | sides & Top   ) drawEdge(&edges.sides.front_top,    color, opacity, line_width, viewport);
        if (sides & Front | sides & Bottom) drawEdge(&edges.sides.front_bottom, color, opacity, line_width, viewport);
        if (sides & Front | sides & Left  ) drawEdge(&edges.sides.front_left,   color, opacity, line_width, viewport);
        if (sides & Front | sides & Right ) drawEdge(&edges.sides.front_right,  color, opacity, line_width, viewport);
        if (sides & Back  | sides & Top   ) drawEdge(&edges.sides.back_top,     color, opacity, line_width, viewport);
        if (sides & Back  | sides & Bottom) drawEdge(&edges.sides.back_bottom,  color, opacity, line_width, viewport);
        if (sides & Back  | sides & Left  ) drawEdge(&edges.sides.back_left,    color, opacity, line_width, viewport);
        if (sides & Back  | sides & Right ) drawEdge(&edges.sides.back_right,   color, opacity, line_width, viewport);
        if (sides & Left  | sides & Top   ) drawEdge(&edges.sides.left_top,     color, opacity, line_width, viewport);
        if (sides & Left  | sides & Bottom) drawEdge(&edges.sides.left_bottom,  color, opacity, line_width, viewport);
        if (sides & Right | sides & Top   ) drawEdge(&edges.sides.right_top,    color, opacity, line_width, viewport);
        if (sides & Right | sides & Bottom) drawEdge(&edges.sides.right_bottom, color, opacity, line_width, viewport);
    }
}

void drawCamera(Camera *camera, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    static Box box;
    static Primitive primitive;
    initBox(&box);
    primitive.flags = ALL_FLAGS;
    primitive.rotation = camera->transform.rotation;
    primitive.position = camera->transform.position;
    primitive.scale.x  = primitive.scale.y = primitive.scale.z = 1;
    drawBox(&box, BOX__ALL_SIDES, &primitive, color, opacity, line_width, viewport);
    box.vertices.corners.back_bottom_left   = scaleVec3(box.vertices.corners.back_bottom_left,  0.5f);
    box.vertices.corners.back_bottom_right  = scaleVec3(box.vertices.corners.back_bottom_right, 0.5f);
    box.vertices.corners.back_top_left      = scaleVec3(box.vertices.corners.back_top_left,     0.5f);
    box.vertices.corners.back_top_right     = scaleVec3(box.vertices.corners.back_top_right,    0.5f);
    box.vertices.corners.front_bottom_left  = scaleVec3(box.vertices.corners.front_bottom_left,  2);
    box.vertices.corners.front_bottom_right = scaleVec3(box.vertices.corners.front_bottom_right, 2);
    box.vertices.corners.front_top_left     = scaleVec3(box.vertices.corners.front_top_left,     2);
    box.vertices.corners.front_top_right    = scaleVec3(box.vertices.corners.front_top_right,    2);
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        box.vertices.buffer[i].z += 1.5f;
    drawBox(&box, BOX__ALL_SIDES, &primitive, color, opacity, line_width, viewport);
}



#define CURVE_STEPS 3600

void drawCurve(Curve *curve, u32 step_count, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    f32 one_over_step_count = 1.0f / (f32)step_count;
    f32 rotation_step = one_over_step_count * TAU;
    f32 rotation_step_times_rev_count = rotation_step * (f32)curve->revolution_count;

    if (primitive->type == PrimitiveType_Helix)
        rotation_step = rotation_step_times_rev_count;

    vec3 center_to_orbit;
    center_to_orbit.x = 1;
    center_to_orbit.y = center_to_orbit.z = 0;

    vec3 orbit_to_curve;
    orbit_to_curve.x = curve->thickness;
    orbit_to_curve.y = orbit_to_curve.z = 0;

    mat3 rotation;
    rotation.X.x = rotation.Z.z = cosf(rotation_step);
    rotation.X.z = sinf(rotation_step);
    rotation.Z.x = -rotation.X.z;
    rotation.X.y = rotation.Z.y = rotation.Y.x = rotation.Y.z =  0;
    rotation.Y.y = 1;

    mat3 orbit_to_curve_rotation;
    if (primitive->type == PrimitiveType_Coil) {
        orbit_to_curve_rotation.X.x = orbit_to_curve_rotation.Y.y = cosf(rotation_step_times_rev_count);
        orbit_to_curve_rotation.X.y = sinf(rotation_step_times_rev_count);
        orbit_to_curve_rotation.Y.x = -orbit_to_curve_rotation.X.y;
        orbit_to_curve_rotation.X.z = orbit_to_curve_rotation.Y.z = orbit_to_curve_rotation.Z.x = orbit_to_curve_rotation.Z.y =  0;
        orbit_to_curve_rotation.Z.z = 1;
    }

    // Transform vertices positions of edges from view-space to screen-space (w/ culling and clipping):
    mat3 accumulated_orbit_rotation = rotation;
    vec3 current_position, previous_position;
    Edge edge;

    for (u32 i = 0; i < step_count; i++) {
        center_to_orbit = mulVec3Mat3(center_to_orbit, rotation);

        switch (primitive->type) {
            case PrimitiveType_Helix:
                current_position = center_to_orbit;
                current_position.y -= 1;
                break;
            case PrimitiveType_Coil:
                orbit_to_curve  = mulVec3Mat3(orbit_to_curve, orbit_to_curve_rotation);
                current_position = mulVec3Mat3(orbit_to_curve, accumulated_orbit_rotation);
                current_position = addVec3(center_to_orbit, current_position);
                break;
            default:
                break;
        }

        current_position = convertPositionToWorldSpace(current_position, primitive);
        current_position = subVec3(    current_position, viewport->camera->transform.position);
        current_position = mulVec3Quat(current_position, viewport->camera->transform.rotation_inverted);

        if (i) {
            edge.from = previous_position;
            edge.to   = current_position;
            drawEdge(&edge, color, opacity, line_width, viewport);
        }

        switch (primitive->type) {
            case PrimitiveType_Helix:
                center_to_orbit.y += 2 * one_over_step_count;
                break;
            case PrimitiveType_Coil:
                accumulated_orbit_rotation = mulMat3(accumulated_orbit_rotation, rotation);
                break;
            default:
                break;
        }

        previous_position = current_position;
    }
}



void transformGridVerticesFromObjectToViewSpace(Grid *grid, GridVertices *transformed_vertices, Primitive *primitive, Viewport *viewport) {
    vec3 position;
    for (u8 side = 0; side < 2; side++) {
        for (u8 axis = 0; axis < 2; axis++) {
            u8 segment_count = axis ? grid->v_segments : grid->u_segments;
            for (u8 segment = 0; segment < segment_count; segment++) {
                position = grid->vertices.buffer[axis][side][segment];
                position = convertPositionToWorldSpace(position, primitive);
                position = subVec3(    position, viewport->camera->transform.position);
                position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
                transformed_vertices->buffer[axis][side][segment] = position;
            }
        }
    }
}

void drawGrid(Grid *grid, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    static GridVertices vertices;

    transformGridVerticesFromObjectToViewSpace(grid, &vertices, primitive, viewport);

    // Distribute transformed vertices positions to edges:
    static GridEdges edges;
    setGridEdgesFromVertices(edges.uv.u, grid->u_segments, vertices.uv.u.from, vertices.uv.u.to);
    setGridEdgesFromVertices(edges.uv.v, grid->v_segments, vertices.uv.v.from, vertices.uv.v.to);

    for (u8 u = 0; u < grid->u_segments; u++) drawEdge(edges.uv.u + u, color, opacity, line_width, viewport);
    for (u8 v = 0; v < grid->v_segments; v++) drawEdge(edges.uv.v + v, color, opacity, line_width, viewport);
}



void drawMesh(Mesh *mesh, bool draw_normals, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    EdgeVertexIndices *edge_vertex_indices = mesh->edge_vertex_indices;
    quat cam_rot = viewport->camera->transform.rotation_inverted;
    vec3 cam_pos = viewport->camera->transform.position;
    vec3 *positions = mesh->vertex_positions;
    vec3 position;
    Edge edge;
    for (u32 i = 0; i < mesh->edge_count; i++, edge_vertex_indices++) {
        position = positions[edge_vertex_indices->from];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.from = position;

        position = positions[edge_vertex_indices->to];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.to  = position;

        drawEdge(&edge, color, opacity, line_width, viewport);
    }

    if (draw_normals && mesh->normals_count && mesh->vertex_normals && mesh->vertex_normal_indices) {
        TriangleVertexIndices *vertex_normal_indices = mesh->vertex_normal_indices;
        TriangleVertexIndices *vertex_position_indices = mesh->vertex_position_indices;
        vec3 *normals = mesh->vertex_normals;
        for (u32 t = 0; t < mesh->triangle_count; t++, vertex_normal_indices++, vertex_position_indices++) {
            for (u8 i = 0; i < 3; i++) {
                position = positions[vertex_position_indices->ids[i]];
                edge.from = convertPositionToWorldSpace(position, primitive);
                edge.from = subVec3(edge.from,     cam_pos);
                edge.from = mulVec3Quat(edge.from, cam_rot);

                edge.to = addVec3(position, scaleVec3(normals[vertex_normal_indices->ids[i]], 0.1f));
                edge.to = convertPositionToWorldSpace(edge.to, primitive);
                edge.to = subVec3(edge.to,     cam_pos);
                edge.to = mulVec3Quat(edge.to, cam_rot);

                drawEdge(&edge, Color(Red), opacity * 0.5f, line_width, viewport);
            }
        }
    }
}


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

App *app;

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



void zoomCamera(Camera *camera, f32 zoom) {
    f32 new_zoom = camera->zoom + zoom;
    camera->focal_length = new_zoom > 1 ? new_zoom : (new_zoom < -1 ? (-1 / new_zoom) : 1);
    camera->zoom = new_zoom;
}

void dollyCamera(Camera *camera, f32 dolly) {
    vec3 target_position = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    target_position = addVec3(camera->transform.position, target_position);

    // Compute new target distance:
    camera->dolly += dolly;
    camera->target_distance = powf(2, camera->dolly / -200) * CAMERA_DEFAULT__TARGET_DISTANCE;

    // Back-track from target position to new current position:
    camera->transform.position = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = subVec3(target_position,camera->transform.position);
}

void orbitCamera(Camera *camera, f32 azimuth, f32 altitude) {
    // Move the camera forward to the position of it's target:
    vec3 movement = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = addVec3(camera->transform.position, movement);

    // Reorient the camera while it is at the position of it's target:
    rotateXform3(&camera->transform, azimuth, altitude, 0);

    // Back the camera away from it's target position using the updated forward direction:
    movement = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = subVec3(camera->transform.position, movement);
}

void panCamera(Camera *camera, f32 right, f32 up) {
    vec3 up_movement    = scaleVec3(*camera->transform.up_direction, up);
    vec3 right_movement = scaleVec3(*camera->transform.right_direction, right);
    camera->transform.position = addVec3(camera->transform.position, up_movement);
    camera->transform.position = addVec3(camera->transform.position, right_movement);
}

void panViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.pan * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.pan * +(f32)mouse->pos_raw_diff.y;
    panCamera(viewport->camera, x, y);

    viewport->navigation.moved = true;
    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void zoomViewport(Viewport *viewport, Mouse *mouse) {
    f32 z = viewport->navigation.settings.speeds.zoom * mouse->wheel_scroll_amount;
    zoomCamera(viewport->camera, z);

    viewport->navigation.zoomed = true;
    mouse->wheel_scroll_handled = true;
}

void dollyViewport(Viewport *viewport, Mouse *mouse) {
    f32 z = viewport->navigation.settings.speeds.dolly * mouse->wheel_scroll_amount;
    dollyCamera(viewport->camera, z);
    viewport->navigation.moved = true;
    mouse->wheel_scroll_handled = true;
}

void orientViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.orient * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.orient * -(f32)mouse->pos_raw_diff.y;
    rotateXform3(&viewport->camera->transform, x, y, 0);

    mouse->moved = false;
    mouse->raw_movement_handled = true;
    viewport->navigation.turned = true;
}

void orbitViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.orbit * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.orbit * -(f32)mouse->pos_raw_diff.y;
    orbitCamera(viewport->camera, x, y);

    viewport->navigation.turned = true;
    viewport->navigation.moved = true;
    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void navigateViewport(Viewport *viewport, f32 delta_time) {
    vec3 target_velocity = getVec3Of(0);
    Navigation *navigation = &viewport->navigation;
    Camera *camera = viewport->camera;

    if (navigation->move.right)    target_velocity.x += navigation->settings.max_velocity;
    if (navigation->move.left)     target_velocity.x -= navigation->settings.max_velocity;
    if (navigation->move.up)       target_velocity.y += navigation->settings.max_velocity;
    if (navigation->move.down)     target_velocity.y -= navigation->settings.max_velocity;
    if (navigation->move.forward)  target_velocity.z += navigation->settings.max_velocity;
    if (navigation->move.backward) target_velocity.z -= navigation->settings.max_velocity;
    if (navigation->turn.left) {
        rotateXform3(&camera->transform, delta_time * +navigation->settings.speeds.turn, 0, 0);
        navigation->turned = true;
    }
    if (navigation->turn.right) {
        rotateXform3(&camera->transform, delta_time * -navigation->settings.speeds.turn, 0, 0);
        navigation->turned = true;
    }

    // Update the current velocity:
    f32 velocity_difference = navigation->settings.acceleration * delta_time;
    camera->current_velocity = approachVec3(camera->current_velocity, target_velocity, velocity_difference);

    navigation->moved = nonZeroVec3(camera->current_velocity);
    if (navigation->moved) { // Update the current position:
        vec3 movement = scaleVec3(camera->current_velocity, delta_time);
        movement = mulVec3Mat3(movement, camera->transform.rotation_matrix);
        camera->transform.position = addVec3(camera->transform.position, movement);
    }
}



void setViewportProjectionPlane(Viewport *viewport) {
    Camera *camera = viewport->camera;
    ProjectionPlane *projection_plane = &viewport->projection_plane;
    Dimensions *dimensions = &viewport->dimensions;

    projection_plane->start = scaleVec3(*camera->transform.forward_direction, dimensions->f_height * camera->focal_length);
    projection_plane->right = scaleVec3(*camera->transform.right_direction, 1.0f - (f32)dimensions->width);
    projection_plane->down  = scaleVec3(*camera->transform.up_direction, dimensions->f_height - 1.0f);
    projection_plane->start = addVec3(projection_plane->start, projection_plane->right);
    projection_plane->start = addVec3(projection_plane->start, projection_plane->down);

    projection_plane->right = scaleVec3(*camera->transform.right_direction, 2);
    projection_plane->down  = scaleVec3(*camera->transform.up_direction, -2);
}

INLINE void setRayFromCoords(Ray *ray, vec2i coords, Viewport *viewport) {
    ray->origin = viewport->camera->transform.position;
    ray->direction = scaleAddVec3(viewport->projection_plane.right, (f32)coords.x, viewport->projection_plane.start);
    ray->direction = scaleAddVec3(viewport->projection_plane.down,  (f32)coords.y, ray->direction);
    ray->direction = normVec3(ray->direction);
}

INLINE bool hitPlane(vec3 plane_origin, vec3 plane_normal, vec3 *ray_origin, vec3 *ray_direction, RayHit *hit) {
    f32 Rd_dot_n = dotVec3(*ray_direction, plane_normal);
    if (Rd_dot_n == 0) // The ray is parallel to the plane
        return false;

    bool ray_is_facing_the_plane = Rd_dot_n < 0;

    vec3 RtoP = subVec3(*ray_origin, plane_origin);
    f32 Po_to_Ro_dot_n = dotVec3(RtoP, plane_normal);
    hit->from_behind = Po_to_Ro_dot_n < 0;
    if (hit->from_behind == ray_is_facing_the_plane) // The ray can't hit the plane
        return false;

    hit->distance = fabsf(Po_to_Ro_dot_n / Rd_dot_n);
    hit->position = scaleVec3(*ray_direction, hit->distance);
    hit->position = addVec3(*ray_origin,      hit->position);
    hit->normal   = plane_normal;

    return true;
}

INLINE BoxSide getBoxSide(vec3 octant, u8 axis) {
    switch (axis) {
        case 0 : return octant.x > 0 ? Right : Left;
        case 3 : return octant.x > 0 ? Left : Right;
        case 1 : return octant.y > 0 ? Top : Bottom;
        case 4 : return octant.y > 0 ? Bottom : Top;
        case 2 : return octant.z > 0 ? Front : Back;
        default: return octant.z > 0 ? Back : Front;
    }
}

INLINE BoxSide hitCube(RayHit *hit, vec3 *Ro, vec3 *Rd) {
    vec3 octant, RD_rcp = oneOverVec3(*Rd);
    octant.x = signbit(Rd->x) ? 1.0f : -1.0f;
    octant.y = signbit(Rd->y) ? 1.0f : -1.0f;
    octant.z = signbit(Rd->z) ? 1.0f : -1.0f;

    f32 t[6];
    t[0] = ( octant.x - Ro->x) * RD_rcp.x;
    t[1] = ( octant.y - Ro->y) * RD_rcp.y;
    t[2] = ( octant.z - Ro->z) * RD_rcp.z;
    t[3] = (-octant.x - Ro->x) * RD_rcp.x;
    t[4] = (-octant.y - Ro->y) * RD_rcp.y;
    t[5] = (-octant.z - Ro->z) * RD_rcp.z;

    u8 max_axis = t[3] < t[4] ? 3 : 4; if (t[5] < t[max_axis]) max_axis = 5;
    f32 maxt = t[max_axis];
    if (maxt < 0) // Further-away hit is behind the ray - intersection can not occur.
        return NoSide;

    u8 min_axis = t[0] > t[1] ? 0 : 1; if (t[2] > t[min_axis]) min_axis = 2;
    f32 mint = t[min_axis];
    if (maxt < (mint > 0 ? mint : 0))
        return NoSide;

    hit->from_behind = mint < 0; // Further-away hit is in front of the ray, closer one is behind it.
    if (hit->from_behind) {
        mint = maxt;
        min_axis = max_axis;
    }

    BoxSide side = getBoxSide(octant, min_axis);
    hit->position = scaleAddVec3(*Rd, mint, *Ro);
    hit->normal = getVec3Of(0);
    switch (side) {
        case Left:   hit->normal.x = hit->from_behind ? +1.0f : -1.0f; break;
        case Right:  hit->normal.x = hit->from_behind ? -1.0f : +1.0f; break;
        case Bottom: hit->normal.y = hit->from_behind ? +1.0f : -1.0f; break;
        case Top:    hit->normal.y = hit->from_behind ? -1.0f : +1.0f; break;
        case Back:   hit->normal.z = hit->from_behind ? +1.0f : -1.0f; break;
        case Front:  hit->normal.z = hit->from_behind ? -1.0f : +1.0f; break;
        default: return NoSide;
    }

    return side;
}

INLINE bool rayHitScene(Ray *ray, RayHit *local_hit, RayHit *hit, Scene *scene) {
    bool current_found, found = false;
    vec3 Ro, Rd;
    Primitive hit_primitive, primitive;
    for (u32 i = 0; i < scene->settings.primitives; i++) {
        primitive = scene->primitives[i];
        if (primitive.type == PrimitiveType_Mesh)
            primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

        convertPositionAndDirectionToObjectSpace(ray->origin, ray->direction, &primitive, &Ro, &Rd);

        current_found = hitCube(local_hit, &Ro, &Rd);
        if (current_found) {
            local_hit->position       = convertPositionToWorldSpace(local_hit->position, &primitive);
            local_hit->distance_squared = squaredLengthVec3(subVec3(local_hit->position, ray->origin));
            if (local_hit->distance_squared < hit->distance_squared) {
                *hit = *local_hit;
                hit->object_type = primitive.type;
                hit->object_id = i;

                hit_primitive = primitive;
                found = true;
            }
        }
    }

    if (found) {
        hit->distance = sqrtf(hit->distance_squared);
        hit->normal = normVec3(convertDirectionToWorldSpace(hit->normal, &hit_primitive));
    }

    return found;
}

void manipulateSelection(Scene *scene, Viewport *viewport, Controls *controls) {
    Mouse *mouse = &controls->mouse;
    Camera *camera = viewport->camera;
    Dimensions *dimensions = &viewport->dimensions;
    Selection *selection = scene->selection;

    setProjectionMatrix(viewport);
    setViewportProjectionPlane(viewport);

    vec3 position;
    vec3 *cam_pos = &camera->transform.position;
    mat3 *rot     = &camera->transform.rotation_matrix;
    mat3 *inv_rot = &camera->transform.rotation_matrix_inverted;
    RayHit *hit = &selection->hit;
    Ray ray, *local_ray = &selection->local_ray;
    Primitive primitive;
    vec2i mouse_pos = Vec2i(mouse->pos.x - viewport->position.x,
                            mouse->pos.y - viewport->position.y);

    if (mouse->left_button.is_pressed) {
        if (!mouse->left_button.is_handled) { // This is the first frame after the left mouse button went down:
            mouse->left_button.is_handled = true;

            // Cast a ray onto the scene to find the closest object behind the hovered pixel:
            setRayFromCoords(&ray, mouse_pos, viewport);

            hit->distance_squared = INFINITY;
            if (rayHitScene(&ray, &selection->local_hit, hit, scene)) {
                // Detect if object scene->selection has changed:
                selection->changed = (
                        selection->object_type != hit->object_type ||
                        selection->object_id   != hit->object_id
                );

                // Track the object that is now selected:
                selection->object_type = hit->object_type;
                selection->object_id   = hit->object_id;
                selection->primitive   = null;

                // Capture a pointer to the selected object's position for later use in transformations:
                selection->primitive = scene->primitives + selection->object_id;
                selection->world_position = &selection->primitive->position;
                selection->transformation_plane_origin = hit->position;

                selection->world_offset = subVec3(hit->position, *selection->world_position);

                // Track how far away the hit position is from the camera along the z axis:
                position = mulVec3Mat3(subVec3(hit->position, ray.origin), *inv_rot);
                selection->object_distance = position.z;
            } else {
                if (selection->object_type)
                    selection->changed = true;
                selection->object_type = 0;
            }
        }
    }

    if (selection->object_type) {
        if (controls->is_pressed.alt) {
            bool any_mouse_button_is_pressed = (
                    mouse->left_button.is_pressed ||
                    mouse->middle_button.is_pressed ||
                    mouse->right_button.is_pressed);
            if (selection->primitive && !any_mouse_button_is_pressed) {
                // Cast a ray onto the bounding box of the currently selected object:
                setRayFromCoords(&ray, mouse_pos, viewport);
                primitive = *selection->primitive;
                if (primitive.type == PrimitiveType_Mesh)
                    primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

                convertPositionAndDirectionToObjectSpace(ray.origin, ray.direction, &primitive, &local_ray->origin, &local_ray->direction);

                selection->box_side = hitCube(hit, &local_ray->origin, &local_ray->direction);
                if (selection->box_side) {
                    selection->transformation_plane_center = convertPositionToWorldSpace(hit->normal,   &primitive);
                    selection->transformation_plane_origin = convertPositionToWorldSpace(hit->position, &primitive);
                    selection->transformation_plane_normal = convertDirectionToWorldSpace(hit->normal,  &primitive);
                    selection->transformation_plane_normal = normVec3(selection->transformation_plane_normal);
                    selection->world_offset = subVec3(selection->transformation_plane_origin, *selection->world_position);
                    selection->object_scale    = selection->primitive->scale;
                    selection->object_rotation = selection->primitive->rotation;
                }
            }

            if (selection->box_side) {
                if (selection->primitive) {
                    if (any_mouse_button_is_pressed) {
                        setRayFromCoords(&ray, mouse_pos, viewport);
                        if (hitPlane(selection->transformation_plane_origin,
                                     selection->transformation_plane_normal,
                                     &ray.origin,
                                     &ray.direction,
                                     hit)) {

                            primitive = *selection->primitive;
                            if (primitive.type == PrimitiveType_Mesh)
                                primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);
                            if (mouse->left_button.is_pressed) {
                                position = subVec3(hit->position, selection->world_offset);
                                *selection->world_position = position;
                                if (selection->primitive)
                                    selection->primitive->flags |= IS_TRANSLATED;
                            } else if (mouse->middle_button.is_pressed) {
                                position      = selection->transformation_plane_origin;
                                position      = convertPositionToObjectSpace(     position, &primitive);
                                hit->position = convertPositionToObjectSpace(hit->position, &primitive);

                                selection->primitive->scale = mulVec3(selection->object_scale,
                                                                      mulVec3(hit->position, oneOverVec3(position)));
                                selection->primitive->flags |= IS_SCALED | IS_SCALED_NON_UNIFORMLY;
                            } else if (mouse->right_button.is_pressed) {
                                vec3 v1 = subVec3(hit->position,
                                                  selection->transformation_plane_center);
                                vec3 v2 = subVec3(selection->transformation_plane_origin,
                                                  selection->transformation_plane_center);
                                quat q;
                                q.axis = crossVec3(v2, v1);
                                q.amount = sqrtf(squaredLengthVec3(v1) * squaredLengthVec3(v2)) + dotVec3(v1, v2);
                                q = normQuat(q);
                                selection->primitive->rotation = mulQuat(q, selection->object_rotation);
                                selection->primitive->rotation = normQuat(selection->primitive->rotation);
                                selection->primitive->flags |= IS_ROTATED;
                            }
                        }
                    }
                }
            }
        } else {
            selection->box_side = NoSide;
            if (mouse->left_button.is_pressed && mouse->moved) {
                // Back-project the new mouse position onto a quad at a distance of the selected-object away from the camera
                position.z = selection->object_distance;

                // Screen -> NDC:
                position.x = ((f32)mouse_pos.x + 0.5f) / dimensions->h_width  - 1;
                position.y = ((f32)mouse_pos.y + 0.5f) / dimensions->h_height - 1;
                position.y = -position.y;

                // NDC -> View:
                position.x *= position.z / (camera->focal_length * dimensions->height_over_width);
                position.y *= position.z / camera->focal_length;

                // View -> World:
                position = addVec3(mulVec3Mat3(position, *rot), *cam_pos);

                // Back-track by the world offset (from the hit position back to the selected-object's center):
                position = subVec3(position, selection->world_offset);
                *selection->world_position = position;
                if (selection->primitive)
                    selection->primitive->flags |= IS_TRANSLATED;
            }
        }
    }
}

void drawSelection(Scene *scene, Viewport *viewport, Controls *controls) {
    Mouse *mouse = &controls->mouse;
    Selection *selection = scene->selection;
    Box *box = &selection->box;

    if (controls->is_pressed.alt &&
        !mouse->is_captured &&
        selection->object_type &&
        selection->primitive) {
        Primitive primitive = *selection->primitive;
        if (primitive.type == PrimitiveType_Mesh)
            primitive.scale = mulVec3(primitive.scale, scene->meshes[primitive.id].aabb.max);

        initBox(box);
        drawBox(box, BOX__ALL_SIDES, &primitive, Color(Yellow), 0.5f, 0, viewport);
        if (selection->box_side) {
            vec3 color = Color(White);
            switch (selection->box_side) {
                case Left:  case Right:  color = Color(Red);   break;
                case Top:   case Bottom: color = Color(Green); break;
                case Front: case Back:   color = Color(Blue);  break;
                case NoSide: break;
            }
            drawBox(box, selection->box_side, &primitive, color, 0.5f, 1, viewport);
        }
    }
}



void initApp(Defaults *defaults);

void _windowRedraw() {
    if (!app->is_running) return;
    if (app->on.windowRedraw) app->on.windowRedraw();
}

void _windowResize(u16 width, u16 height) {
    if (!app->is_running) return;
    updateDimensions(&app->viewport.dimensions, width, height, width);

    if (app->on.windowResize) app->on.windowResize(width, height);
    if (app->on.windowRedraw) app->on.windowRedraw();
}

void _keyChanged(u8 key, bool pressed) {
    if (key == app->controls.key_map.ctrl)  app->controls.is_pressed.ctrl  = pressed;
    else if (key == app->controls.key_map.alt)   app->controls.is_pressed.alt   = pressed;
    else if (key == app->controls.key_map.shift) app->controls.is_pressed.shift = pressed;
    else if (key == app->controls.key_map.space) app->controls.is_pressed.space = pressed;
    else if (key == app->controls.key_map.tab)   app->controls.is_pressed.tab   = pressed;

    if (app->on.keyChanged) app->on.keyChanged(key, pressed);
}

void _mouseButtonDown(MouseButton *mouse_button, i32 x, i32 y) {
    mouse_button->is_pressed = true;
    mouse_button->is_handled = false;

    mouse_button->down_pos.x = x;
    mouse_button->down_pos.y = y;

    if (app->on.mouseButtonDown) app->on.mouseButtonDown(mouse_button);
}

void _mouseButtonUp(MouseButton *mouse_button, i32 x, i32 y) {
    mouse_button->is_pressed = false;
    mouse_button->is_handled = false;

    mouse_button->up_pos.x = x;
    mouse_button->up_pos.y = y;

    if (app->on.mouseButtonUp) app->on.mouseButtonUp(mouse_button);
}

void _mouseButtonDoubleClicked(MouseButton *mouse_button, i32 x, i32 y) {
    app->controls.mouse.double_clicked = true;
    mouse_button->double_click_pos.x = x;
    mouse_button->double_click_pos.y = y;
    if (app->on.mouseButtonDoubleClicked) app->on.mouseButtonDoubleClicked(mouse_button);
}

void _mouseWheelScrolled(f32 amount) {
    app->controls.mouse.wheel_scroll_amount += amount * 100;
    app->controls.mouse.wheel_scrolled = true;

    if (app->on.mouseWheelScrolled) app->on.mouseWheelScrolled(amount);
}

void _mousePositionSet(i32 x, i32 y) {
    app->controls.mouse.pos.x = x;
    app->controls.mouse.pos.y = y;

    if (app->on.mousePositionSet) app->on.mousePositionSet(x, y);
}

void _mouseMovementSet(i32 x, i32 y) {
    app->controls.mouse.movement.x = x - app->controls.mouse.pos.x;
    app->controls.mouse.movement.y = y - app->controls.mouse.pos.y;
    app->controls.mouse.moved = true;

    if (app->on.mouseMovementSet) app->on.mouseMovementSet(x, y);
}

void _mouseRawMovementSet(i32 x, i32 y) {
    app->controls.mouse.pos_raw_diff.x += x;
    app->controls.mouse.pos_raw_diff.y += y;
    app->controls.mouse.moved = true;

    if (app->on.mouseRawMovementSet) app->on.mouseRawMovementSet(x, y);
}

bool initAppMemory(u64 size) {
    if (app->memory.address) return false;

    void* memory_address = app->platform.getMemory(size);
    if (!memory_address) {
        app->is_running = false;
        return false;
    }

    initMemory(&app->memory, (u8*)memory_address, size);
    return true;
}

void* allocateAppMemory(u64 size) {
    void *new_memory = allocateMemory(&app->memory, size);
    if (new_memory) return new_memory;

    app->is_running = false;
    return null;
}

void initMaterial(Material *material) {
    for (u8 i = 0; i < 3; i++) {
        material->specular.components[i] = 1;
        material->diffuse.components[i] = 1;
    }
    material->pixel_shader = shadePixelDepth;
    material->mesh_shader = shadeMesh;
    material->roughness  = 1;
    material->shininess  = 1;
    material->texture_count = 0;
    for (u8 i = 0; i < 16; i++) material->texture_ids[i] = 0;
}

void initRasterizer(Rasterizer *rasterizer, u32 max_vertex_count, u32 max_normal_count, Memory *memory) {
    rasterizer->vertex_flags = (u8*)allocateMemory(memory, max_vertex_count);
    rasterizer->clip_space_vertex_positions  = (vec4*)allocateMemory(memory, sizeof(vec4) * max_vertex_count);
    rasterizer->world_space_vertex_positions = (vec3*)allocateMemory(memory, sizeof(vec3) * max_vertex_count);
    rasterizer->world_space_vertex_normals = (vec3*)allocateMemory(memory, sizeof(vec3) * max_normal_count);
    setMeshToCube(&rasterizer->default_cube_mesh);
}

void initScene(Scene *scene, SceneSettings *settings, Memory *memory, Platform *platform) {
    scene->settings   = *settings;
    scene->primitives = null;
    scene->cameras    = null;
    scene->curves     = null;
    scene->boxes      = null;
    scene->grids      = null;
    scene->meshes     = null;
    scene->textures   = null;

    scene->selection = (Selection*)allocateMemory(memory, sizeof(Selection));
    scene->selection->object_type = scene->selection->object_id = 0;
    scene->selection->changed = false;

    if (settings->meshes && settings->mesh_files) {
        scene->meshes = (Mesh*)allocateMemory(memory, sizeof(Mesh) * settings->meshes);
        for (u32 i = 0; i < settings->meshes; i++)
            loadMeshFromFile(&scene->meshes[i], settings->mesh_files[i].char_ptr, platform, memory);
    }

    if (settings->cameras) {
        scene->cameras = (Camera*)allocateMemory(memory, sizeof(Camera) * settings->cameras);
        if (scene->cameras)
            for (u32 i = 0; i < settings->cameras; i++)
                initCamera(scene->cameras + i);
    }

    if (settings->primitives) {
        scene->primitives = (Primitive*)allocateMemory(memory, sizeof(Primitive) * settings->primitives);
        if (scene->primitives)
            for (u32 i = 0; i < settings->primitives; i++) {
                initPrimitive(scene->primitives + i);
                scene->primitives[i].id = i;
            }
    }

    if (settings->curves) {
        scene->curves = (Curve*)allocateMemory(memory, sizeof(Curve) * settings->curves);
        if (scene->curves)
            for (u32 i = 0; i < settings->curves; i++)
                initCurve(scene->curves + i);
    }

    if (settings->boxes) {
        scene->boxes = (Box*)allocateMemory(memory, sizeof(Box) * settings->boxes);
        if (scene->boxes)
            for (u32 i = 0; i < settings->boxes; i++)
                initBox(scene->boxes + i);
    }

    if (settings->grids) {
        scene->grids = (Grid*)allocateMemory(memory, sizeof(Grid) * settings->grids);
        if (scene->grids)
            for (u32 i = 0; i < settings->grids; i++)
                initGrid(scene->grids + i, 3, 3);
    }

    if (settings->lights) {
        Light *light = scene->lights = (Light*)allocateMemory(memory, sizeof(Light) * settings->lights);
        for (u32 i = 0; i < settings->lights; i++, light++) {
            for (u8 c = 0; c < 3; c++) {
                light->position_or_direction.components[c] = 0;
                light->color.components[c]                 = 1;
                light->attenuation.components[c]           = 1;
            }
            light->intensity = 1;
            light->is_directional = false;
        }
    }

    if (settings->materials)   {
        Material *material = scene->materials = (Material*)allocateMemory(memory, sizeof(Material) * settings->materials);
        for (u32 i = 0; i < settings->materials; i++, material++)
            initMaterial(material);
    }

    if (settings->textures && settings->texture_files) {
        scene->textures = (Texture*)allocateMemory(memory, sizeof(Texture) * settings->textures);
        for (u32 i = 0; i < settings->textures; i++)
            loadTextureFromFile(&scene->textures[i], settings->texture_files[i].char_ptr, platform, memory);
    }

    scene->last_io_ticks = 0;
    scene->last_io_is_save = false;
}


void _initApp(Defaults *defaults, u32* window_content) {
    app->window_content = window_content;

    app->is_running = true;
    app->user_data = null;
    app->memory.address = null;

    app->on.sceneReady = null;
    app->on.viewportReady = null;
    app->on.windowRedraw = null;
    app->on.keyChanged = null;
    app->on.mouseButtonUp = null;
    app->on.mouseButtonDown = null;
    app->on.mouseButtonDoubleClicked = null;
    app->on.mouseWheelScrolled = null;
    app->on.mousePositionSet = null;
    app->on.mouseMovementSet = null;
    app->on.mouseRawMovementSet = null;

    defaults->title = (char*)"";
    defaults->width = 480;
    defaults->height = 360;
    defaults->additional_memory_size = 0;

    SceneSettings *scene_settings = &defaults->settings.scene;
    ViewportSettings *viewport_settings = &defaults->settings.viewport;
    NavigationSettings *navigation_settings = &defaults->settings.navigation;

    setDefaultSceneSettings(scene_settings);
    setDefaultViewportSettings(viewport_settings);
    setDefaultNavigationSettings(navigation_settings);

    initTime(&app->time, app->platform.getTicks, app->platform.ticks_per_second);
    initMouse(&app->controls.mouse);
    initApp(defaults);

    u64 memory_size = sizeof(Selection) + defaults->additional_memory_size;
    memory_size += scene_settings->primitives * sizeof(Primitive);
    memory_size += scene_settings->textures   * sizeof(Texture);
    memory_size += scene_settings->meshes     * sizeof(Mesh);
    memory_size += scene_settings->curves     * sizeof(Curve);
    memory_size += scene_settings->boxes      * sizeof(Box);
    memory_size += scene_settings->grids      * sizeof(Grid);
    memory_size += scene_settings->cameras    * sizeof(Camera);
    memory_size += scene_settings->materials  * sizeof(Material);
    memory_size += scene_settings->lights     * sizeof(Light);
    memory_size += viewport_settings->hud_line_count * sizeof(HUDLine);

    if (scene_settings->textures && scene_settings->texture_files) {
        Texture texture;
        for (u32 i = 0; i < scene_settings->textures; i++)
            memory_size += getTextureMemorySize(&texture, scene_settings->texture_files[i].char_ptr, &app->platform);
    }

    u32 max_triangle_count = CUBE__TRIANGLE_COUNT;
    u32 max_vertex_count = CUBE__VERTEX_COUNT;
    u32 max_normal_count = CUBE__NORMAL_COUNT;
    if (scene_settings->meshes && scene_settings->mesh_files) {
        Mesh mesh;
        for (u32 i = 0; i < scene_settings->meshes; i++) {
            memory_size += getMeshMemorySize(&mesh, scene_settings->mesh_files[i].char_ptr, &app->platform);
            if (mesh.triangle_count > max_triangle_count) max_triangle_count = mesh.triangle_count;
            if (mesh.vertex_count > max_vertex_count) max_vertex_count = mesh.vertex_count;
            if (mesh.normals_count > max_normal_count) max_normal_count = mesh.normals_count;
        }
    }

    memory_size += max_vertex_count * (sizeof(vec3) + sizeof(vec4) + 1);
    memory_size += max_normal_count * sizeof(vec3);
    memory_size += FRAME_BUFFER_MEMORY_SIZE;

    initAppMemory(memory_size);

    PixelQuad *pixels = (PixelQuad*)allocateAppMemory(FRAME_BUFFER_MEMORY_SIZE);
    initRasterizer(&app->rasterizer, max_vertex_count, max_normal_count, &app->memory);
    initScene(&app->scene, scene_settings, &app->memory, &app->platform);
    if (app->on.sceneReady) app->on.sceneReady(&app->scene);

    if (viewport_settings->hud_line_count)
        viewport_settings->hud_lines = (HUDLine*)allocateAppMemory(viewport_settings->hud_line_count * sizeof(HUDLine));

    initViewport(&app->viewport, viewport_settings, navigation_settings, app->scene.cameras, pixels);
    if (app->on.viewportReady) app->on.viewportReady(&app->viewport);
}

#ifdef __linux__
//linux code goes here
#elif _WIN32


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef NDEBUG
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

void DisplayError(LPTSTR lpszFunction) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    unsigned int last_error = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            null, last_error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 0, null);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    if (FAILED( StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                                TEXT("%s failed with error code %d as follows:\n%s"), lpszFunction, last_error, lpMsgBuf)))
        printf("FATAL ERROR: Unable to output error code.\n");

    _tprintf(TEXT((LPTSTR)"ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
#endif

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

WNDCLASSA window_class;
HWND window;
HDC win_dc;
BITMAPINFO info;
RECT win_rect;
RAWINPUT raw_inputs;
HRAWINPUT raw_input_handle;
RAWINPUTDEVICE raw_input_device;
UINT raw_input_size;
PUINT raw_input_size_ptr = (PUINT)(&raw_input_size);
UINT raw_input_header_size = sizeof(RAWINPUTHEADER);

u64 Win32_ticksPerSecond;
LARGE_INTEGER performance_counter;

void Win32_setWindowTitle(char* str) { SetWindowTextA(window, str); }
void Win32_setCursorVisibility(bool on) { ShowCursor(on); }
void Win32_setWindowCapture(bool on) { if (on) SetCapture(window); else ReleaseCapture(); }
u64 Win32_getTicks() {
    QueryPerformanceCounter(&performance_counter);
    return (u64)performance_counter.QuadPart;
}
void* Win32_getMemory(u64 size) {
    return VirtualAlloc((LPVOID)MEMORY_BASE, (SIZE_T)size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

inline UINT getRawInput(LPVOID data) {
    return GetRawInputData(raw_input_handle, RID_INPUT, data, raw_input_size_ptr, raw_input_header_size);
}
inline bool hasRawInput() {
    return getRawInput(0) == 0 && raw_input_size != 0;
}
inline bool hasRawMouseInput(LPARAM lParam) {
    raw_input_handle = (HRAWINPUT)(lParam);
    return (
            hasRawInput() &&
            getRawInput((LPVOID)&raw_inputs) == raw_input_size &&
            raw_inputs.header.dwType == RIM_TYPEMOUSE
    );
}

void Win32_closeFile(void *handle) { CloseHandle(handle); }
void* Win32_openFileForReading(const char* path) {
    HANDLE handle = CreateFile(path,           // file to open
                               GENERIC_READ,          // open for reading
                               FILE_SHARE_READ,       // share for reading
                               null,                  // default security
                               OPEN_EXISTING,         // existing file only
                               FILE_ATTRIBUTE_NORMAL, // normal file
                               null);                 // no attr. template
#ifndef NDEBUG
    if (handle == INVALID_HANDLE_VALUE) {
        DisplayError(TEXT((LPTSTR)"CreateFile"));
        _tprintf(TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), path);
        return null;
    }
#endif
    return handle;
}
void* Win32_openFileForWriting(const char* path) {
    HANDLE handle = CreateFile(path,           // file to open
                               GENERIC_WRITE,          // open for writing
                               0,                      // do not share
                               null,                   // default security
                               OPEN_ALWAYS,            // create new or open existing
                               FILE_ATTRIBUTE_NORMAL,  // normal file
                               null);
#ifndef NDEBUG
    if (handle == INVALID_HANDLE_VALUE) {
        DisplayError(TEXT((LPTSTR)"CreateFile"));
        _tprintf(TEXT("Terminal failure: unable to open file \"%s\" for write.\n"), path);
        return null;
    }
#endif
    return handle;
}
bool Win32_readFromFile(LPVOID out, DWORD size, HANDLE handle) {
    DWORD bytes_read = 0;
    BOOL result = ReadFile(handle, out, size, &bytes_read, null);
#ifndef NDEBUG
    if (result == FALSE) {
        DisplayError(TEXT((LPTSTR)"ReadFile"));
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", (unsigned int)GetLastError());
        CloseHandle(handle);
    }
#endif
    return result != FALSE;
}

bool Win32_writeToFile(LPVOID out, DWORD size, HANDLE handle) {
    DWORD bytes_written = 0;
    BOOL result = WriteFile(handle, out, size, &bytes_written, null);
#ifndef NDEBUG
    if (result == FALSE) {
        DisplayError(TEXT((LPTSTR)"WriteFile"));
        printf("Terminal failure: Unable to write from file.\n GetLastError=%08x\n", (unsigned int)GetLastError());
        CloseHandle(handle);
    }
#endif
    return result != FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            app->is_running = false;
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            GetClientRect(window, &win_rect);

            info.bmiHeader.biWidth = win_rect.right - win_rect.left;
            info.bmiHeader.biHeight = win_rect.top - win_rect.bottom;

            _windowResize((u16)info.bmiHeader.biWidth, (u16)-info.bmiHeader.biHeight);

            break;

        case WM_PAINT:
            SetDIBitsToDevice(win_dc,
                              0, 0, app->viewport.dimensions.width, app->viewport.dimensions.height,
                              0, 0, 0, app->viewport.dimensions.height,
                              app->window_content, &info, DIB_RGB_COLORS);

            ValidateRgn(window, null);
            break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            _keyChanged((u8)wParam, true);
            break;

        case WM_SYSKEYUP:
        case WM_KEYUP: _keyChanged((u8)wParam, false); break;

        case WM_MBUTTONUP:     _mouseButtonUp(  &app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONUP  :   _mouseButtonUp(  &app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONUP:     _mouseButtonUp(  &app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MOUSEWHEEL:    _mouseWheelScrolled((f32)(GET_WHEEL_DELTA_WPARAM(wParam)) / (f32)(WHEEL_DELTA)); break;
        case WM_MOUSEMOVE:
            _mouseMovementSet(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            _mousePositionSet(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;

        case WM_INPUT:
            if ((hasRawMouseInput(lParam)) && (
                    raw_inputs.data.mouse.lLastX != 0 ||
                    raw_inputs.data.mouse.lLastY != 0))
                _mouseRawMovementSet(
                        raw_inputs.data.mouse.lLastX,
                        raw_inputs.data.mouse.lLastY
                );

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {

    void* app_memory = GlobalAlloc(GPTR, sizeof(App));
    if (!app_memory)
        return -1;

    app = (App*)app_memory;

    void* window_content_memory = GlobalAlloc(GPTR, sizeof(u32) * MAX_WIDTH * MAX_HEIGHT);
    if (!window_content_memory)
        return -1;

    LARGE_INTEGER performance_frequency;
    QueryPerformanceFrequency(&performance_frequency);
    Win32_ticksPerSecond = (u64)performance_frequency.QuadPart;

    app->controls.key_map.space = VK_SPACE;
    app->controls.key_map.shift = VK_SHIFT;
    app->controls.key_map.ctrl  = VK_CONTROL;
    app->controls.key_map.alt   = VK_MENU;
    app->controls.key_map.tab   = VK_TAB;

    app->platform.ticks_per_second    = Win32_ticksPerSecond;
    app->platform.getTicks            = Win32_getTicks;
    app->platform.getMemory           = Win32_getMemory;
    app->platform.setWindowTitle      = Win32_setWindowTitle;
    app->platform.setWindowCapture    = Win32_setWindowCapture;
    app->platform.setCursorVisibility = Win32_setCursorVisibility;
    app->platform.closeFile           = Win32_closeFile;
    app->platform.openFileForReading  = Win32_openFileForReading;
    app->platform.openFileForWriting  = Win32_openFileForWriting;
    app->platform.readFromFile        = Win32_readFromFile;
    app->platform.writeToFile         = Win32_writeToFile;

    Defaults defaults;
    _initApp(&defaults, (u32*)window_content_memory);

    info.bmiHeader.biSize        = sizeof(info.bmiHeader);
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biBitCount    = 32;
    info.bmiHeader.biPlanes      = 1;

    window_class.lpszClassName  = "RnDer";
    window_class.hInstance      = hInstance;
    window_class.lpfnWndProc    = WndProc;
    window_class.style          = CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    window_class.hCursor        = LoadCursorA(null, IDC_ARROW);

    if (!RegisterClassA(&window_class)) return -1;

    win_rect.top = 0;
    win_rect.left = 0;
    win_rect.right  = defaults.width;
    win_rect.bottom = defaults.height;
    AdjustWindowRect(&win_rect, WS_OVERLAPPEDWINDOW, false);

    window = CreateWindowA(
            window_class.lpszClassName,
            defaults.title,
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT,
            CW_USEDEFAULT,
            win_rect.right - win_rect.left,
            win_rect.bottom - win_rect.top,

            null,
            null,
            hInstance,
            null
    );
    if (!window)
        return -1;

    raw_input_device.usUsagePage = 0x01;
    raw_input_device.usUsage = 0x02;
    if (!RegisterRawInputDevices(&raw_input_device, 1, sizeof(raw_input_device)))
        return -1;

    win_dc = GetDC(window);

    SetICMMode(win_dc, ICM_OFF);



    ShowWindow(window, nCmdShow);

    MSG message;
    while (app->is_running) {
        while (PeekMessageA(&message, null, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        _windowRedraw();
        InvalidateRgn(window, null, false);
    }

    return 0;
}


#endif