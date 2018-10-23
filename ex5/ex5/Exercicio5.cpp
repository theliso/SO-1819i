#include<stdio.h>
#include<Windows.h>
#include<tchar.h>

typedef struct {
	DWORD height;
	DWORD width;
	
};


VOID PrintExifTags(TCHAR filename) {
	HANDLE hfile = CreateFileMapping((HANDLE)filename, NULL, PAGE_READONLY, NULL, NULL, (LPCSTR)filename);	
	HANDLE fileView = MapViewOfFile(hfile, );
}



int main(DWORD argc, TCHAR *argv[]) {
	if (argc < 2) {
		printf("The arguments are few!");
		return 1;
	}
	PrintExifTags(*argv[1]);
	return 0;
}