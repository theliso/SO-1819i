#include<stdio.h>
#include<Windows.h>
#include<tchar.h>

#define INTEL 0x4949
#define MOTOROLA 0x4d4d

#define MODEL_MOT 0x0110
#define MODEL_INTEL 0x1001

#define DATE_TIME_MOT 0X9003
#define DATE_TIME_INTEL 0X0390

#define ISO_MOT 0X8827
#define ISO_INTEL 0X2788

#define APERTURE_VALUE_MOT 0X9202
#define APERTURE_VALUE_INTEL 0x0292

#define IMAGE_WIDTH_MOT 0Xa002
#define IMAGE_WIDTH_INTEL 0X02a0

#define IMAGE_HEIGHT_MOT 0Xa003
#define IMAGE_HEIGHT_INTEL 0Xa003




#pragma pack(push,1)

typedef struct {
	USHORT		mark;
	USHORT		size;
	UINT		exifHeader;
	USHORT		exifData;
	USHORT		brand;
	USHORT		tagMark;
	UINT		offsetToFirstIFD;

}TIFF_HEADER, *PTIFF_HEADER;

typedef struct {
	USHORT numberOfEntries;
}NUMBER_ENTRIES, *PNUMBER_ENTRIES;

typedef struct {
	USHORT tagNumber;
	USHORT dataFormat;
	UINT nrOfComponents;
	UINT offsetDataValue;
}ENTRIES, *PENTRIES;

#pragma pack(pop)



VOID PrintLastError() {
	printf("" + GetLastError());
}

//Navigate through pointers in the structure and print the metadata
VOID PrintTags(LPVOID baseView) {
	PTIFF_HEADER tHeader = (PTIFF_HEADER)baseView;
	while (tHeader->mark != 0xFFE1){
		baseView = (char *)((char *)baseView + tHeader->size + sizeof(tHeader ->mark));
		tHeader = (PTIFF_HEADER)baseView;
	}
	PNUMBER_ENTRIES nrOfEntries = (PNUMBER_ENTRIES)(tHeader->offsetToFirstIFD + tHeader);
	PENTRIES entry = (PENTRIES)(nrOfEntries + sizeof(PNUMBER_ENTRIES));
	for (DWORD i = 0; i < nrOfEntries->numberOfEntries; ++i) {
		//inc the entries and theirs offset and print
	}
	

	//if(tHeader ->brand == INTEL)


	 
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