#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "options.h"

static XColor def_uicolor[NUM_UICOLORS] = {
	{0, 0, 0, 0},
	{0, 0xb4b4, 0xb4b4, 0xb4b4},
	{0, 0xdfdf, 0xdfdf, 0xdfdf},
	{0, 0x6262, 0x6262, 0x6262}
};

void init_opt(void)
{
	opt.x = opt.y = 0;
	opt.xsz = opt.ysz = 100;
	opt.upd_rate = 250;

	memcpy(opt.vis.uicolor, def_uicolor, sizeof opt.vis.uicolor);
	opt.vis.font = "-*-helvetica-bold-r-*-*-12-*";
	opt.vis.frm_width = 4;
	opt.vis.decor = 0;
	opt.vis.bevel_thick = 2;

	opt.cpu.ncolors = 16;
}

int parse_args(int argc, char **argv)
{
	int i, x, y;
	unsigned int width, height, flags;
	char *endp;

	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "-geometry") == 0) {
				if(!argv[++i] || !(flags = XParseGeometry(argv[i], &x, &y, &width, &height))) {
					fprintf(stderr, "-geometry must be followed by WxH+X+Y\n");
					return -1;
				}
				if(flags & XValue) opt.x = x;
				if(flags & YValue) opt.y = y;
				if(flags & WidthValue) opt.xsz = width;
				if(flags & HeightValue) opt.ysz = height;

			} else if(strcmp(argv[i], "-rate") == 0) {
				if(!argv[++i] || (opt.upd_rate = atoi(argv[i])) <= 0) {
					fprintf(stderr, "-rate must be followed by an update rate in milliseconds\n");
					return -1;
				}

			} else if(strcmp(argv[i], "-font") == 0){
				if(!argv[++i]) {
					fprintf(stderr, "-font must be followed by a X font descriptor\n");
					return -1;
				}
				opt.vis.font = argv[i];

			} else if(strcmp(argv[i], "-frame") == 0) {
				if(!argv[++i] || (opt.vis.frm_width = strtol(argv[i], &endp, 0),
							(endp == argv[i] || opt.vis.frm_width < 0))) {
					fprintf(stderr, "-frame must be followed by the frame thickness\n");
					return -1;
				}

			} else if(strcmp(argv[i], "-decor") == 0) {
				opt.vis.decor = 1;
			} else if(strcmp(argv[i], "-nodecor") == 0) {
				opt.vis.decor = 0;

			} else if(strcmp(argv[i], "-bevel") == 0) {
				if(!argv[++i] || (opt.vis.bevel_thick = strtol(argv[i], &endp, 0),
							(endp == argv[i] || opt.vis.bevel_thick < 0))) {
					fprintf(stderr, "-bevel must be followed by the bevel thickness\n");
					return -1;
				}

			} else {
				fprintf(stderr, "unrecognized option: %s\n", argv[i]);
				return -1;
			}

		} else {
			fprintf(stderr, "unexpected argument: %s\n", argv[i]);
			return -1;
		}
	}

	return 0;
}

int read_config(const char *fname)
{
	return -1;
}
