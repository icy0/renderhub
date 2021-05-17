#include <windows.h>
#include <windowsx.h>

#include "renderhub_types.h"
#include "renderhub_input.h"
#include "renderhub_logging.h"
#include "win_renderhub_globals.h"

void win32_on_size(HWND hwnd, UINT flag, int width, int height)
{
    char buffer[256];
    g_window_properties->window_width = width;
    g_window_properties->window_height = height;
    sprintf_s(buffer, "WM_SIZE: W:%d, H:%d", width, height);
    rh_log_message(buffer);

    if (g_swap_chain)
    {
        g_device_context->OMSetRenderTargets(0, 0, 0);

        g_render_target_view->Release();
        g_depth_stencil_view->Release();
        g_swap_chain->ResizeBuffers(
            RENDERER_BUFFER_COUNT,
            g_window_properties->window_width,
            g_window_properties->window_height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            NULL);

        ID3D11Texture2D* back_buffer = nullptr;
        g_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &back_buffer);
        g_device->CreateRenderTargetView(back_buffer, nullptr, &g_render_target_view);

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

        g_device->CreateTexture2D(&depth_stencil_buffer_description, nullptr, &back_buffer);
        g_device->CreateDepthStencilView(back_buffer, nullptr, &g_depth_stencil_view);

        rh_dx_logging(back_buffer->Release());
        back_buffer = nullptr;
        rh_assert(!back_buffer);

        g_device_context->OMSetRenderTargets(1, &g_render_target_view, g_depth_stencil_view);
    }
}

LRESULT CALLBACK win32_callback_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

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