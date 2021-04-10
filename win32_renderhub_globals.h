#pragma once
#include <windows.h>
#include <d3d11.h>

#include "renderhub_types.h"

extern uint8* g_keyboard_key_states;
extern Mouse_State* g_mouse_state;

extern Window_Properties* g_window_properties;
extern Display_Properties* g_display_properties;

extern IDXGISwapChain* g_swap_chain;
extern ID3D11Device* g_device;
extern ID3D11DeviceContext* g_device_context;