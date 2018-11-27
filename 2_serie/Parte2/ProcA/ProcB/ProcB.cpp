#include <Windows.h>


int main() {
	UINT ctxSwitchTimes = 100000;
	for (UINT i = 0; i < ctxSwitchTimes; i++)
	{
		SwitchToThread();
	}
	return 0;
}