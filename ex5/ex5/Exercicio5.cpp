#include<stdio.h>
#include<Windows.h>
#include<tchar.h>

typedef struct {
	DWORD height;
	DWORD width;
	
};

LPVOID MappingHandler(HANDLE hFile) {
	HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, 0);
	if (hMap == NULL) {
		CloseHandle(hFile);
		return NULL;
	}
	LPVOID hView = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hMap);
	CloseHandle(hFile);
	return hView;
}

LPVOID CreateFileHandlerA(CHAR *filename) {
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	return MappingHandler(hFile);
}

LPVOID CreateFileHandlerW(WCHAR *filename) {
	HANDLE hFile = CreateFileW(filename, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	return MappingHandler(hFile);
}

VOID PrintExifTags(CHAR *filename) {
	
	
	HANDLE fileView = MapViewOfFile(hMap, FILE_MAP_COPY, NULL, NULL, 0);
	CloseHandle(hMap);

}



int main(DWORD argc, TCHAR *argv[]) {
	if (argc < 2) {
		printf("The arguments are few!");
		return 1;
	}
	PrintExifTags(*argv[1]);
	return 0;
}