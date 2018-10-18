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

	MEMORY_BASIC_INFORMATION mbi, *pmbi;

	GetSystemInfo(&si);
	SIZE_T bytes = 0, aux = 0, dim = 0;
	PMEMORY_BASIC_INFORMATION pmbi;
	int count = 2;
	while (count-- > 0) {
		if (bytes == 0) {
			bytes = VirtualQueryEx(hProcess, NULL, &mbi, sizeof(mbi));
		}
		else {
			free(pmbi);
		}
			
		if (GetLastError() != 0) {
			emptyCounters(counters);
			return FALSE;
		}
		dim = sizeof(mbi)*bytes;
		pmbi = (PMEMORY_BASIC_INFORMATION)malloc(dim);
		aux = VirtualQueryEx(hProcess, NULL, pmbi, dim);
		if (aux > bytes) {
			bytes = aux;
		}
	}
	for(SIZE_T idx = 0; idx < dim ; idx += pmbi->RegionSize){}
	

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
