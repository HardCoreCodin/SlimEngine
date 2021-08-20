#pragma once

#include "./projective_base.h"

#define MATRIX_MAX_CHARS_PER_ROW (4 * 5 + 2)
#define MATRIX_PADDING 5
#define MATRIX_MAX_ROW_WIDTH (FONT_WIDTH * MATRIX_MAX_CHARS_PER_ROW)
#define MATRIX_MAX_ROW_HEIGHT (1 + FONT_HEIGHT + 1)
#define MATRIX_MAX_WIDTH  (MATRIX_PADDING + MATRIX_MAX_ROW_WIDTH + MATRIX_PADDING)
#define MATRIX_MAX_HEIGHT (MATRIX_PADDING + (MATRIX_MAX_ROW_HEIGHT * 4) + MATRIX_PADDING)
#define MATRIX_MAX_SIZE (MATRIX_MAX_WIDTH * MATRIX_MAX_HEIGHT)

enum ColorID MATRIX_AXIS_COLORS[4] = {
        BrightRed,
        BrightGreen,
        BrightBlue,
        BrightMagenta
};
char* MATRIX_AXIS_LABELS[4] = {
        "X",
        "Y",
        "Z",
        "W"
};

#define MAX_MATRIX_COUNT 8

typedef struct Matrix {
    mat4 M;
    NumberString components[4][4];
    RGBA component_colors[4][4];
    u8 dim;
} Matrix;

Matrix main_matrix, matrices[MAX_MATRIX_COUNT];
u8 matrix_count = 0;

PixelGrid matrix_image;

void initMatrix(Matrix *matrix) {
    matrix->dim = 4;
    matrix->M = getMat4Identity();
    for (u8 col = 0; col < 4; col++)
        for (u8 row = 0; row < 4; row++) {
            initNumberString(&matrix->components[row][col]);
            matrix->component_colors[row][col] = Color(MATRIX_AXIS_COLORS[row]);
            copyToString(&matrix->components[row][col].string, MATRIX_AXIS_LABELS[col], 0);
        }
}

void setMatrixComponentColor(Matrix *matrix) {
        for (u8 row = 0; row < 4; row++)
            for (u8 column = 0; column < 4; column++)
                if (column == 1 || row == 1) {
                    matrix->component_colors[row][column] = Color(MATRIX_AXIS_COLORS[row]);
                    if (matrix->dim == 4) {
                        matrix->component_colors[row][column].R /= 4;
                        matrix->component_colors[row][column].G /= 4;
                        matrix->component_colors[row][column].B /= 4;
                    }
                }
}

u32 getMatrixRowLength(Matrix *matrix, u8 row_index) {
    u32 row_length = 0;
    for (u8 column_index = 0; column_index < matrix->dim; column_index++)
        row_length += matrix->components[row_index][column_index].string.length;

    return row_length;
}
u32 getMatrixColumnLength(Matrix *matrix, u8 column_index) {
    u32 current_column_length, matrix_column_length = 0;
    for (u8 row_index = 0; row_index < matrix->dim; row_index++) {
        current_column_length = matrix->components[row_index][column_index].string.length;
        if (matrix_column_length < current_column_length)
            matrix_column_length = current_column_length;
    }

    return matrix_column_length;
}

u32 getMatrixMaxRowLength(Matrix *matrix) {
    u32 current_row_length, max_row_length = 0;
    for (u8 row = 0; row < matrix->dim; row++) {
        current_row_length = getMatrixRowLength(matrix, row);
        if (max_row_length < current_row_length)
            max_row_length = current_row_length;
    }

    return max_row_length;
}
u32 getMatrixWidth(Matrix *matrix) {
    return MATRIX_PADDING + ((getMatrixMaxRowLength(matrix) + matrix->dim - 1) * FONT_WIDTH) + MATRIX_PADDING;
}

void updateMatrixStrings(Matrix *matrix) {
    static NumberString number_string;
    for (u8 row = 0; row < matrix->dim; row++) {
        for (u8 column = 0; column < matrix->dim; column++) {
            printFloatIntoString(matrix->M.axis[row].components[column], &number_string, 2);
            copyToString(&matrix->components[row][column].string, number_string.string.char_ptr, 0);
        }
    }
}

void drawMatrix(Matrix *matrix, i32 x, i32 y) {
    RGBA background_color = Color(Black);
    RGBA foreground_color = Color(White);

    fillPixelGrid(&matrix_image, background_color);

    i32 width = (i32)getMatrixWidth(matrix);
    i32 height = MATRIX_PADDING + (MATRIX_MAX_ROW_HEIGHT * matrix->dim) + MATRIX_PADDING;
    Rect rect;
    rect.min = Vec2i(0, 0);
    rect.max = Vec2i(width, height);
    drawRect(&matrix_image, foreground_color, &rect);
    rect.min.x++;
    rect.min.y++;
    rect.max.x--;
    rect.max.y--;
    drawRect(&matrix_image, foreground_color, &rect);
    rect.min.x--;
    rect.max.x++;
    rect.min.x += MATRIX_PADDING;
    rect.max.x -= MATRIX_PADDING;
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, 0);
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, 1);
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, height - 0);
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, height - 1);

    vec2i offset = Vec2i(MATRIX_PADDING, MATRIX_PADDING);
    for (u8 row_index = 0; row_index < matrix->dim; row_index++) {
        offset.x = MATRIX_PADDING;
        for (u8 column_index = 0; column_index < matrix->dim; column_index++) {
            drawText(&matrix_image,
                     matrix->component_colors[row_index][column_index],
                     matrix->components[row_index][column_index].string.char_ptr, offset.x, offset.y);
            offset.x += FONT_WIDTH * ((i32)getMatrixColumnLength(matrix, column_index) + 1);
        }
        offset.y += MATRIX_MAX_ROW_HEIGHT;
    }
    copyPixels(&matrix_image, app->viewport.frame_buffer, width, height, x, y);
}

void drawMatrixHUD(PixelGrid *canvas) {
    Rect rect;

    rect.min.x = rect.min.y = 0;
    rect.max.x = canvas->dimensions.width;
    rect.max.y = 140;
    fillRect(canvas, Color(Black), &rect);

    drawText(canvas, default_near_color, "Near Clipping Plane: N = ", 10, 10);
    drawText(canvas, default_far_color,  "Far  Clipping Plane: F = ", 10, 20);
    drawText(canvas, focal_length_color, "Focal Length : L = ", 10 + 250, 10);
    drawText(canvas, aspect_ratio_color, "Aspect Ratio : A = ", 10 + 250, 20);

    NumberString number;
    initNumberString(&number);

    printFloatIntoString(secondary_viewport.settings.near_clipping_plane_distance, &number, 2);
    drawText(canvas, default_near_color, number.string.char_ptr, 10 + 25*FONT_WIDTH, 10);

    printFloatIntoString(secondary_viewport.settings.far_clipping_plane_distance, &number, 2);
    drawText(canvas, default_far_color, number.string.char_ptr, 10 + 25*FONT_WIDTH, 20);

    printFloatIntoString(secondary_viewport.camera->focal_length, &number, 2);
    drawText(canvas, focal_length_color, number.string.char_ptr, 10 + 250 + 19*FONT_WIDTH, 10);

    printFloatIntoString(secondary_viewport_frame_buffer.dimensions.width_over_height, &number, 2);
    drawText(canvas, aspect_ratio_color, number.string.char_ptr, 10 + 250 + 19*FONT_WIDTH, 20);

    rect.min.x = canvas->dimensions.width - MATRIX_MAX_WIDTH;
    drawText(canvas, Color(White), "Transformation:", rect.min.x, 10);
    drawMatrix(&main_matrix,                                     rect.min.x, 20);

    drawText(canvas, Color(White), "P' = p * M", 10, 65);

    i32 x = 80;
    i32 y = 50;
    Matrix *matrix = matrices;
    for (u8 i = 0; i < matrix_count; i++, matrix++) {
        drawMatrix(matrix, x, y);
        x += (i32)getMatrixWidth(matrix) + MATRIX_PADDING;
    }
}