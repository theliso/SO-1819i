#include <Windows.h>
#include "../ex1/ex1.h"
#include <stdio.h>


#define MAX_PATH 17
#define NUMBER_OF_FILES 4


VOID printHistogram(LPVOID userCtx, DWORD status, UINT32 *histogram) {
	char *ctx = (PCHAR)userCtx;

	for (size_t i = 0; i < 26; i++)
	{
		UINT32 j = histogram[i];
		printf("%s -> %c: %d \n", (PCHAR)userCtx, ('A' + i), histogram[i]);
	}
}

VOID TestSingleFileCount() {
	printf("begin TestSingleFileCount");
	char *file = (char *)malloc(MAX_PATH);

	AsyncInit();

	sprintf_s(file, MAX_PATH, "../ex1/file.txt");
	
	HistogramFileAsync(file, printHistogram, file);

	AsyncTerminate();
}

VOID TestMultiplesFilesCount() {
	printf("begin TestMultiplesFilesCount");
	char *files[NUMBER_OF_FILES];
	AsyncInit();
	for (int i = 0; i < NUMBER_OF_FILES; ++i) {
		files[i] = (char *)malloc(MAX_PATH);
		if (i == 0) {
			snprintf(files[i], MAX_PATH, "../ex1/file.txt");
			HistogramFileAsync(files[i], printHistogram, files[i]);
		}
		else {
			snprintf(files[i], MAX_PATH, "../ex1/file%d.txt", i);
			HistogramFileAsync(files[i], printHistogram, files[i]);
		}
	}
	AsyncTerminate();
	for (int i = 0; i < NUMBER_OF_FILES; ++i) {
		free(files[i]);
	}
}

int main() {
	printf("starting test in one file \n");
	TestSingleFileCount();
	printf("end TestSingleFileCount \n");
	printf("press any key to start count multiple files \n");
	getchar();
	TestMultiplesFilesCount();
	printf("end TestMultiplesFilesCount \n");
	printf("press any key to end the application \n");
	getchar();
}