#pragma once
#include <windows.h>

LRESULT CALLBACK win32_callback_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void win32_on_size(HWND hwnd, UINT flag, int width, int height);