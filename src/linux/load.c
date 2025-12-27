#include <stdlib.h>
#include "xmon.h"

int load_init(void)
{
	return 0;
}

void load_update(void)
{
	double val[3];

	if(getloadavg(val, 3) == -1) {
		return;
	}

	smon.loadavg[0] = (unsigned int)(val[0] * 1024.0);
	smon.loadavg[1] = (unsigned int)(val[1] * 1024.0);
	smon.loadavg[2] = (unsigned int)(val[2] * 1024.0);
}
