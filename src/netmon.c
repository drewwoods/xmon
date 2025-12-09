#include <stdio.h>
#include <string.h>
#include "xmon.h"
#include "options.h"

static XRectangle rect;
static unsigned int rx_acc, tx_acc;
static unsigned int rx_rate, tx_rate;
static long last_upd;

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

static void update(void)
{
	unsigned long msec, interv;

	if(!opt.net.show_rate) return;

	rx_acc += smon.net_rx;
	tx_acc += smon.net_tx;

	msec = get_msec();
	interv = msec - last_upd;
	if(!interv) return;

	rx_rate = rx_acc * 1000 / interv;
	tx_rate = tx_acc * 1000 / interv;
	rx_acc = tx_acc = 0;
	last_upd = msec;
}

void netmon_draw(void)
{
	char tbuf[64], rbuf[64];
	int baseline;

	update();

	baseline = rect.y + font_height - font->descent - 1;

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_BG].pixel);
	XFillRectangle(dpy, win, gc, rect.x, rect.y, rect.width, font_height * 3);

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_FG].pixel);
	XDrawString(dpy, win, gc, rect.x, baseline, "NET", 3);

	baseline += font_height;

	XSetForeground(dpy, gc, opt.vis.uicolor[COL_FG].pixel);

	if(opt.net.show_rate) {
		strcpy(rbuf, "Rx ");
		memfmt(rbuf + 3, rx_rate, 0);
		strcat(rbuf, "/s");

		strcpy(tbuf, "Tx ");
		memfmt(tbuf + 3, tx_rate, 0);
		strcat(tbuf, "/s");
	} else {
		strcpy(rbuf, "Rx ");
		memfmt(rbuf + 3, smon.net_rx, 0);

		strcpy(tbuf, "Tx ");
		memfmt(tbuf + 3, smon.net_tx, 0);
	}

	XDrawString(dpy, win, gc, rect.x, baseline, rbuf, strlen(rbuf));
	baseline += font_height;
	XDrawString(dpy, win, gc, rect.x, baseline, tbuf, strlen(tbuf));
}
