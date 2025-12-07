#include <stdio.h>
#include <string.h>
#include "xmon.h"
#include "options.h"

static XRectangle rect;

int netmon_init(void)
{
	return 0;
}

void netmon_move(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

void netmon_resize(int x, int y)
{
	rect.width = x;
	rect.height = y;
}

int netmon_height(int w)
{
	return font_height * 3 + 2;
}

void netmon_draw(void)
{
	char buf[128];
	int baseline;

	baseline = rect.y + font_height - font->descent - 1;

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_BG].pixel);
	XFillRectangle(dpy, win, gc, rect.x, rect.y, rect.width, font_height * 3);

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_FG].pixel);
	XDrawString(dpy, win, gc, rect.x, baseline, "NET", 3);

	baseline += font_height;

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_FG].pixel);

	strcpy(buf, "Rx ");
	memfmt(buf + 3, smon.net_rx);
	XDrawString(dpy, win, gc, rect.x, baseline, buf, strlen(buf));

	baseline += font_height;

	strcpy(buf, "Tx ");
	memfmt(buf + 3, smon.net_tx);
	XDrawString(dpy, win, gc, rect.x, baseline, buf, strlen(buf));
}
