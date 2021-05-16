#include "renderhub_types.h"
#include <iostream>

void win32_setup_console_io()
{
    int32 console_alloc_successfully = AllocConsole();
    if (!console_alloc_successfully)
    {
        OutputDebugStringA("couldn't create console successfully!\n");
    }
    else
    {
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        SetConsoleTitleA("Debug Console");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
    }
}

void win32_log_message(const char* message, const char* file, int32 line)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
    std::cout << file << ", Line " << line << ": " << message << std::endl;
}

void win32_log_warning(const char* message, const char* file, int32 line)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
    std::cout << file << ", Line " << line << ": " << message << std::endl;
}

void win32_log_error(const char* message, const char* file, int32 line)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
    std::cout << file << ", Line " << line << ": " << message << std::endl;
}