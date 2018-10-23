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
	MEMORY_BASIC_INFORMATION mbi, *pmbi;

	while (VirtualQuery(hProcess, &mbi, sizeof(pmbi)) != 0) {
		if (mbi.Type == MEM_IMAGE) {
			counters->img += mbi.RegionSize;
		}
		if (mbi.Type == MEM_MAPPED) {
			counters->map += mbi.RegionSize;
		}
		if (mbi.Type == MEM_PRIVATE) {
			counters->prv += mbi.RegionSize;
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
	if (argc < 3) {
		printf("No process was specified as argument of the program!");
		return 1;
	}
	int id = atoi(&argv[2]);
	PCommitCounters commit = (PCommitCounters)malloc(sizeof(CommitCounters));
	emptyCounters(commit);
	if (GetCommitCountersFromProcess(id, commit)) {
		printResults(commit);
		return 0;
	}
	printf("%d",GetLastError());
	getchar();

	return 0;
}
