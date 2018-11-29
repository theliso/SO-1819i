#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>

#define BUFFER_SIZE 200

bool terminate = FALSE;

VOID createReadObserver(LPVOID readObserver) {
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[BUFFER_SIZE];
	DWORD written;
	while (TRUE) {
		if (ReadFile((HANDLE)readObserver, buffer, BUFFER_SIZE, &written, NULL)) {
				WriteFile(stdOut, buffer, written, NULL, NULL);
		}
		if (GetLastError() == ERROR_BROKEN_PIPE) {
			terminate = TRUE;
			break;
		}
		buffer[written - 2] = '\0';
		DWORD len = strlen(buffer);
		if (strcmp(buffer, "exit") == 0) {
			terminate = TRUE;
			break;
		}
	}
}


VOID ReadFromConsole(HANDLE hInput, HANDLE hWrite, char *buffer) {

	DWORD written;

	if (!ReadFile(hInput, buffer, BUFFER_SIZE, &written, NULL)) {
		printf("error on reading input");
		return;
	}
	buffer[written] = '\0';
	WriteFile(hWrite, buffer, strlen(buffer), NULL, NULL);
}

VOID WriteToHandle(HANDLE read, char *buffer) {
	DWORD written;
	WriteFile(read, buffer, strlen(buffer), &written, NULL);
}

VOID ReadConsole(HANDLE write, HANDLE read) {
	HANDLE readObserver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createReadObserver, (LPVOID)read, NULL, NULL);
	while (terminate == 0) {
		char buffer[BUFFER_SIZE];
		if (terminate) {
			break;
		}
		ReadFromConsole(GetStdHandle(STD_INPUT_HANDLE), write, buffer);
		if (buffer != NULL) {
			WriteToHandle(read, buffer);
		}
		if (strcmp(buffer, "exit") == 0) {
			break;
		}
	}
	CloseHandle(write);
	CloseHandle(read);
	CloseHandle(readObserver);
}


int main(int argc, char *argv[]) {
	HANDLE writeChild = (HANDLE)_tstoi(argv[1]);
	HANDLE readChild = (HANDLE)_tstoi(argv[2]);
	ReadConsole(writeChild, readChild);
	return 0;
}