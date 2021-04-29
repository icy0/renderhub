#include <windows.h>
#include <windowsx.h>

#include "renderhub_types.h"
#include "renderhub_input.h"
#include "win32_renderhub_globals.h"

void win32_on_size(HWND hwnd, UINT flag, int width, int height)
{
    g_window_properties->window_width = width;
    g_window_properties->window_height = height;

    if (g_swap_chain)
    {
        g_device_context->OMSetRenderTargets(0, 0, 0);

        // TODO(paul): release outstanding reference(s) to swapchain buffers
        // TODO(paul): preserve existing buffer count and format
        // TODO(paul): swapchain.ResizeBuffers()
        // TODO(paul): create new render target view
        // TODO(paul): set output merger render target
        // TODO(paul):

    }
}

LRESULT CALLBACK win32_callback_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char msg[256];
    switch (uMsg)
    {
    case WM_SIZE:
    {
        int width = LOWORD(lParam);  // Macro to get the low-order word.
        int height = HIWORD(lParam); // Macro to get the high-order word.

        // Respond to the message:
        win32_on_size(hwnd, (UINT)wParam, width, height);
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