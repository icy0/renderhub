#include <Windows.h>
#include <windowsx.h>

#include <stdio.h>

#include "renderhub_types.h"
#include "renderhub_input.h"
#include "dx11_renderhub_renderer.h"

uint8* g_keyboard_key_states;
Mouse_State* g_mouse_state;

Window_Properties* g_window_properties;
Display_Properties* g_display_properties;

IDXGISwapChain* g_swap_chain;
ID3D11Device* g_device;
ID3D11DeviceContext* g_device_context;

double g_lmb_down_counter;

void update(double delta_time)
{

}

void render()
{

}

void on_size(HWND hwnd, UINT flag, int width, int height)
{
    g_window_properties->window_width = width;
    g_window_properties->window_height = height;

    // TODO reinitialize directx
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char msg[256];
    switch (uMsg)
    {
    case WM_SIZE:
    {
        int width = LOWORD(lParam);  // Macro to get the low-order word.
        int height = HIWORD(lParam); // Macro to get the high-order word.

        // Respond to the message:
        on_size(hwnd, (UINT)wParam, width, height);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT paintstruct;
        HDC hdc;

        hdc = BeginPaint(hwnd, &paintstruct);
        EndPaint(hwnd, &paintstruct);
        break;
    }
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        uint8 key_down = (((lParam >> 31) & 1) + 1) % 2;
        register_keyboard_input(g_keyboard_key_states, wParam, key_down);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::LEFT_MOUSE_BUTTON, true);
        break;
    }
    case WM_LBUTTONUP:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::LEFT_MOUSE_BUTTON, false);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::RIGHT_MOUSE_BUTTON, true);
        break;
    }
    case WM_RBUTTONUP:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::RIGHT_MOUSE_BUTTON, false);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::MIDDLE_MOUSE_BUTTON, true);
        break;
    }
    case WM_MBUTTONUP:
    {
        register_mouse_input(g_mouse_state, MOUSEBUTTON::MIDDLE_MOUSE_BUTTON, false);
        break;
    }
    case WM_XBUTTONDOWN:
    {
        if (GET_XBUTTON_WPARAM(wParam) == 1)
            register_mouse_input(g_mouse_state, MOUSEBUTTON::X1_MOUSE_BUTTON, true);
        else
            register_mouse_input(g_mouse_state, MOUSEBUTTON::X2_MOUSE_BUTTON, true);
        break;
    }
    case WM_XBUTTONUP:
    {
        if (GET_XBUTTON_WPARAM(wParam) == 1)
            register_mouse_input(g_mouse_state, MOUSEBUTTON::X1_MOUSE_BUTTON, false);
        else
            register_mouse_input(g_mouse_state, MOUSEBUTTON::X2_MOUSE_BUTTON, false);
        break;
    }
    case WM_MOUSEMOVE:
    {
        register_mouse_movement(g_mouse_state, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    }
    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    g_keyboard_key_states = new uint8[256];
    ZeroMemory(g_keyboard_key_states, sizeof(uint8) * 256);

    g_mouse_state = new Mouse_State{};
    ZeroMemory(g_mouse_state, sizeof(Mouse_State));

    g_display_properties = new Display_Properties();
    ZeroMemory(g_display_properties, sizeof(Display_Properties));
    win32_get_current_display_device(g_display_properties);

    g_window_properties = new Window_Properties();
    ZeroMemory(g_window_properties, sizeof(Window_Properties));
    g_window_properties->window_width = g_display_properties->horizontal_pixel_count;
    g_window_properties->window_height = g_display_properties->vertical_pixel_count;

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"resourceloader_windowclass";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"resourceloader",              // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        0, 0, g_window_properties->window_width, g_window_properties->window_height, // Size and position
        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL);      // Additional application data

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    g_window_properties->window_handle = hwnd;

    init_directx11(g_display_properties, g_window_properties, g_swap_chain, g_device, g_device_context);

    MSG msg = {};

    static DWORD prev_frametime = timeGetTime();
    double fps;
    char fps_print_buffer[256];
    double delta_time;
    DWORD curr_frametime;

    while (msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            curr_frametime = timeGetTime();
            delta_time = (curr_frametime - prev_frametime) / 1000.0f;
            //fps = 1.0 / delta_time;
            prev_frametime = curr_frametime;

            //sprintf_s(fps_print_buffer, "FPS: %.5f\n", fps);
            //OutputDebugStringA(fps_print_buffer);

            update(delta_time);
            render();

            // after update and render
            update_keyboard_input(g_keyboard_key_states);
            update_mouse_input(g_mouse_state);
        }
    }

    return 0;
}