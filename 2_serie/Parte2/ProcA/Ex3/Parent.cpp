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
		BOOL xpto = ReadFile(stdIn, buffer, bufferInfoRead, NULL, NULL);
		if (xpto == 1) {
			WriteFile(writeObserver, buffer, bufferInfoRead, NULL, NULL);
		}
	}
}


VOID associateOperationsToThreads(HANDLE write, HANDLE read, HANDLE *threadsObserver) {
	HANDLE readObserver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createReadObserver, (LPVOID)read, 0, NULL);
	HANDLE writeObserver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createWriteObserver, (LPVOID)write, 0, NULL);
	threadsObserver[0] = readObserver;
	threadsObserver[1] = writeObserver;
}

VOID createTalkInfraStructure() {
	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.bInheritHandle = TRUE;
	HANDLE writeParent, readParent;
	HANDLE writeChild, readChild;
	BOOL parentPipe = CreatePipe(&readChild, &writeParent, &sa, 0);
	assert(parentPipe == TRUE);
	BOOL childPipe = CreatePipe(&readParent, &writeChild, &sa, 0);
	assert(childPipe == TRUE);
	HANDLE threads[2];
	associateOperationsToThreads(writeChild, readChild, threads);
	HANDLE commandLine[3] = { writeParent, readParent, GetCurrentProcess() };
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	BOOL pChild = CreateProcess(_T("..\\Debug\\Ex3Child.exe"), (LPSTR)commandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	assert(pChild == TRUE);
	CloseHandle(readParent);
	CloseHandle(writeParent);
	ResumeThread(pi.hThread);
	//test
	while (TRUE) {
		TCHAR buf[100];
		_getts_s(buf, _countof(buf));
	}
	CloseHandle(writeChild);
	CloseHandle(readChild);
	CloseHandle(pi.hThread);
	for (UINT i = 0; i < 2; ++i) {
		CloseHandle(threads[i]);
	}
}



int main() {
	createTalkInfraStructure();
	return 0;
}