#ifndef WIDGET_H_
#define WIDGET_H_

/* UI colors */
enum {
	COL_FG,
	COL_BG,
	COL_BGHI,
	COL_BGLO,

	COL_A,
	COL_B,
	COL_AB,

	NUM_UICOLORS
};

extern unsigned int uicolor[NUM_UICOLORS];
extern int bar_height;

int init_widgets(void);

void draw_frame(int x, int y, int w, int h, int depth);
void draw_bar(int x, int y, int w, int val, int total);
void draw_sep(int x, int y, int w);

#endif	/* WIDGET_H_ */
