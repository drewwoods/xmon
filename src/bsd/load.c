#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include "xmon.h"

int load_init(void)
{
	return 0;
}

void load_update(void)
{
	struct loadavg la;
	size_t len = sizeof la;

	sysctlbyname("vm.loadavg", &la, &len, 0, 0);

	smon.loadavg[0] = la.ldavg[0] / (float)la.fscale;
	smon.loadavg[1] = la.ldavg[1] / (float)la.fscale;
	smon.loadavg[2] = la.ldavg[2] / (float)la.fscale;
}
