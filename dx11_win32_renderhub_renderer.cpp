#include <d3d11.h>

#include "renderhub_types.h"
#include "win32_renderhub_globals.h"

void win32_get_display_devices()
{
	int32 device_count = 0;
	DISPLAY_DEVICE dummy_display_device;
	while (EnumDisplayDevices(NULL, device_count, &dummy_display_device, EDD_GET_DEVICE_INTERFACE_NAME))
	{
		device_count++;
	}

	DISPLAY_DEVICE* display_device = new DISPLAY_DEVICE[device_count];
	ZeroMemory(display_device, sizeof(DISPLAY_DEVICE) * device_count);

	int32* device_graphics_modes_count = new int32[device_count];
	ZeroMemory(device_graphics_modes_count, sizeof(int32) * device_count);

	int32 total_sum_of_graphics_modes = 0;

	for (int32 display_devices = 0; display_devices < device_count; display_device++)
	{
		EnumDisplayDevices(NULL, display_devices, &display_device[display_devices], EDD_GET_DEVICE_INTERFACE_NAME);
		DEVMODE dummy_devmode;
		while (EnumDisplaySettings(display_device[display_devices].DeviceName, device_graphics_modes_count[display_devices], &dummy_devmode))
		{
			device_graphics_modes_count[display_devices]++;
			total_sum_of_graphics_modes++;
		}
	}

	DEVMODE* device_graphics_modes = new DEVMODE[total_sum_of_graphics_modes];
	ZeroMemory(device_graphics_modes, sizeof(DEVMODE) * total_sum_of_graphics_modes);

	int32 index = 0;
	for (int display_devices = 0; display_devices < device_count; display_devices++)
	{
		for (int graphics_modes = 0; graphics_modes < device_graphics_modes_count[display_devices]; graphics_modes++, index++)
		{
			EnumDisplaySettings(display_device[display_devices].DeviceName, graphics_modes, &device_graphics_modes[index]);
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
	swap_chain_description.BufferCount = 1; // TODO not 2?
	swap_chain_description.OutputWindow = g_window_properties->window_handle;
	swap_chain_description.Windowed = true;
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

	result = D3D11CreateDeviceAndSwapChain(NULL, 
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
		&g_device_context);

	if (FAILED(result))
	{
		OutputDebugStringA("Device / Swapchain creation has failed.");
	}
}