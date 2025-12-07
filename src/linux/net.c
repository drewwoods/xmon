#ifndef NET_H_
#define NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "xmon.h"

static FILE *fp;
static unsigned long long prev_rx, prev_tx;

int net_init(void)
{
	if(!(fp = fopen("/proc/net/dev", "rb"))) {
		fprintf(stderr, "failed to open /proc/net/dev: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void net_update(void)
{
	int i, ncols;
	char buf[256];
	char *ptr, *endp, *col[16];
	unsigned long long cur_rx, cur_tx;

	fseek(fp, 0, SEEK_SET);

	cur_rx = cur_tx = 0;

	while(fgets(buf, sizeof buf, fp)) {
		if(!(ptr = strchr(buf, ':'))) {
			continue;
		}
		ptr++;

		ncols = 0;
		for(i=0; i<16; i++) {
			while(*ptr && isspace(*ptr)) ptr++;		/* skip leading blanks */
			if(!*ptr || !isdigit(*ptr)) break;		/* should be a number */
			col[ncols++] = ptr;
			while(*ptr && !isspace(*ptr)) ptr++;	/* skip number */
		}

		if(ncols < 9) continue;

		cur_rx += strtoull(col[0], &endp, 10);
		if(endp == col[0]) continue;
		cur_tx += strtoull(col[8], &endp, 10);
		if(endp == col[8]) continue;
	}

	smon.net_rx = cur_rx - prev_rx;
	smon.net_tx = cur_tx - prev_tx;
	prev_rx = cur_rx;
	prev_tx = cur_tx;
}

#endif	/* NET_H_ */
