#pragma once
#include <Windows.h>

typedef struct {
	volatile LONG counter;
}COUNT_WORK, *PCOUNT_WORK;

VOID Initialize(PCOUNT_WORK work) {
	InterlockedExchange(&work->counter, 0);
}

VOID WorkIncremet(PCOUNT_WORK work) {

	InterlockedIncrement(&work->counter);

}

VOID WorkDecrement(PCOUNT_WORK work) {
	if (InterlockedAnd(&work->counter, -1L) > 0) {
		InterlockedDecrement(&work->counter);
	}
}

VOID WaitForWorkDone(PCOUNT_WORK work) {
	while (InterlockedAnd(&work->counter, -1L))
		;
}

