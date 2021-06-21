#pragma once

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

typedef struct KeyMap      { u8 ctrl, alt, shift, space, tab; } KeyMap;
typedef struct IsPressed { bool ctrl, alt, shift, space, tab; } IsPressed;
typedef struct Controls {
    IsPressed is_pressed;
    KeyMap key_map;
    Mouse mouse;
} Controls;

void initControls(Controls *controls) {
    initMouse(&controls->mouse);
}