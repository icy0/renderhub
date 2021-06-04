#pragma once

void read_file_binary(const char* filename, char** file_bytes, uint64* file_size_in_bytes);
obj_model* win32_read_obj(const char* filename);