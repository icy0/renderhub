#include <assert.h>

#include "renderhub_types.h"
#include "renderhub_logging.h"
#include "renderhub_input.h"

void register_keyboard_input(uint8* key_states, uint64 virtual_keycode, uint8 is_down)
{
    assert(virtual_keycode < 256);
    if(is_down)
        key_states[virtual_keycode] |= 0b1;
    else
        key_states[virtual_keycode] &= 0b0;
}

bool key_down(uint8* key_states, uint64 virtual_keycode)
{
    return key_states[virtual_keycode] & KEY_DOWN;
}

bool key_was_down(uint8* key_states, uint64 virtual_keycode)
{
    return key_states[virtual_keycode] & KEY_WAS_DOWN;
}

void register_mouse_input(mouse_state* mouse_state, MOUSEBUTTON button_index, bool is_down)
{
    if (is_down)
    {
        mouse_state->button_state_info[button_index] = KEY_DOWN;
    }
    else
    {
        mouse_state->button_state_info[button_index] = 0b0;
    }
}

void register_mouse_movement(mouse_state* mouse_state, int32 x_position, int32 y_position)
{
    mouse_state->cursor_position_x = x_position;
    mouse_state->cursor_position_y = y_position;
}

// this happens at the end of a frame to update the was_down values
void update_keyboard_input(uint8* keyboard_key_states)
{
    for (uint16 key_state = 0; key_state < 256; key_state++)
    {
        if (key_was_down(keyboard_key_states, key_state))
        {
            keyboard_key_states[key_state] &= 0b1;
        }
        if (key_down(keyboard_key_states, key_state))
        {
            keyboard_key_states[key_state] |= 0b10;
        }
    }
}

// this happens at the end of a frame to update the was_down values
void update_mouse_input(mouse_state* mouse_state)
{
    for (uint8 mouse_button_state = 0; mouse_button_state < 5; mouse_button_state++)
    {
        if (key_was_down(mouse_state->button_state_info, mouse_button_state))
        {
            mouse_state->button_state_info[mouse_button_state] &= 0b1; // set was_down to 0
        }
        if (key_down(mouse_state->button_state_info, mouse_button_state))
        {
            mouse_state->button_state_info[mouse_button_state] |= 0b10; // set was_down to 1
        }
    }
}