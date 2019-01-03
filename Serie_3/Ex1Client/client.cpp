#include <Windows.h>
#include "../ex1/ex1.h"
#include <stdio.h>


#define MAX_PATH 17


VOID printHistogram(LPVOID userCtx, DWORD status, UINT32 *histogram) {
	for (size_t i = 0; i < 26; i++)
	{
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
	char *files[1];
	AsyncInit();

	for (int i = 0; i < 2; ++i) {
		files[i] = (char *)malloc(MAX_PATH);
		if (i == 1) {
			sprintf_s(files[i], MAX_PATH, "../ex1/file.txt");
			HistogramFileAsync(files[i], printHistogram, files[i]);
		}
		else {
			sprintf_s(files[i], MAX_PATH, "../ex1/file1.txt");
			HistogramFileAsync(files[i], printHistogram, files[i]);
		}
	}
	AsyncTerminate();
}

int main() {
	printf("starting test in one file");
	TestSingleFileCount();
	printf("end TestSingleFileCount");
	printf("press any key to start count multiple files");
	getchar();
	TestMultiplesFilesCount();
	printf("end TestMultiplesFilesCount");
	printf("press any key to end the application");
	getchar();
}