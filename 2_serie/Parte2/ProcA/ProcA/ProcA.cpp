#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>

VOID switchThreadInTheSameProcess(UINT times) {
	for (UINT i = 0; i < times; ++i) {
		SwitchToThread();
	}
}

VOID countThreadsSwitchOfTheSameProcess() {
	DWORD timeStart = GetTickCount();
	UINT ctxSwitchTimes = 100000;
	// threads creation
	HANDLE thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)switchThreadInTheSameProcess, (LPVOID)ctxSwitchTimes, CREATE_SUSPENDED, NULL);
	HANDLE thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)switchThreadInTheSameProcess, (LPVOID)ctxSwitchTimes, CREATE_SUSPENDED, NULL);
	// threads processor affinity
	SetProcessAffinityMask(thread1, 0);
	SetProcessAffinityMask(thread2, 0);
	// threads region set
	SetThreadPriority(thread1, THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadPriority(thread2, THREAD_PRIORITY_TIME_CRITICAL);

	ResumeThread(thread1);
	ResumeThread(thread2);
	HANDLE threads[2] = { thread1, thread2 };
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);

	DWORD timeEnd = GetTickCount();
	DWORD timeColapsed = (timeEnd - timeStart);
	printf("time of switch thread = %lu \n", timeColapsed);
}

VOID switchThreadsOfDifferentProcesses() {
	UINT ctxSwitchTimes = 100000;
	for (UINT i = 0; i < ctxSwitchTimes; ++i) {
		SwitchToThread();
	}
}

VOID countThreadsSwitchOfDifferentProcess() {
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	DWORD res;
	res = CreateProcess(_T("..\\Debug\\ProcB.exe"), NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	assert(res == TRUE);
	SetProcessAffinityMask(pi.hThread, 0);
	SetThreadPriority(pi.hThread, THREAD_PRIORITY_TIME_CRITICAL);

	DWORD startTime = GetTickCount();
	HANDLE threadProcA = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)switchThreadsOfDifferentProcesses, NULL, CREATE_SUSPENDED, NULL);
	SetProcessAffinityMask(threadProcA, 0);
	SetThreadPriority(threadProcA, THREAD_PRIORITY_TIME_CRITICAL);
	ResumeThread(threadProcA);
	WaitForSingleObject(pi.hThread, INFINITE);
	DWORD timeEnd = GetTickCount();
	DWORD timeColapsed = (timeEnd - startTime);
	printf("time of switch thread = %lu \n", timeColapsed);
}

int _tmain() {
	printf("---------------- Start tick count of two threads of the same process ----------------\n");
	countThreadsSwitchOfTheSameProcess();
	printf("---------------- End tick count of two threads of the same process ---------------- \n");
	printf("start countThreadsSwitchOfDifferentProcess() \n press ENTER");
	getchar();
	printf("---------------- Start tick count of two threads of different processes ----------------\n");
	countThreadsSwitchOfDifferentProcess();
	printf("---------------- End tick count of two threads of different processes ----------------\n");
	printf("press ENTER to finish");
	getchar();
	return 0;
}