#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include <dxgidebug.h>

#include "renderhub_types.h"
#include "renderhub_input.h"
#include "renderhub_logging.h"
#include "renderhub_assert.h"
#include "renderhub_resourceloader.h"

#include "win_renderhub_resourceloader.h"
#include "win_renderhub_window_settings.h"
#include "dx11win_renderhub_renderer.h"

uint8* g_keyboard_key_states = new uint8[256];
Mouse_State* g_mouse_state = new Mouse_State{};

Display_Properties* g_display_properties = new Display_Properties();
Window_Properties* g_window_properties = new Window_Properties();

IDXGISwapChain* g_swap_chain = nullptr;
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_device_context = nullptr;

ID3D11RenderTargetView* g_render_target_view = nullptr;
ID3D11DepthStencilView* g_depth_stencil_view = nullptr;
ID3D11DepthStencilState* g_depth_stencil_state = nullptr;
ID3D11RasterizerState* g_rasterizer_state = nullptr;
D3D11_VIEWPORT* g_viewport = new D3D11_VIEWPORT{0};

struct IDXGIInfoQueue* g_info_queue = nullptr;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    rh_assert(g_keyboard_key_states);
    rh_assert(g_mouse_state);
    rh_assert(g_display_properties);
    rh_assert(g_window_properties);

    ZeroMemory(g_keyboard_key_states, sizeof(uint8) * 256);
    ZeroMemory(g_mouse_state, sizeof(Mouse_State));
    ZeroMemory(g_display_properties, sizeof(Display_Properties));
    ZeroMemory(g_window_properties, sizeof(Window_Properties));
    ZeroMemory(g_viewport, sizeof(D3D11_VIEWPORT));

    win32_get_current_display_device();
    g_window_properties->window_width = g_display_properties->horizontal_pixel_count;
    g_window_properties->window_height = g_display_properties->vertical_pixel_count;

    win32_setup_console_io();

    const wchar_t CLASS_NAME[] = L"resourceloader_windowclass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = win32_callback_procedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    rh_assert(RegisterClass(&wc));

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"RenderHub", WS_OVERLAPPEDWINDOW, 
        0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);
    rh_assert(hwnd);

    ShowWindow(hwnd, nCmdShow);

    // OBJ_Model* obj_dummy_sphere = nullptr;
    // rh_log_timing(obj_dummy_sphere = win32_read_obj("test_resources\\dummy_sphere.obj"));
    // rh_assert(obj_dummy_sphere);

    // Mesh* mesh_dummy_sphere = convert_to_mesh(obj_dummy_sphere);
    // rh_assert(mesh_dummy_sphere);

    g_window_properties->window_handle = hwnd;

    win32_init_directx11();

    MSG msg = {};

    LARGE_INTEGER performance_frequency;
    QueryPerformanceFrequency(&performance_frequency);

    LARGE_INTEGER prev_frametime;
    QueryPerformanceCounter(&prev_frametime);

    double fps = 0.0;
    // char fps_print_buffer[256];

    LARGE_INTEGER delta_time = {};
    LARGE_INTEGER curr_frametime = {};

    uint64 prev_cycle_count = __rdtsc();
    uint64 delta_cycle_count = 0;
    uint64 curr_cycle_count = 0;
    uint32 checking_delay = 0;

    while (msg.message != WM_QUIT)
    {
        if(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        else
        {
            // sprintf_s(fps_print_buffer, "FPS: %.5f, CPU-Cycles / Frame: %lld", fps, delta_cycle_count);
            // rh_log_message(fps_print_buffer);

            curr_cycle_count = __rdtsc();
            delta_cycle_count = curr_cycle_count - prev_cycle_count;
            prev_cycle_count = curr_cycle_count;

            QueryPerformanceCounter(&curr_frametime);
            delta_time.QuadPart = curr_frametime.QuadPart - prev_frametime.QuadPart;
            delta_time.QuadPart *= 1'000'000;
            delta_time.QuadPart /= performance_frequency.QuadPart;

            fps = 1.0 / (delta_time.QuadPart / 1'000'000.0f);
            prev_frametime.QuadPart = curr_frametime.QuadPart;

            // after update and render
            update_keyboard_input(g_keyboard_key_states);
            update_mouse_input(g_mouse_state);
        }
    }

    return 0;
}