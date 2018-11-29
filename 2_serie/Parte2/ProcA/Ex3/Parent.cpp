#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>


#define BUFFER_SIZE 100

VOID createReadObserver(LPVOID readObserver) {
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[BUFFER_SIZE];
	DWORD written;
	while (TRUE) {
		if (ReadFile((HANDLE)readObserver, buffer, BUFFER_SIZE, &written, NULL)) {
			WriteFile(stdOut, buffer, written, NULL, NULL);
		}
	}
}

VOID CreatePipes(HANDLE *pipes) {
	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.bInheritHandle = TRUE;
	HANDLE writeParent, readParent;
	HANDLE writeChild, readChild;
	BOOL parentPipe = CreatePipe(&readChild, &writeParent, &sa, 0);
	assert(parentPipe == TRUE);
	BOOL childPipe = CreatePipe(&readParent, &writeChild, &sa, 0);
	assert(childPipe == TRUE);
	pipes[0] = writeParent;
	pipes[1] = readParent;
	pipes[2] = writeChild;
	pipes[3] = readChild;
}


HANDLE createChildProcess(HANDLE writeChild, HANDLE readChild) {
	char cmd[128];
	_stprintf_s(cmd, _T("..\\Debug\\Ex3Child.exe %d %d"), (DWORD)writeChild, (DWORD)readChild);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	BOOL pChild = CreateProcess(_T("..\\Debug\\Ex3Child.exe"), cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	assert(pChild == TRUE);
	return pi.hProcess;
}


VOID ReadFromConsole(HANDLE stdIn, HANDLE write, char *buffer) {
	DWORD written;
	if (!ReadFile(stdIn, buffer, BUFFER_SIZE, &written, NULL)) {
		printf("couldn't read");
		return;
	}
	buffer[written] = '\0';
	WriteFile(write, buffer, strlen(buffer), NULL, NULL);
}

VOID WriteToHandle(HANDLE read, char *buffer) {
	DWORD written;
	WriteFile(read, buffer, strlen(buffer), &written, NULL);
}

VOID ReadConsole(HANDLE write, HANDLE read) {
	HANDLE threadRead = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createReadObserver, (LPVOID)read, 0, NULL);
	while (TRUE) {
		char buffer[BUFFER_SIZE];
		ReadFromConsole(GetStdHandle(STD_INPUT_HANDLE), write, buffer);
		if (buffer != NULL) {
			WriteToHandle(read, buffer);
		}
	}
	CloseHandle(threadRead);
	CloseHandle(write);
	CloseHandle(read);
}

VOID Talk(HANDLE write, HANDLE read, HANDLE pChild) {
	ReadConsole(write, read);
	WaitForSingleObject(pChild, INFINITE);	
}

int main() {
	HANDLE pipes[4];
	CreatePipes(pipes);
	HANDLE pChild = createChildProcess(pipes[2], pipes[3]);
	CloseHandle(pipes[2]);
	CloseHandle(pipes[3]);
	Talk(pipes[0], pipes[1], pChild);
	return 0;
}