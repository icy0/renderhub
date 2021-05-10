#pragma once
#include <d3d11.h>
#include "renderhub_types.h"

void win32_get_display_devices();
void win32_get_current_display_device();

void win32_init_directx11();

void dx11_win32_update(uint64 delta_time);
void dx11_win32_render();