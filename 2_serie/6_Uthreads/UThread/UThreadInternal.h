/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2011
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, Jo�o Trindade, Duarte Nunes, Jorge Martins
// 

#pragma once

#include <Windows.h>
#include "UThread.h"
#include "List.h"
#include <assert.h>
//
// The data structure representing the layout of a thread's execution context
// when saved in the stack.
//

// Uncomment next line when build for x64
//#define UTHREAD_X64

#ifdef UTHREAD_X64
//RBX, RBP, RDI, RSI, R12, R13, R14, and R15 
typedef struct _UTHREAD_CONTEXT {
	ULONGLONG R15;
	ULONGLONG R14;
	ULONGLONG R13;
	ULONGLONG R12;
	ULONGLONG RSI;
	ULONGLONG RDI;
	ULONGLONG RBX;
	ULONGLONG RBP;
	VOID (*RetAddr)();
} UTHREAD_CONTEXT, *PUTHREAD_CONTEXT;
#else
typedef struct _UTHREAD_CONTEXT {
	ULONG EDI;
	ULONG ESI;
	ULONG EBX;
	ULONG EBP;
	VOID (*RetAddr)();
} UTHREAD_CONTEXT, *PUTHREAD_CONTEXT;
#endif

//
// The descriptor of a user thread, containing an intrusive link (through which
// the thread is linked in the ready queue), the thread's starting function and
// argument, the memory block used as the thread's stack and a pointer to the
// saved execution context.
//
typedef struct _UTHREAD {
	PUTHREAD_CONTEXT ThreadContext;
	LIST_ENTRY       Link;
	UT_FUNCTION      Function;   
	UT_ARGUMENT      Argument; 
	PUCHAR           Stack;
	DWORD			 ThreadState; // READY (0), RUNNING(1), BLOCKED(2)
	LIST_ENTRY		 Block;
	BOOL			 Cancelled;
} UTHREAD, *PUTHREAD;

//Thread State
#define READY 0
#define RUNNING 1
#define BLOCKED 2

//
// The fixed stack size of a user thread.
//
#define STACK_SIZE (16 * 4096)
