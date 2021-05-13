#pragma once

#include "renderhub_logging.h"

#ifdef _WIN32
	#ifdef _MSC_VER
		#ifndef rh_assert
		#define rh_assert(x) if(!(x)) __debugbreak(); else {};
		#endif // ifndef rh_assert_or_break
	#else // other compiler than _MSC_VER
		#ifdef rh_assert
		#define rh_assert(x)
		#endif // ifdef rh_assert_or_break
	#endif // ifdef MSC_VER
#endif // ifdef _WIN32

#ifndef rh_static_assert
#include <assert.h>
	#define rh_static_assert(x, message) static_assert(x, message);
#endif // ifndef rh_static_assert