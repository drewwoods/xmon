#include <windows.h>
#include "xmon.h"

int mem_init(void)
{
	mem_update();
	return 0;
}

void mem_update(void)
{
	MEMORYSTATUS ms;

	GlobalMemoryStatus(&ms);
	smon.mem_total = ms.dwTotalPhys >> 10;
	smon.mem_free = ms.dwAvailPhys >> 10;
}
