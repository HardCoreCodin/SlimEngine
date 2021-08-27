#pragma once

#include "./projective_base.h"

#define MATRIX_PADDING 5
#define MATRIX_MAX_ROW_HEIGHT (1 + FONT_HEIGHT + 1)
#define MATRIX_HUD_START_X 10
#define MATRIX_HUD_START_Y 10


enum ColorID MATRIX_AXIS_COLORS[4] = {
        BrightRed,
        BrightGreen,
        BrightBlue,
        BrightMagenta
};

#define MAX_MATRIX_COUNT 8

typedef struct Matrix {
    mat4 M;
    NumberString components[4][4];
    RGBA component_colors[4][4];
    u8 dim;
    bool is_custom;
} Matrix;

Matrix main_matrix, final_matrix, matrices[MAX_MATRIX_COUNT];
u8 matrix_count = 0;


void initMatrix(Matrix *matrix) {
    matrix->dim = 4;
    matrix->is_custom = false;
    matrix->M = getMat4Identity();
    for (u8 col = 0; col < 4; col++)
        for (u8 row = 0; row < 4; row++) {
            initNumberString(&matrix->components[row][col]);
            matrix->component_colors[row][col] = Color(MATRIX_AXIS_COLORS[row]);
            copyToString(&matrix->components[row][col].string, col == row ? "1" : "0", 0);
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
    if (matrix == &final_matrix) return secondary_viewport.settings.use_cube_NDC ? 380 : 320;
    return MATRIX_PADDING + ((getMatrixMaxRowLength(matrix) + matrix->dim - 1) * FONT_WIDTH) + MATRIX_PADDING;
}
u32 getMatrixHeight(Matrix *matrix) {
    return MATRIX_PADDING + (matrix->dim * MATRIX_MAX_ROW_HEIGHT) + MATRIX_PADDING;
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

void drawMatrix(PixelGrid *canvas, RGBA background_color, Matrix *matrix, i32 x, i32 y) {
    RGBA foreground_color = Color(White);

    i32 width  = (i32)getMatrixWidth(matrix);
    i32 height = (i32)getMatrixHeight(matrix);
    Rect rect;
    rect.min = Vec2i(x, y);
    rect.max = Vec2i(x + width, y + height);
    fillTransparentRect(canvas, background_color, 0.85f, &rect);
    drawRect(canvas, foreground_color, &rect);
    rect.min.x++;
    rect.min.y++;
    rect.max.x--;
    rect.max.y--;
    drawRect(canvas, foreground_color, &rect);
    rect.min.x--;
    rect.max.x++;
    rect.min.x += MATRIX_PADDING;
    rect.max.x -= MATRIX_PADDING;
    drawHLine2D(canvas, background_color, rect.min.x, rect.max.x, y + 0);
    drawHLine2D(canvas, background_color, rect.min.x, rect.max.x, y + 1);
    drawHLine2D(canvas, background_color, rect.min.x, rect.max.x, y + height - 0);
    drawHLine2D(canvas, background_color, rect.min.x, rect.max.x, y + height - 1);

    vec2i offset = Vec2i(x + MATRIX_PADDING, y + MATRIX_PADDING);
    for (u8 row_index = 0; row_index < matrix->dim; row_index++) {
        offset.x = x + MATRIX_PADDING;
        for (u8 column_index = 0; column_index < matrix->dim; column_index++) {
            drawText(canvas,
                     matrix->component_colors[row_index][column_index],
                     matrix->components[row_index][column_index].string.char_ptr, offset.x, offset.y);
            offset.x += FONT_WIDTH * ((i32)getMatrixColumnLength(matrix, column_index) + 1);
        }
        offset.y += MATRIX_MAX_ROW_HEIGHT;
    }
}

void drawMatrixHUD(PixelGrid *canvas, bool show_focal_length, bool show_aspect_ratio) {
    Rect rect;

    rect.min.x = rect.min.y = 0;
    rect.max.x = canvas->dimensions.width;
    rect.max.y = MATRIX_HUD_START_Y + LINE_HEIGHT * 2 + (i32)getMatrixHeight(show_final_matrix ? &final_matrix : &main_matrix) + MATRIX_PADDING;
    RGBA background_color = Color(Black);
    fillTransparentRect(canvas, background_color, 0.85f, &rect);

    drawText(canvas, default_near_color, "Near Clipping Plane: N = ", MATRIX_HUD_START_X, MATRIX_HUD_START_Y);
    drawText(canvas, default_far_color, "Far  Clipping Plane: F = ", MATRIX_HUD_START_X, MATRIX_HUD_START_Y + LINE_HEIGHT);

    NumberString number;
    initNumberString(&number);

    printFloatIntoString(secondary_viewport.settings.near_clipping_plane_distance, &number, 2);
    drawText(canvas, default_near_color, number.string.char_ptr, MATRIX_HUD_START_X + 25 * FONT_WIDTH, MATRIX_HUD_START_Y);

    printFloatIntoString(secondary_viewport.settings.far_clipping_plane_distance, &number, 2);
    drawText(canvas, default_far_color, number.string.char_ptr, MATRIX_HUD_START_X + 25 * FONT_WIDTH, MATRIX_HUD_START_Y + LINE_HEIGHT);

    if (show_focal_length) {
        printFloatIntoString(secondary_viewport.camera->focal_length, &number, 2);
        drawText(canvas, focal_length_color, "Focal Length : L = ", MATRIX_HUD_START_X + 32 * FONT_WIDTH, MATRIX_HUD_START_Y);
        drawText(canvas, focal_length_color, number.string.char_ptr, MATRIX_HUD_START_X + 32 * FONT_WIDTH + 19 * FONT_WIDTH, MATRIX_HUD_START_Y);

        if (show_aspect_ratio) {
            printFloatIntoString(secondary_viewport_frame_buffer.dimensions.width_over_height, &number, 2);
            drawText(canvas, aspect_ratio_color, "Aspect Ratio : A = ", MATRIX_HUD_START_X + 32 * FONT_WIDTH, MATRIX_HUD_START_Y + LINE_HEIGHT);
            drawText(canvas, aspect_ratio_color, number.string.char_ptr, MATRIX_HUD_START_X + 32 * FONT_WIDTH + 19 * FONT_WIDTH, MATRIX_HUD_START_Y + LINE_HEIGHT);
        }
    }


    rect.min.x = canvas->dimensions.width - (i32)getMatrixWidth(show_final_matrix ? &final_matrix : &main_matrix) - MATRIX_PADDING;
    drawText(canvas, Color(White), show_final_matrix ? "Final Matrix" : "Transformation:", rect.min.x, MATRIX_HUD_START_Y);

    drawMatrix(canvas, background_color, show_final_matrix ? &final_matrix : &main_matrix, rect.min.x, MATRIX_HUD_START_Y + LINE_HEIGHT);

    drawText(canvas, Color(White), "P' = P * M * ", MATRIX_HUD_START_X, MATRIX_HUD_START_Y + LINE_HEIGHT * 3);

    i32 x = MATRIX_HUD_START_X + FONT_WIDTH * 13;
    i32 y = MATRIX_HUD_START_Y + LINE_HEIGHT * 2;
    Matrix *matrix = matrices;
    for (u8 i = 0; i < matrix_count; i++, matrix++) {
        drawMatrix(canvas, background_color, matrix, x, y);
        x += (i32)getMatrixWidth(matrix) + MATRIX_PADDING;
    }
}