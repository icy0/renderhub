#pragma once

#ifndef KEY_DOWN
#define KEY_DOWN 0b1;
#endif

#ifndef KEY_WAS_DOWN
#define KEY_WAS_DOWN 0b10;
#endif

void register_keyboard_input(uint8* key_states, uint64 virtual_keycode, uint8 is_down);
bool key_down(uint8* key_states, uint64 virtual_keycode);
bool key_was_down(uint8* key_states, uint64 virtual_keycode);
void register_mouse_input(Mouse_State* mouse_state, MOUSEBUTTON button_index, bool is_down);
void register_mouse_movement(Mouse_State* mouse_state, int32 x_position, int32 y_position);
void update_keyboard_input(uint8* keyboard_key_states);
void update_mouse_input(Mouse_State* mouse_state);