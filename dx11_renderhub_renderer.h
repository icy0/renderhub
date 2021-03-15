#pragma once
#include <d3d11.h>
#include "renderhub_types.h"

void win32_get_display_devices();
void win32_get_current_display_device(Display_Properties* display_properties);

void init_directx11(Display_Properties* display_properties,
	Window_Properties* window_properties,
	IDXGISwapChain* swap_chain,
	ID3D11Device* device,
	ID3D11DeviceContext* device_context);