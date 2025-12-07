#ifndef XMON_H_
#define XMON_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifndef NO_XSHM
#include <X11/extensions/XShm.h>
#endif
#include "widget.h"

struct sysmon {
	/* CPU usage range [0, 127] */
	int single;		/* aggregate CPU usage for all CPUs */
	int *cpu;		/* per CPU usage */
	int num_cpus;

	float loadavg[3];

	unsigned long mem_total, mem_free;	/* in kb */
	unsigned long net_rx, net_tx;		/* in bytes */
};

extern struct sysmon smon;

extern Display *dpy;
extern int scr;
extern Window win, root;
extern XVisualInfo *vinf;
extern Colormap cmap;
extern GC gc;

#ifndef NO_XSHM
extern XShmSegmentInfo xshm;
extern int have_xshm;
#endif

extern XFontStruct *font;
extern int font_height;

extern int quit;

int cpu_init(void);
int mem_init(void);
int load_init(void);
int net_init(void);

void cpu_update(void);
void mem_update(void);
void load_update(void);
void net_update(void);

int cpumon_init(void);
void cpumon_destroy(void);
void cpumon_move(int x, int y);
void cpumon_resize(int x, int y);
int cpumon_height(int w);
void cpumon_update(void);
void cpumon_draw(void);

void memmon_move(int x, int y);
void memmon_resize(int x, int y);
int memmon_height(int w);
void memmon_draw(void);

int loadmon_init(void);
void loadmon_move(int x, int y);
void loadmon_resize(int x, int y);
int loadmon_height(int w);
void loadmon_draw(void);

void netmon_move(int x, int y);
void netmon_resize(int x, int y);
int netmon_height(int w);
void netmon_draw(void);

int memfmt(char *buf, long mem);		/* defined in memmon.c */

#endif	/* XMON_H_ */
