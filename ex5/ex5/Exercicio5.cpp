#include<stdio.h>
#include<Windows.h>
#include<tchar.h>


typedef struct {
	BYTE tagNumber;
	BYTE dataFormat;
	BYTE nrOfComponents;
	BYTE offsetDataValue;
}IMAGE_FILE_DIRECTORY, *PIMAGE_FILE_DIRECTORY;


typedef struct {
	DWORD nrOfEntries;
	IMAGE_FILE_DIRECTORY imagefile[1];
}TABLE_ENTRY, *PTABLE_ENTRY;

VOID PrintLastError() {
	printf("" + GetLastError());
}

//Navigate through pointers in the structure and print the metadata
VOID PrintTags(LPVOID view) {

}

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

VOID PrintExifTagsA(CHAR *filename) {
	LPVOID view = CreateFileHandlerA(filename);
	if (view == NULL) {
		PrintLastError();
		return;
	}
	PrintTags(view);
}

VOID PrintExifTagsW(WCHAR *filename) {
	LPVOID view = CreateFileHandlerW(filename);
	if (view == NULL) {
		PrintLastError();
		return;
	}
	PrintTags(view);
}



int main(DWORD argc, TCHAR *argv[]) {
	if (argc < 2) {
		printf("The arguments are few!");
		return 1;
	}
	PrintExifTags(argv[1]);
	return 0;
}