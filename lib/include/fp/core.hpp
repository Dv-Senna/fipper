#pragma once


#ifdef FP_STATIC
	#define FP_CORE
#elif defined(FP_SHARED)
	#ifdef FP_WINDOWS
		#ifdef FP_BUILD_LIB
			#define FP_CORE __declspec(dllexport)
		#else
			#define FP_CORE __declspec(dllimport)
		#endif
	#else
		#define FP_CORE
	#endif
#else
	#error You must define either 'FP_STATIC' or 'FP_SHARED'
#endif
