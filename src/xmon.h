#ifndef XMON_H_
#define XMON_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct sysmon {
	int single;
	int *cpu;
	int num_cpus;

	long mem_total, mem_free;
};

/* UI colors */
enum {
	COL_FG,
	COL_BG,
	COL_BGHI,
	COL_BGLO,

	NUM_UICOLORS
};

extern struct sysmon smon;

extern Display *dpy;
extern int scr;
extern Window win, root;
extern XVisualInfo *vinf;
extern Colormap cmap;
extern GC gc;
extern XFontStruct *font;
extern int font_height;

extern int quit;

int cpu_init(void);
int mem_init(void);

void cpu_update(void);
void mem_update(void);

int cpumon_init(void);
void cpumon_destroy(void);
void cpumon_move(int x, int y);
void cpumon_resize(int x, int y);
void cpumon_update(void);
void cpumon_draw(void);

int memmon_init(void);
void memmon_destroy(void);
void memmon_move(int x, int y);
void memmon_resize(int x, int y);
void memmon_draw(void);

void draw_frame(int x, int y, int w, int h, int depth);

#endif	/* XMON_H_ */
