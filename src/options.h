#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <X11/Xlib.h>
#include "xmon.h"

struct vis_options {
	XColor uicolor[NUM_UICOLORS];
	const char *font;
	int frm_width;
	int decor, bevel_thick;
	/* TODO skin */
};

struct cpu_options {
	int ncolors;
};

struct options {
	int x, y, xsz, ysz;
	int upd_interv;

	struct vis_options vis;
	struct cpu_options cpu;
};

struct options opt;

void init_opt(void);

int parse_args(int argc, char **argv);
int read_config(const char *fname);

#define BEVEL	opt.vis.bevel_thick

#endif	/* OPTIONS_H_ */
