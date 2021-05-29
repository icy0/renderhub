#include <d3d11.h>
#include <directxmath.h>
#include <DirectXColors.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "renderhub_types.h"
#include "renderhub_assert.h"
#include "renderhub_logging.h"

#include "win_renderhub_globals.h"

void win32_get_display_devices()
{
	int32 device_count = 0;
	DISPLAY_DEVICEA dummy_display_device = {};
	while (EnumDisplayDevicesA(NULL, device_count, &dummy_display_device, EDD_GET_DEVICE_INTERFACE_NAME))
	{
		device_count++;
	}

	DISPLAY_DEVICEA* display_device = new DISPLAY_DEVICEA[device_count];
	ZeroMemory(display_device, sizeof(DISPLAY_DEVICEA) * device_count);

	int32* device_graphics_modes_count = new int32[device_count];
	ZeroMemory(device_graphics_modes_count, sizeof(int32) * device_count);

	int32 total_sum_of_graphics_modes = 0;

	for (int32 display_devices = 0; display_devices < device_count; display_device++)
	{
		EnumDisplayDevicesA(NULL, display_devices, &display_device[display_devices], EDD_GET_DEVICE_INTERFACE_NAME);
		DEVMODEA dummy_devmode;
		while (EnumDisplaySettingsA(display_device[display_devices].DeviceName, device_graphics_modes_count[display_devices], &dummy_devmode))
		{
			device_graphics_modes_count[display_devices]++;
			total_sum_of_graphics_modes++;
		}
	}

	DEVMODEA* device_graphics_modes = new DEVMODEA[total_sum_of_graphics_modes];
	ZeroMemory(device_graphics_modes, sizeof(DEVMODEA) * total_sum_of_graphics_modes);

	int32 index = 0;
	for (int display_devices = 0; display_devices < device_count; display_devices++)
	{
		for (int graphics_modes = 0; graphics_modes < device_graphics_modes_count[display_devices]; graphics_modes++, index++)
		{
			EnumDisplaySettingsA(display_device[display_devices].DeviceName, graphics_modes, &device_graphics_modes[index]);
		}
	}
}

void win32_get_current_display_device()
{
	DEVMODE device_graphics_mode = {};
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &device_graphics_mode);

	g_display_properties->horizontal_pixel_count = device_graphics_mode.dmPelsWidth;
	g_display_properties->vertical_pixel_count = device_graphics_mode.dmPelsHeight;
	g_display_properties->refresh_rate = device_graphics_mode.dmDisplayFrequency;
}

void win32_init_directx11()
{
	HRESULT result;

#ifdef _DEBUG
	typedef HRESULT(WINAPI* DXGIGetDebugInterface_Type)(REFIID, void**);
	HMODULE dxgi_debug_dll = LoadLibraryEx(L"Dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	// HMODULE dxgi_debug_dll = GetModuleHandle(L"dxgidebug.dll");
	rh_assert(dxgi_debug_dll);
	DXGIGetDebugInterface_Type DXGIGetDebugInterface = 
		(DXGIGetDebugInterface_Type)((void*)GetProcAddress(dxgi_debug_dll, "DXGIGetDebugInterface"));
	rh_assert(DXGIGetDebugInterface);
	result = DXGIGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&g_info_queue));
	rh_assert(SUCCEEDED(result));
#endif

	DXGI_MODE_DESC back_buffer_display_mode;
	ZeroMemory(&back_buffer_display_mode, sizeof(back_buffer_display_mode));
	back_buffer_display_mode.Width = g_window_properties->window_width;
	back_buffer_display_mode.Height = g_window_properties->window_height;
	back_buffer_display_mode.RefreshRate.Numerator = g_display_properties->refresh_rate;
	back_buffer_display_mode.RefreshRate.Denominator = 1;
	back_buffer_display_mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	back_buffer_display_mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	back_buffer_display_mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swap_chain_description;
	ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
	swap_chain_description.BufferDesc = back_buffer_display_mode;
	swap_chain_description.SampleDesc.Count = 1;
	swap_chain_description.SampleDesc.Quality = 0;
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_description.BufferCount = RENDERER_BUFFER_COUNT;
	swap_chain_description.OutputWindow = g_window_properties->window_handle;
	swap_chain_description.Windowed = true;
	// swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_description.Flags = 0;

	uint32 device_creation_flags = 0;

#if defined(_DEBUG)
	device_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	rh_dx_logging(result = D3D11CreateDeviceAndSwapChain(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL, 
		device_creation_flags,
		feature_levels,
		ARRAYSIZE(feature_levels),
		D3D11_SDK_VERSION, 
		&swap_chain_description, 
		&g_swap_chain,
		&g_device,
		NULL, 
		&g_device_context));
	rh_assert(SUCCEEDED(result));
	rh_assert(g_swap_chain);
	rh_assert(g_device);
	rh_assert(g_device_context);

	ID3D11Texture2D* back_buffer = nullptr;
	rh_dx_logging(result = g_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &back_buffer));
	rh_assert(SUCCEEDED(result));

	rh_dx_logging(result = g_device->CreateRenderTargetView(back_buffer, nullptr, &g_render_target_view));
	rh_assert(SUCCEEDED(result));
	rh_assert(g_render_target_view);

	rh_dx_logging(back_buffer->Release());
	back_buffer = nullptr;
	rh_assert(!back_buffer);

	D3D11_TEXTURE2D_DESC depth_stencil_buffer_description;
	ZeroMemory(&depth_stencil_buffer_description, sizeof(D3D11_TEXTURE2D_DESC));

	depth_stencil_buffer_description.ArraySize = 1;
	depth_stencil_buffer_description.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_buffer_description.CPUAccessFlags = 0;
	depth_stencil_buffer_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_buffer_description.Width = g_window_properties->window_width;
	depth_stencil_buffer_description.Height = g_window_properties->window_height;
	depth_stencil_buffer_description.MipLevels = 1;
	depth_stencil_buffer_description.SampleDesc.Count = 1;
	depth_stencil_buffer_description.SampleDesc.Quality = 0;
	depth_stencil_buffer_description.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* depth_stencil_buffer = nullptr;
	rh_dx_logging(result = g_device->CreateTexture2D(&depth_stencil_buffer_description, nullptr, &depth_stencil_buffer));
	rh_assert(SUCCEEDED(result));
	rh_assert(depth_stencil_buffer);

	rh_dx_logging(result = g_device->CreateDepthStencilView(depth_stencil_buffer, nullptr, &g_depth_stencil_view));
	rh_assert(SUCCEEDED(result));
	rh_assert(g_depth_stencil_view);

	rh_dx_logging(depth_stencil_buffer->Release());
	depth_stencil_buffer = nullptr;
	rh_assert(!depth_stencil_buffer);

	D3D11_DEPTH_STENCIL_DESC depth_stencil_state_description = {};
	depth_stencil_state_description.DepthEnable = true;
	depth_stencil_state_description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_state_description.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_state_description.StencilEnable = false;

	rh_dx_logging(result = g_device->CreateDepthStencilState(&depth_stencil_state_description, &g_depth_stencil_state));
	rh_assert(SUCCEEDED(result));
	rh_assert(g_depth_stencil_state);

	rh_dx_logging(g_device_context->OMSetDepthStencilState(g_depth_stencil_state, 1));

	D3D11_RASTERIZER_DESC rasterizer_state_description = {};
	rasterizer_state_description.FillMode = D3D11_FILL_SOLID;
	rasterizer_state_description.CullMode = D3D11_CULL_BACK;
	rasterizer_state_description.FrontCounterClockwise = false;
	rasterizer_state_description.DepthBias = 0;
	rasterizer_state_description.DepthBiasClamp = 0.0f;
	rasterizer_state_description.SlopeScaledDepthBias = 0.0f;
	rasterizer_state_description.DepthClipEnable = true;
	rasterizer_state_description.ScissorEnable = false;
	rasterizer_state_description.MultisampleEnable = false;
	rasterizer_state_description.AntialiasedLineEnable = false;

	rh_dx_logging(result = g_device->CreateRasterizerState(&rasterizer_state_description, &g_rasterizer_state));
	rh_assert(SUCCEEDED(result));
	rh_assert(g_rasterizer_state);

	rh_dx_logging(g_device_context->RSSetState(g_rasterizer_state));

	g_viewport->Width = (FLOAT) g_window_properties->window_width;
	g_viewport->Height = (FLOAT) g_window_properties->window_height;
	g_viewport->TopLeftX = 0.0f;
	g_viewport->TopLeftY = 0.0f;
	g_viewport->MinDepth = 0.0f;
	g_viewport->MaxDepth = 1.0f;

	rh_dx_logging(g_device_context->OMSetRenderTargets(1, &g_render_target_view, g_depth_stencil_view));
	rh_dx_logging(g_device_context->RSSetViewports(1, g_viewport));
}