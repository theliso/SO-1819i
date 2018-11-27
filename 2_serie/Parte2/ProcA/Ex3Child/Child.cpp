#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>

VOID createReadObserver(HANDLE readObserver) {
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	LPVOID buffer = NULL;
	DWORD bufferInfoRead = 0;
	while (TRUE) {
		if (ReadFile(readObserver, buffer, bufferInfoRead, NULL, NULL)) {
			WriteFile(stdOut, buffer, bufferInfoRead, NULL, NULL);
		}
	}
}

VOID createWriteObserver(HANDLE writeObserver) {
	HANDLE stdIn = GetStdHandle(STD_INPUT_HANDLE);
	LPVOID buffer = NULL;
	DWORD bufferInfoRead = 0;
	while (TRUE) {
		if (ReadFile(stdIn, buffer, bufferInfoRead, NULL, NULL)) {
			WriteFile(writeObserver, buffer, bufferInfoRead, NULL, NULL);
		}
	}
}


VOID createOperationsToEvents(HANDLE write, HANDLE read, HANDLE *threads) {
	HANDLE readObserver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createReadObserver, (LPVOID)read, CREATE_SUSPENDED, NULL);
	HANDLE writeObserver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createReadObserver, (LPVOID)write, CREATE_SUSPENDED, NULL);
	threads[0] = readObserver;
	threads[1] = writeObserver;
}


int main(int argc, char *argv[]) {
	HANDLE writeParent = argv[0];
	HANDLE readParent = argv[1];
	HANDLE parentProcess = argv[2];
	HANDLE threads[2];
	createOperationsToEvents(writeParent, readParent, threads);
	ResumeThread(threads[0]);
	ResumeThread(threads[1]);
	while (TRUE) {
		TCHAR buf[100];
		_getts_s(buf, _countof(buf));
	}
	CloseHandle(writeParent);
	CloseHandle(readParent);
	for (UINT i = 0; i < 2; ++i) {
		CloseHandle(threads[i]);
	}
	return 0;
}