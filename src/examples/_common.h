#pragma once

#include "../SlimEngine/core/base.h"
#include "../SlimEngine/core/text.h"
#include "../SlimEngine/shapes/rect.h"
// Or using the single-header file:
//#include "../SlimEngine.h"

#define MOUSE_WHEEL__MAX 2000

void drawKeyboard(PixelGrid *canvas, Navigation *navigation) {
    Rect rect;
    i32 right  = canvas->dimensions.width;
    i32 bottom = canvas->dimensions.height;

    // Draw the keyboard contour:
    rect.max.x = right - 2;
    rect.min.x = rect.max.x - 230;
    rect.max.y = bottom - 2;
    rect.min.y = rect.max.y - 100;
    drawRect(canvas, ColorOf(White), &rect);

    // Draw the 'A' key:
    rect.max.x = right - 172;
    rect.min.x = rect.max.x - 20;
    rect.max.y = bottom - 42;
    rect.min.y = rect.max.y - 20;
    fillRect(canvas, ColorOf(navigation->move.left ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.left ? Grey : White), "A", rect.min.x + 2, rect.min.y - 1);

    // Draw the 'S' key:
    rect.min.x += 22;
    rect.max.x += 22;
    fillRect(canvas, ColorOf(navigation->move.backward ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.backward ? Grey : White), "S", rect.min.x + 2, rect.min.y - 1);

    // Draw the 'D' key:
    rect.min.x += 22;
    rect.max.x += 22;
    fillRect(canvas, ColorOf(navigation->move.right ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.right ? Grey : White), "D", rect.min.x + 2, rect.min.y - 1);

    // Draw the 'D' key:
    rect.min.x += 22;
    rect.max.x += 22;
    fillRect(canvas, ColorOf(navigation->move.down ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.down ? Grey : White), "F", rect.min.x + 2, rect.min.y - 1);

    // Draw the 'Q' key:
    rect.min.x -= 28 * 3;
    rect.max.x -= 28 * 3;
    rect.min.y -= 22;
    rect.max.y -= 22;

//    fillRect(canvas, Color(navigation->turn.left ? White : Grey), &rect);
//    drawText(canvas, Color(navigation->turn.left ? Grey : White), "Q", rect.min.x + 5, rect.min.y + 5);

    // Draw the 'W' key:
    rect.min.x += 22;
    rect.max.x += 22;
    fillRect(canvas, ColorOf(navigation->move.forward ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.forward ? Grey : White), "W", rect.min.x + 2, rect.min.y - 1);

    // Draw the 'E' key:
    rect.min.x += 22;
    rect.max.x += 22;
//    fillRect(canvas, Color(navigation->turn.right ? White : Grey), &rect);
//    drawText(canvas, Color(navigation->turn.right ? Grey : White), "E", rect.min.x + 5, rect.min.y + 5);

    // Draw the 'R' key:
    rect.min.x += 22;
    rect.max.x += 22;
    fillRect(canvas, ColorOf(navigation->move.up ? White : Grey), &rect);
    drawText(canvas, ColorOf(navigation->move.up ? Grey : White), "R", rect.min.x + 2, rect.min.y - 1);


    // Draw the left Ctrl key:
    rect.max.x = right - 202;
    rect.min.x = rect.max.x - 28;
    rect.max.y = bottom - 4;
    rect.min.y = rect.max.y - 18;
    fillRect(canvas, ColorOf(app->controls.is_pressed.ctrl ? Blue : Cyan), &rect);

    // Draw the left Alt key:
    rect.min.x += 30;
    rect.max.x += 30;
    fillRect(canvas, ColorOf(app->controls.is_pressed.alt ? Red : Magenta), &rect);

    // Draw the left Shift key:
    rect.min.x -= 30;
    rect.max.x -= 15;
    rect.min.y -= 20;
    rect.max.y -= 20;
    fillRect(canvas, ColorOf(app->controls.is_pressed.shift ? Green : Yellow), &rect);

    // Draw the right Ctrl key:
    rect.max.x = right - 4;
    rect.min.x = rect.max.x - 28;
    rect.max.y = bottom - 4;
    rect.min.y = rect.max.y - 18;
    fillRect(canvas, ColorOf(app->controls.is_pressed.ctrl ? Blue : Cyan), &rect);

    // Draw the right Alt key:
    rect.min.x -= 30;
    rect.max.x -= 30;
    fillRect(canvas, ColorOf(app->controls.is_pressed.alt ? Red : Magenta), &rect);

    // Draw the right Shift key:
    rect.min.x += 15;
    rect.max.x += 30;
    rect.min.y -= 20;
    rect.max.y -= 20;
    fillRect(canvas, ColorOf(app->controls.is_pressed.shift ? Green : Yellow), &rect);

    // Draw the Space key:
    rect.max.x = right - 62;
    rect.min.x = rect.max.x - 110;
    rect.max.y = bottom - 4;
    rect.min.y = rect.max.y - 18;
    fillRect(canvas,
             ColorOf(app->controls.is_pressed.space ?
                     White : Grey), &rect);
}

void drawMouse(PixelGrid *canvas, Mouse *mouse) {
    Rect rect;
    i32 bottom = canvas->dimensions.height;

    // Draw the mouse contour:
    rect.min.x = 2;
    rect.max.x = 62;
    rect.max.y = bottom - 1;
    rect.min.y = rect.max.y - 111;
    drawRect(canvas, ColorOf(White), &rect);

    // Draw the left mouse button:
    rect.min.x = 3;
    rect.max.x = 24;
    rect.max.y = bottom - 70;
    rect.min.y = rect.max.y - 40;
    fillRect(canvas, ColorOf(mouse->left_button.is_pressed ? Blue : Cyan), &rect);

    // Draw the middle mouse button:
    rect.min.x += 20;
    rect.max.x += 20;
    fillRect(canvas, ColorOf(mouse->middle_button.is_pressed ? Green : Yellow), &rect);

    // Draw the right mouse button:
    rect.min.x += 20;
    rect.max.x += 20;
    fillRect(canvas, ColorOf(mouse->right_button.is_pressed ? Red : Magenta), &rect);

    // Draw the mouse wheel:
    rect.min.x = 28;
    rect.max.x = 38;
    rect.max.y = bottom - 73;
    rect.min.y = rect.max.y - 33;
    fillRect(canvas, ColorOf(Grey), &rect);

    // Draw a marker representing the state of the mouse wheel:
    rect.min.x += 1;
    rect.max.x -= 1;
    rect.min.y += 14;
    rect.max.y -= 14;

    static f32 accumulated_mouse_wheel_scroll_amount = 0;
    static f32 mouse_wheel_delta_y = 0;
    if (mouse->wheel_scrolled) {
        accumulated_mouse_wheel_scroll_amount += mouse->wheel_scroll_amount;
        if (accumulated_mouse_wheel_scroll_amount > MOUSE_WHEEL__MAX)
            accumulated_mouse_wheel_scroll_amount = -MOUSE_WHEEL__MAX;
        if (accumulated_mouse_wheel_scroll_amount < -MOUSE_WHEEL__MAX)
            accumulated_mouse_wheel_scroll_amount = MOUSE_WHEEL__MAX;

        mouse_wheel_delta_y = accumulated_mouse_wheel_scroll_amount;
        mouse_wheel_delta_y += MOUSE_WHEEL__MAX;
        mouse_wheel_delta_y /= MOUSE_WHEEL__MAX;
        mouse_wheel_delta_y -= 1;
        mouse_wheel_delta_y *= 38;
        mouse_wheel_delta_y /= -2;

        mouse->wheel_scroll_handled = true;
    }
    rect.min.y += (i32)mouse_wheel_delta_y;
    rect.max.y += (i32)mouse_wheel_delta_y;
    fillRect(canvas, ColorOf(White), &rect);
}

void drawMouseAndKeyboard(Viewport *viewport, Mouse *mouse) {
    drawMouse(viewport->frame_buffer, mouse);
    drawKeyboard(viewport->frame_buffer, &viewport->navigation);
}