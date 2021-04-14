#pragma once

#include <stdio.h>

#include "win32_renderhub_logging.h"

// TODO: add stubs for when NOT compiling on _WIN32

#ifdef _WIN32

	#ifndef rh_log_message
	#define rh_log_message(str)\
			if((str))\
			{\
				win32_log_message((str), __FILE__, __LINE__);\
			}\
			else { }\

	#endif // ifndef rh_log_message

	#ifndef rh_log_warning
	#define rh_log_warning(str)\
			if((str))\
			{\
				win32_log_warning((str), __FILE__, __LINE__);\
			}\
			else { }\

	#endif // ifndef rh_log_warning

	#ifndef rh_log_error
	#define rh_log_error(str)\
		if((str))\
		{\
			win32_log_error((str), __FILE__, __LINE__);\
		}\
		else { }\

	#endif // ifndef rh_log_error

	#ifdef _MSC_VER
		#ifndef rh_log_timing
		#define rh_log_timing(macrodef_function_call) \
		{ \
			LARGE_INTEGER macrodef_performance_frequency; \
			LARGE_INTEGER macrodef_start_time; \
			LARGE_INTEGER macrodef_delta_time; \
			LARGE_INTEGER macrodef_end_time; \
			uint64 macrodef_start_cyclecount; \
			uint64 macrodef_end_cyclecount; \
			uint64 macrodef_cpu_cycles; \
			real32 macrodef_delta_seconds; \
			real32 macrodef_delta_milliseconds; \
			real32 macrodef_delta_microseconds; \
			real32 macrodef_delta_nanoseconds; \
			\
			QueryPerformanceFrequency(&macrodef_performance_frequency); \
			QueryPerformanceCounter(&macrodef_start_time); \
			\
			macrodef_start_cyclecount = __rdtsc(); \
			\
			(macrodef_function_call); \
			\
			macrodef_end_cyclecount = __rdtsc(); \
			QueryPerformanceCounter(&macrodef_end_time); \
			\
			macrodef_cpu_cycles = macrodef_end_cyclecount - macrodef_start_cyclecount; \
			macrodef_delta_time.QuadPart = macrodef_end_time.QuadPart - macrodef_start_time.QuadPart; \
			macrodef_delta_time.QuadPart *= 1'000'000'000; \
			macrodef_delta_time.QuadPart /= macrodef_performance_frequency.QuadPart; \
			\
			macrodef_delta_seconds = (macrodef_delta_time.QuadPart / 1'000'000'000.0f); \
			macrodef_delta_milliseconds = (macrodef_delta_time.QuadPart / 1'000'000.0f); \
			macrodef_delta_microseconds = (macrodef_delta_time.QuadPart / 1'000.0f); \
			macrodef_delta_nanoseconds = (real32) macrodef_delta_time.QuadPart; \
			\
			char macrodef_print_buffer[256]; \
			sprintf_s(macrodef_print_buffer, \
			"Performance of "#macrodef_function_call": " \
			"%lld cpu-cycles, %.2f seconds | %.2f ms | %.2f us | %.2f ns", \
			macrodef_cpu_cycles, macrodef_delta_seconds, macrodef_delta_milliseconds, \
			macrodef_delta_microseconds, macrodef_delta_nanoseconds); \
			rh_log_message(macrodef_print_buffer); \
		}
		#endif // ifndef rh_log_timing
	#else // if different compiler than _MSC_VER
		#ifndef rh_log_timing
		#define rh_log_timing(macrodef_function_call) macrodef_function_call;
		#endif // ifndef rh_log_timing
	#endif // ifdef _MSV_VER

#else // else if _WIN32 (other OSes)

	#ifdef // Linux macro
		// TODO LINUX logging
	#endif

#endif // ifdef _WIN32