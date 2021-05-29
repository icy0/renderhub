#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgidebug.h>

#include "renderhub_types.h"

#define RENDERER_BUFFER_COUNT 1

extern uint8* g_keyboard_key_states;
extern mouse_state* g_mouse_state;

extern window_properties* g_window_properties;
extern display_properties* g_display_properties;

extern IDXGISwapChain* g_swap_chain;
extern ID3D11Device* g_device;
extern ID3D11DeviceContext* g_device_context;
extern IDXGIFactory* g_gi_factory;
extern IDXGIAdapter** g_dxgi_adapters;

extern ID3D11RenderTargetView* g_render_target_view;
extern ID3D11DepthStencilView* g_depth_stencil_view;
extern ID3D11DepthStencilState* g_depth_stencil_state;
extern ID3D11RasterizerState* g_rasterizer_state;
extern D3D11_VIEWPORT* g_viewport;

// DEBUG ONLY
extern struct IDXGIInfoQueue* g_info_queue;