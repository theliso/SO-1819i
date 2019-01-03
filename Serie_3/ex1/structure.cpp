#include <Windows.h>
#include "ex1.h"
#include "Sync.h"
#include <stdio.h>
#include <tchar.h>
#include <string.h>

#define BUFFER_SIZE 100
#define WORK 1
#define DONE 2
#define MAX_THREADS 100

typedef struct {
	OVERLAPPED ov;
	CHAR buffer[BUFFER_SIZE];
	HANDLE hfile;
	DWORD read;
	DWORD fileSize;
	UINT32 *histogram;
	AsyncCallback cb;
	LPVOID userCtx;
} USERCTX, *PUSERCTX;



volatile HANDLE ioCompletionPort;
volatile LONG Terminate = 0;
volatile LONG HasInitialized = 0;
volatile LONG CanProcced = 1;
COUNT_WORK work;



HANDLE CreateNewCompletionPort(DWORD numberOfThreads) {
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numberOfThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hComplPort, HANDLE hDevice, DWORD CompletionKey) {
	HANDLE h = CreateIoCompletionPort(hDevice, hComplPort, CompletionKey, 0);
	return h == hComplPort;
}

PUSERCTX CreateContext(HANDLE hfile, AsyncCallback cb, LPVOID userCtx) {
	PUSERCTX ctx = (PUSERCTX)calloc(1, sizeof(USERCTX));
	ctx->cb = cb;
	ctx->hfile = hfile;
	ctx->userCtx = userCtx;
	ctx->read = 0;
	ctx->fileSize = GetFileSize(hfile, NULL);
	ctx->histogram = (UINT32 *)calloc(26, sizeof(UINT) * 26);
	return ctx;
}

VOID EraseCtx(PUSERCTX ctx) {
	CloseHandle(ctx->hfile);
	free(ctx->histogram);
	free(&ctx->ov);
	free(ctx->buffer);
	free(ctx);
}

VOID SetFileOffset(LPOVERLAPPED ov, UINT64 offset) {
	LARGE_INTEGER li;
	li.QuadPart = offset;
	ov->Offset = li.LowPart;
	ov->OffsetHigh = li.HighPart;
}

BOOL ReadFileAsync(HANDLE hfile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped) {
	if (!ReadFile(hfile, lpBuffer, nNumberOfBytesToRead, NULL, lpOverlapped)) {
		DWORD error = GetLastError();
		return error == ERROR_IO_PENDING;
	}
	return TRUE;
}

VOID ReleaseContext(PUSERCTX ctx, DWORD status) {
	if (ctx->cb != NULL) {
		ctx->cb(ctx->userCtx, status, ctx->histogram);
	}
	EraseCtx(ctx);
}


BOOL HistogramFileAsync(PCSTR file, AsyncCallback cb, LPVOID userCtx) {
	HANDLE hfile = CreateFile(_T(file), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hfile == NULL) {
		return FALSE;
	}
	AssociateDeviceWithCompletionPort(ioCompletionPort, hfile, WORK);

	PUSERCTX ctx = CreateContext(hfile, cb, userCtx);
	WorkIncremet(&work);

	if (!ReadFileAsync(ctx->hfile, ctx->buffer, BUFFER_SIZE, &ctx->ov)) {
		ReleaseContext(ctx, GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL ProccessRead(PUSERCTX ctx, BOOL queueStatus, DWORD bytesTransfered) {
	DWORD error = GetLastError();

	ctx->read += bytesTransfered;

	if (!queueStatus && error == ERROR_HANDLE_EOF) {
		WorkDecrement(&work);
		ReleaseContext(ctx, error);
		return FALSE;
	}
	for (DWORD i = 0; i < bytesTransfered; ++i) {
		char c = ctx->buffer[i];
		int idx = toupper(c) - 'A';
		UINT aux = ctx->histogram[idx];
		aux += 1;
		ctx->histogram[idx] = aux;
	}
	if (ctx->fileSize == ctx->read) {
		WorkDecrement(&work);
		ReleaseContext(ctx, error);
		return FALSE;
	}
	SetFileOffset(&ctx->ov, ctx->read);
	if (!ReadFileAsync(ctx->hfile, ctx->buffer, BUFFER_SIZE, &ctx->ov)) {
		ReleaseContext(ctx, GetLastError());
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI DelegateWork(LPVOID lparams) {
	DWORD bytesTransfered = 0;
	DWORD completionKey = 0;
	OVERLAPPED *ov = 0;
	while (TRUE) {
		BOOL res = GetQueuedCompletionStatus(ioCompletionPort, &bytesTransfered, &completionKey, &ov, INFINITE);

		if (completionKey == WORK) {
			if (!ProccessRead((PUSERCTX)ov, res, bytesTransfered)) break;
		}
		else {
			if (completionKey == DONE) break;
		}
	}
	return 0;
}

BOOL AsyncInit() {
	DWORD previousValue = InterlockedExchange(&HasInitialized, TRUE);
	if (previousValue == FALSE) {

		while (InterlockedAnd(&CanProcced, -1L) == 0);

		DWORD nrOfChars = 26;
		ioCompletionPort = CreateNewCompletionPort(MAX_THREADS);
		if (ioCompletionPort == INVALID_HANDLE_VALUE) return FALSE;
		for (DWORD i = 0; i < MAX_THREADS; ++i) {
			HANDLE thread = CreateThread(NULL, 0, DelegateWork, NULL, 0, NULL);
		}
		Initialize(&work);
		InterlockedExchange(&Terminate, 0);
		return TRUE;
	}

	while (InterlockedAnd(&CanProcced, -1L) == 0);

	return TRUE;
}

VOID AsyncTerminate() {
	WaitForWorkDone(&work);
	if (InterlockedCompareExchange(&Terminate, 1, 0) == 0) {
		CloseHandle(ioCompletionPort);
		for (int i = 0; i < MAX_THREADS; ++i) {
			PostQueuedCompletionStatus(ioCompletionPort, 0, DONE, NULL);
		}
		InterlockedExchange(&CanProcced, 0);
		InterlockedExchange(&HasInitialized, 0);
	}
}


//----------------------------------

VOID printHistogram(LPVOID userCtx, DWORD status, UINT32 *histogram) {
	for (size_t i = 0; i < 26; i++)
	{
		printf("%s -> %c: %d \n", (PCHAR)userCtx, ('A' + i), histogram[i]);
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("specify the file you want to open and proccess!\n");
		return 0;
	}
	char *p = argv[1];
	if (!AsyncInit()) {
		printf("couldn't initialize the structure!\n");
		return 0;
	}

	if (!HistogramFileAsync(p, printHistogram, p)) {
		DWORD error = GetLastError();
		if (error == ERROR_HANDLE_EOF || error == ERROR_HANDLES_CLOSED) {
			printf("finish proccessing file\n");
		}
		else {
			if (error == ERROR_INVALID_HANDLE) {
				printf("there was an error on proccessing file! \n");
			}
		}
	}
	printf("finish main\n");
	AsyncTerminate();
	getchar();
	return 0;
}

