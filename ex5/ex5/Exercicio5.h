#pragma once
#include <Windows.h>
#ifdef UNICODE 
#define PrintExifTags PrintExifTagsW

#else 
#define PrintExifTags PrintExifTagsA
#endif

#ifdef EXERCICIO5_DLL_EXPORT
	#define EXERCICIO5_DLL_API _declspec(dllexport)
#else 
#define EXERCICIO5_DLL_API _declspec(dllimport)
#endif 

#ifdef __cplusplus

extern "C" {
#endif 
	EXERCICIO5_DLL_API VOID PrintExifTagsW(WCHAR * filename);
	EXERCICIO5_DLL_API VOID PrintExifTagsA(CHAR* filename);
#ifdef __cplusplus
	}
#endif 
