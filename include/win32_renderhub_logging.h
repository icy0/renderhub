#pragma once

void win32_setup_console_io();

void win32_log_message(const char* message, const char* file, int32 line);
void win32_log_warning(const char* message, const char* file, int32 line);
void win32_log_error(const char* message, const char* file, int32 line);