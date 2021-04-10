#include <windows.h>
#include <stdio.h>

#include "renderhub_types.h"
#include "renderhub_input.h"
#include "renderhub_resourceloader.h"
#include "renderhub_logging.h"

#include "win32_renderhub_window_settings.h"
#include "dx11_win32_renderhub_renderer.h"

uint8* g_keyboard_key_states = new uint8[256];
Mouse_State* g_mouse_state = new Mouse_State{};

Display_Properties* g_display_properties = new Display_Properties();
Window_Properties* g_window_properties = new Window_Properties();

IDXGISwapChain* g_swap_chain = nullptr;
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_device_context = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    ZeroMemory(g_keyboard_key_states, sizeof(uint8) * 256);
    ZeroMemory(g_mouse_state, sizeof(Mouse_State));
    ZeroMemory(g_display_properties, sizeof(Display_Properties));
    ZeroMemory(g_window_properties, sizeof(Window_Properties));

    g_window_properties->window_width = g_display_properties->horizontal_pixel_count;
    g_window_properties->window_height = g_display_properties->vertical_pixel_count;

    win32_get_current_display_device();

    win32_setup_console_io();

    const wchar_t CLASS_NAME[] = L"resourceloader_windowclass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = win32_callback_procedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"RenderHub", WS_OVERLAPPEDWINDOW, 
        1520, 0, 1920, 1080, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    //win32_read_obj("test_resources\\case-with-money-low-poly\\source\\two-case-MONEY_triang-LP\\two-case-MONEY_triang-LP.obj");

    g_window_properties->window_handle = hwnd;

    win32_init_directx11();

    MSG msg = {};

    LARGE_INTEGER performance_frequency;
    QueryPerformanceFrequency(&performance_frequency);

    LARGE_INTEGER prev_frametime;
    QueryPerformanceCounter(&prev_frametime);

     double fps;
     char fps_print_buffer[256];

    LARGE_INTEGER delta_time;
    LARGE_INTEGER curr_frametime;

    while (msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            QueryPerformanceCounter(&curr_frametime);
            delta_time.QuadPart = (curr_frametime.QuadPart - prev_frametime.QuadPart);
            delta_time.QuadPart *= 1'000'000;
            delta_time.QuadPart /= performance_frequency.QuadPart;

            fps = 1.0 / (delta_time.QuadPart / 1'000'000.0f);
            prev_frametime.QuadPart = curr_frametime.QuadPart;

             sprintf_s(fps_print_buffer, "FPS: %.5f", fps);
             rh_log_message(fps_print_buffer);

            // TODO call DLL function update(delta_time);
            // TODO call DLL function render();

            // after update and render
            update_keyboard_input(g_keyboard_key_states);
            update_mouse_input(g_mouse_state);
        }
    }

    return 0;
}