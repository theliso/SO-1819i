#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>
#include<Windows.h>
#include<tchar.h>
#include "Exercicio5.h"

#define INTEL 0x4949
#define MOTOROLA 0x4d4d

#define MODEL_INTEL 0x0110
#define MODEL_MOT 0x1001

#define DATE_TIME_INTEL 0X9003
#define DATE_TIME_MOT 0X0390

#define ISO_INTEL 0X8827
#define ISO_MOT 0X2788

#define APERTURE_VALUE_INTEL 0X9202
#define APERTURE_VALUE_MOT 0x0292

#define IMAGE_WIDTH_INTEL 0Xa002
#define IMAGE_WIDTH_MOT 0X02a0

#define IMAGE_HEIGHT_INTEL 0Xa003
#define IMAGE_HEIGHT_MOT 0X03a0

#define EXIF_SUB_OFFSET_INTEL 0x8769
#define EXIF_SUB_OFFSET_MOT 0x6987




#pragma pack(push,1)

typedef struct{
	USHORT		begin_Mark;
	USHORT		size;
}MARK, *PMARK; 

typedef struct {
	UINT		exifHeader;
	USHORT		exifData;
}BEFORETIFF, *PBEFORETIFF;

typedef struct {
	USHORT		brand;
	USHORT		tagMark;
	UINT		offsetToFirstIFD;

}TIFF_HEADER, *PTIFF_HEADER;

typedef struct {
	USHORT tagNumber;
	USHORT dataFormat;
	UINT nrOfComponents;
	UINT offsetDataValue;
}ENTRIES, *PENTRIES;


typedef struct {
	USHORT numberOfEntries;
	ENTRIES entries[];
	//PENTRIES entries;
}NUMBER_ENTRIES, *PNUMBER_ENTRIES;



#pragma pack(pop)

DWORD intelij[2] = {INTEL,MOTOROLA};

const char *data_formats[12] = {"%d","%s","%d","%d","%f","%d",NULL,"%d","%d","%f","%f","%f"};


VOID PrintLastError() {
	printf("%d\n", GetLastError());
}

VOID VerifyInIntelMode(PENTRIES entry, LPVOID header ) {
	char *data_address = (char *)(((char *)header) + entry->offsetDataValue);
	const char *data_format = data_formats[entry->dataFormat-1];
	
	if (entry->tagNumber == MODEL_INTEL) {
		printf("Model: ");
		for (int i = 0; i < entry->nrOfComponents;++i) {
			printf("%c", data_address[i]);
		}
		printf("\n");
		return;
	}		
	if (entry->tagNumber == DATE_TIME_INTEL) {
		printf("Data: ");
		for (int i = 0; i < entry->nrOfComponents; ++i) {
			printf("%c", data_address[i]);
		}

		printf("\n");
		return;
	}
	if (entry->tagNumber == ISO_INTEL) {
		char *res = (char *)malloc(sizeof("ISO: ") + sizeof(data_format));
		sprintf(res, "ISO: ", data_format);
		printf(res, *data_address);
		printf("\n");
		return;
	}
	if (entry->tagNumber == APERTURE_VALUE_INTEL){
		char *res = (char *)malloc(sizeof("Abertura: F 1 / ") + sizeof(data_format));
		sprintf(res, "Abertura: F 1 / ", data_format);
		printf(res, *data_address);
		printf("\n");
		return;
	}		
	if (entry->tagNumber == IMAGE_WIDTH_INTEL) {
		char *res = (char *)malloc(sizeof("Dimensao ") + sizeof(data_format));
		sprintf(res, "Dimensao ", data_format);
		printf(res, *data_address);
		printf(" px x ");
		printf("\n");
		return;
	}		
	if (entry->tagNumber == IMAGE_HEIGHT_INTEL) {
		char *res = (char *) (sizeof(data_format));
		sprintf(res, data_format);
		printf(res, *data_address);
		printf(" px");
		printf("\n");
	}
}

VOID VerifyInMMMode(PENTRIES entry, LPVOID header) {
	char *data_address = (char *)(((char *)header) + entry->offsetDataValue);
	const char *data_format = data_formats[entry->dataFormat];

	if (entry->tagNumber == MODEL_MOT){
		printf("Model: ");
		for (int i = 0; i < entry->nrOfComponents; ++i) {
			printf("%c", data_address[i]);
		}
		printf("\n");
		return;
	}
	if (entry->tagNumber == DATE_TIME_MOT) {
		char *res = (char *)malloc(sizeof("Data: ") + sizeof(data_format));
		sprintf(res, "Data: ", data_format);
		printf(res, *data_address);
		printf("\n");
		return;
	}
		

	if (entry->tagNumber == ISO_MOT) {
		char *res = (char *)malloc(sizeof("ISO: ") + sizeof(data_format));
		sprintf(res, "ISO: ", data_format);
		printf(res, *data_address);
		printf("\n");
		return;
	}
		
	if (entry->tagNumber == APERTURE_VALUE_MOT) {
		char *res = (char *)malloc(sizeof("Abertura: F 1 / ") + sizeof(data_format));
		sprintf(res, "Abertura: F 1 / ", data_format);
		printf(res, *data_address);
		printf("\n");
		return;
	}
	if (entry->tagNumber == IMAGE_WIDTH_MOT) {
		char *res = (char *)malloc(sizeof("Dimensao ") + sizeof(data_format));
		sprintf(res, "Dimensao ", data_format);
		printf(res, *data_address);
		printf(" px x ");
		printf("\n");
		return;
	}
	if (entry->tagNumber == IMAGE_HEIGHT_MOT) {
		char *res = (char *)(sizeof(data_format));
		sprintf(res, data_format);
		printf(res, *data_address);
		printf(" px");
		printf("\n");
	}
}

//Navigate through pointers in the structure and print the metadata
VOID PrintTags(LPVOID baseView) {
	/*PTIFF_HEADER tHeader = (PTIFF_HEADER)baseView;
	printf("Mark: %x \n", tHeader->mark);
	printf("Size: %x \n", tHeader->size);
	printf("ExifHeader: %x \n", tHeader->exifHeader);
	printf("ExifData: %x \n", tHeader->exifData);
	printf("Brand: %x \n", tHeader->brand);
	printf("TagMark: %x \n", tHeader->tagMark);	
	printf("OffsetToIFD: %x \n", tHeader->offsetToFirstIFD);
	*/

	//Get Over the starting mark
	PMARK mark = (PMARK)(((char *)baseView) + sizeof(USHORT));
	LPVOID past_starting = (LPVOID) (((char *)baseView) + sizeof(USHORT));


	while (mark->begin_Mark != 0xD9FF && mark->begin_Mark != 0xE1FF){
		//baseView = (char *)((char *)baseView + tHeader->size + sizeof(tHeader ->mark));
		mark = (PMARK)((char *)past_starting + sizeof(USHORT) + mark ->size);
		printf("Mark: %x \n", mark->begin_Mark);
		past_starting = (LPVOID)(((char *)past_starting) +sizeof(USHORT) + mark->size);
	}
	
	PTIFF_HEADER tHeader = (PTIFF_HEADER)(((char *)past_starting) + sizeof(MARK) + sizeof(BEFORETIFF));
	printf("Brand: %x \n", tHeader->brand);
	printf("TagMark: %x \n", tHeader->tagMark);
	printf("OffsetToIFD: %x \n", tHeader->offsetToFirstIFD);

	//char* aux = (char *)(tHeader->offsetToFirstIFD + (sizeof(UINT) * 2) + (sizeof(USHORT)) + ((char*)past_starting));
	char* aux = (char *)(tHeader->offsetToFirstIFD + ((char *)tHeader));
	PNUMBER_ENTRIES helper = (PNUMBER_ENTRIES)aux;
	USHORT n_elems = helper->numberOfEntries;
	printf("N_ELEMS: %x \n", n_elems);

	aux = aux + sizeof(USHORT);
	ULONG sub_offset;
	for (DWORD i = 0; i < n_elems; ++i,aux += sizeof(ENTRIES)) {
		PENTRIES entry = (PENTRIES) aux;
		printf("EntryTag: 0x%x \n", entry->tagNumber);
		if ((entry->tagNumber == EXIF_SUB_OFFSET_INTEL)
			|| (entry->tagNumber == EXIF_SUB_OFFSET_MOT)) {
			sub_offset = entry->offsetDataValue;		
		}
		if (tHeader->brand == INTEL) {
			VerifyInIntelMode(entry, tHeader);
		}
		else {
			VerifyInMMMode(entry, tHeader);
		}
	}
	UINT ifd1_offset = (UINT)aux;
	printf("EntryTag: 0x%x \n", ifd1_offset);
	//char *something = (char *)(((char *) aux) + ifd1_offset);
	char *aux_ifd1 = ((char *) baseView) + ifd1_offset;
	n_elems = (USHORT)aux_ifd1;
	aux_ifd1 = aux_ifd1 + sizeof(USHORT);
	

	for (DWORD i = 0; i < n_elems; ++i, aux_ifd1 += sizeof(ENTRIES)) {
		PENTRIES entry = (PENTRIES)aux_ifd1;
		printf("EntryTag: 0x%x \n", entry->tagNumber);

		if ((entry->tagNumber == EXIF_SUB_OFFSET_INTEL)
			|| (entry->tagNumber == EXIF_SUB_OFFSET_MOT)) {
			sub_offset = entry->offsetDataValue;
			aux_ifd1 += sizeof(ENTRIES);
			break;
		}
		if (tHeader->brand == INTEL) {
			VerifyInIntelMode(entry, tHeader);
		}
		else {
			VerifyInMMMode(entry, tHeader);
		}
	}
	
	char *aux_sub = ((char *)tHeader) + sub_offset;
	n_elems = (USHORT)aux;
	aux_sub = aux_sub + sizeof(USHORT);


	for (DWORD i = 0; i < n_elems; ++i, aux_sub += sizeof(ENTRIES)) {
		PENTRIES entry = (PENTRIES)aux_sub;
		printf("EntryTag: 0x%x \n", entry->tagNumber);

		if ((entry->tagNumber == EXIF_SUB_OFFSET_INTEL)
			|| (entry->tagNumber == EXIF_SUB_OFFSET_MOT)) {
			sub_offset = entry->offsetDataValue;
			aux += sizeof(ENTRIES);
			break;
		}
		if (tHeader->brand == INTEL) {
			VerifyInIntelMode(entry, tHeader);
		}
		else {
			VerifyInMMMode(entry, tHeader);
		}
	}
	//CloseHandle(baseView);
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
	HANDLE hFile = CreateFileA(filename, GENERIC_READ , 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	return MappingHandler(hFile);
}

LPVOID CreateFileHandlerW(WCHAR *filename) {
	HANDLE hFile = CreateFileW((LPCWSTR)filename, GENERIC_READ , 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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