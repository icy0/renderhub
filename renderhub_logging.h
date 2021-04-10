#pragma once
#include "win32_renderhub_logging.h"

#ifdef _WIN32

	#ifndef rh_log_message
	#define rh_log_message(str) \
			if((str)) \
			{ \
				win32_log_message((str), __FILE__, __LINE__); \
			} \
			else { }\

	#endif // ifndef rh_log_message

	#ifndef rh_log_warning
	#define rh_log_warning(str) \
			if((str)) \
			{ \
				win32_log_warning((str), __FILE__, __LINE__); \
			} \
			else { }\

	#endif // ifndef rh_log_warning

	#ifndef rh_log_error
	#define rh_log_error(str) \
		if((str)) \
		{ \
			win32_log_error((str), __FILE__, __LINE__); \
		} \
		else { }\

	#endif // ifndef rh_log_error

#else // else if _WIN32 (other OSes)

	#ifdef // Linux macro
		// TODO LINUX logging
	#endif

#endif // ifdef _WIN32