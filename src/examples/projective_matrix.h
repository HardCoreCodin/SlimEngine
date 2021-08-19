#pragma once

#include "./projective_base.h"

#define MATRIX_MAX_CHARS_PER_ROW (3 * 5 + 2)
#define MATRIX_PADDING 5
#define MATRIX_MAX_ROW_WIDTH (FONT_WIDTH * MATRIX_MAX_CHARS_PER_ROW)
#define MATRIX_MAX_ROW_HEIGHT (1 + FONT_HEIGHT + 1)
#define MATRIX_MAX_WIDTH  (MATRIX_PADDING + MATRIX_MAX_ROW_WIDTH + MATRIX_PADDING)
#define MATRIX_MAX_HEIGHT (MATRIX_PADDING + (MATRIX_MAX_ROW_HEIGHT * 3) + MATRIX_PADDING)
#define MATRIX_MAX_SIZE (MATRIX_MAX_WIDTH * MATRIX_MAX_HEIGHT)
#define MATRIX_DEFAULT_X_AXIS_COLOR BrightRed
#define MATRIX_DEFAULT_Y_AXIS_COLOR BrightGreen
#define MATRIX_DEFAULT_Z_AXIS_COLOR BrightBlue

#define MAX_MATRIX_COUNT 8

typedef struct Matrix {
    mat3 M;
    NumberString components[3][3];
    RGBA component_colors[3][3];
} Matrix;

Matrix main_matrix, matrices[MAX_MATRIX_COUNT];
u8 matrix_count = 0;

PixelGrid matrix_image;

void initMatrix(Matrix *matrix) {
    matrix->M = getMat3Identity();
    for (u8 x = 0; x < 3; x++)
        for (u8 y = 0; y < 3; y++) {
            initNumberString(&matrix->components[y][x]);
            matrix->component_colors[y][x] = Color(
                        y ? (
                            y == 1 ? MATRIX_DEFAULT_Y_AXIS_COLOR :
                                     MATRIX_DEFAULT_Z_AXIS_COLOR
                         )         : MATRIX_DEFAULT_X_AXIS_COLOR
            );
            copyToString(&matrix->components[y][x].string,
                  x == y ? (
                          x ? (
                                  x == 1 ?
                                      "Y" :
                                      "Z"
                                  ) : "X"
                          ) : "0",
                          0);
        }
}

u32 getMatrixRowLength(Matrix *matrix, u8 row_index) {
    return (matrix->components[row_index][0].string.length +
            matrix->components[row_index][1].string.length +
            matrix->components[row_index][2].string.length);
}
u32 getMatrixColumnLength(Matrix *matrix, u8 column_index) {
    return max(max(
            matrix->components[0][column_index].string.length,
            matrix->components[1][column_index].string.length),
               matrix->components[2][column_index].string.length);
}

u32 getMatrixMaxRowLength(Matrix *matrix) {
    return max(max(getMatrixRowLength(matrix, 0),
                   getMatrixRowLength(matrix, 1)),
                   getMatrixRowLength(matrix, 2));
}
u32 getMatrixWidth(Matrix *matrix) {
    return MATRIX_PADDING + ((getMatrixMaxRowLength(matrix) + 2) * FONT_WIDTH) + MATRIX_PADDING;
}

void updateMatrixStrings(Matrix *matrix) {
    static NumberString number_string;
    for (u8 row = 0; row < 3; row++) {
        for (u8 column = 0; column < 3; column++) {
            //                vec3 *axis = row ? (row == 1 ? &matrix->M.Y : &matrix->M.Z) : &matrix->M.X;
            //                printNumberIntoString((i32)(column ? (column == 1 ? axis->y : axis->z) : axis->x), &number_string);
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
    Rect rect;
    rect.min = Vec2i(0, 0);
    rect.max = Vec2i(width, MATRIX_MAX_HEIGHT);
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
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, MATRIX_MAX_HEIGHT - 0);
    drawHLine2D(&matrix_image, background_color, rect.min.x, rect.max.x, MATRIX_MAX_HEIGHT - 1);

    vec2i offset = Vec2i(MATRIX_PADDING, MATRIX_PADDING);
    for (u8 row_index = 0; row_index < 3; row_index++) {
        offset.x = MATRIX_PADDING;
        for (u8 column_index = 0; column_index < 3; column_index++) {
            drawText(&matrix_image,
                     matrix->component_colors[row_index][column_index],
                     matrix->components[row_index][column_index].string.char_ptr, offset.x, offset.y);
            offset.x += FONT_WIDTH * ((i32)getMatrixColumnLength(matrix, column_index) + 1);
        }
        offset.y += MATRIX_MAX_ROW_HEIGHT;
    }
    copyPixels(&matrix_image, app->viewport.frame_buffer, width, MATRIX_MAX_HEIGHT, x, y);
}

void drawMatrixHUD(PixelGrid *canvas) {
    Rect rect;

    rect.min.x = app->viewport.frame_buffer->dimensions.width - MATRIX_MAX_WIDTH;
    rect.min.y = app->viewport.frame_buffer->dimensions.height - MATRIX_MAX_HEIGHT;
    rect.max.x = app->viewport.frame_buffer->dimensions.width;
    rect.max.y = app->viewport.frame_buffer->dimensions.height;
    fillRect(canvas, Color(Black), &rect);
    drawMatrix(&main_matrix, rect.min.x, rect.min.y);

    rect.min.x = rect.min.y = 0;
    rect.max.x = app->viewport.frame_buffer->dimensions.width;
    rect.max.y = 70;
    fillRect(canvas, Color(Black), &rect);

    drawText(canvas, Color(White), "[x`, y`, z`] = [x, y, z] * M", 10, 10);
    drawText(canvas, Color(White), "M = ", 10, 45);

    i32 x = 45;
    i32 y = 30;
    Matrix *matrix = matrices;
    for (u8 i = 0; i < matrix_count; i++, matrix++) {
        drawMatrix(matrix, x, y);
        x += (i32)getMatrixWidth(matrix) + MATRIX_PADDING;
    }
}