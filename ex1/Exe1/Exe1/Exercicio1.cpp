#include<Windows.h>
#include<stdio.h>
#include<psapi.h>


typedef struct {
	DWORD img;
	DWORD map;
	DWORD prv;
} CommitCounters, *PCommitCounters;

void emptyCounters(_Out_ PCommitCounters counters) {
	counters->img = 0;
	counters->map = 0;
	counters->prv = 0;
}

BOOL GetCommitCountersFromProcess(int pid, _Out_ PCommitCounters counters) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess == NULL) {
		emptyCounters(counters);
		return FALSE;
	}
	SYSTEM_INFO si;
	PSAPI_WORKING_SET_INFORMATION wsi, *pwsi;
	MEMORY_BASIC_INFORMATION mbi, *pmbi;
	GetSystemInfo(&si);
	QueryWorkingSet(hProcess, &wsi, sizeof(pwsi));

	for (ULONG_PTR i = 0; i < wsi.NumberOfEntries; ++i) {
		VirtualQuery(&wsi.WorkingSetInfo[1], &mbi, sizeof(pmbi));
		if (mbi.State == MEM_COMMIT) {
			if (mbi.Type == MEM_IMAGE) {
				counters->img += 1;
			}
			if (mbi.Type == MEM_MAPPED) {
				counters->map += 1;
			}
			if (mbi.Type == MEM_PRIVATE) {
				counters->prv += 1;
			}
		}
	}
	return TRUE;
}

void printResults(PCommitCounters counters) {
	printf("image counter = %d \n", counters->img);
	printf("mapped counter = %d \n", counters->map);
	printf("private counter = %d \n", counters->prv);
}

int main(int argc, char argv[]) {
	int id;
	printf("Process id: ");
	scanf_s("%d", &id);
	PCommitCounters commit = (PCommitCounters)malloc(sizeof(CommitCounters));
	emptyCounters(commit);
	if (GetCommitCountersFromProcess(id, commit)) {
		printResults(commit);
		return 0;
	}
	printf("%d",GetLastError());
	int i = 0;
	while (i++ < 10000);

	return 0;
}
