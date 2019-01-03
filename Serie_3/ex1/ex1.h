#pragma once

#include <windows.h>

#ifdef ASYNCCOUNT_EXPORTS
#define ASYNC_API __declspec(dllexport)
#else
#define ASYNC_API __declspec(dllimport) 
#endif

typedef VOID(*AsyncCallback)(LPVOID userCtx, DWORD status, UINT32 *transferedBytes);

#ifdef __cplusplus  
extern "C" {
#endif 

	ASYNC_API
		BOOL AsyncInit();

	ASYNC_API
		BOOL HistogramFileAsync(PCSTR file, AsyncCallback cb, LPVOID userCtx);

	ASYNC_API
		VOID AsyncTerminate();

#ifdef __cplusplus  
}
#endif 
