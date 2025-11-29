#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "xmon.h"
#include "sysmon.h"

#define UPD_RATE_MS	350
#define FONT_DESC	"-*-helvetica-medium-r-*-*-12-*"

static Atom xa_wm_proto, xa_wm_del;

static int win_width = 96, win_height = 96;
static int frm_width = 8;

static struct timeval tv0;

static XRectangle cpumon_rect;

static void layout(void);
static void draw_window(void);
static int create_window(const char *title, int width, int height);
static void proc_event(XEvent *ev);
static long get_msec(void);


int main(int argc, char **argv)
{
	XEvent ev;
	struct timeval tv;
	long prev_upd, msec, dt;

	if(sysmon_init() == -1) {
		return 1;
	}

	if(!(dpy = XOpenDisplay(0))) {
		fprintf(stderr, "failed to connect to the X server\n");
		return 1;
	}
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	xa_wm_proto = XInternAtom(dpy, "WM_PROTOCOLS", False);
	xa_wm_del = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

	if(create_window("xmon", win_width, win_height) == -1) {
		return 1;
	}

	if(!(gc = XCreateGC(dpy, win, 0, 0))) {
		fprintf(stderr, "failed to allocate GC\n");
		return 1;
	}

	if(!(font = XLoadQueryFont(dpy, FONT_DESC))) {
		fprintf(stderr, "failed to load font: %s\n", FONT_DESC);
		return 1;
	}
	XSetFont(dpy, gc, font->fid);

	if(cpumon_init() == -1) {
		return 1;
	}

	layout();

	gettimeofday(&tv0, 0);

	while(!quit) {
		fd_set rdset;
		int xfd;

		msec = get_msec();
		dt = msec - prev_upd;

		FD_ZERO(&rdset);
		xfd = ConnectionNumber(dpy);
		FD_SET(xfd, &rdset);

		if(dt >= UPD_RATE_MS) {
			tv.tv_sec = tv.tv_usec = 0;
		} else {
			long delay = UPD_RATE_MS - dt;
			tv.tv_sec = delay / 1000;
			tv.tv_usec = (delay % 1000) * 1000;
		}

		if(select(xfd + 1, &rdset, 0, 0, &tv) == -1) {
			if(errno == EINTR) continue;
			fprintf(stderr, "select failed: %s\n", strerror(errno));
			break;
		}

		if(FD_ISSET(xfd, &rdset)) {
			while(XPending(dpy)) {
				XNextEvent(dpy, &ev);
				proc_event(&ev);
			}
		}

		msec = get_msec();
		if(msec - prev_upd >= UPD_RATE_MS) {
			prev_upd = msec;
			sysmon_update();

			cpumon_update();

			draw_window();
		}
	}

	XCloseDisplay(dpy);
	return 0;
}

static void layout(void)
{
	cpumon_rect.x = frm_width;
	cpumon_rect.y = frm_width;
	cpumon_rect.width = win_width - frm_width * 2;
	if(win_height >= win_width) {
		cpumon_rect.height = cpumon_rect.width;
	} else {
		cpumon_rect.height = win_height - frm_width * 2;
	}

	cpumon_move(cpumon_rect.x, cpumon_rect.y);
	cpumon_resize(cpumon_rect.width, cpumon_rect.height);
}

static void draw_window(void)
{
	cpumon_draw();
}

static int create_window(const char *title, int width, int height)
{
	XSetWindowAttributes xattr;
	XTextProperty txprop;
	XWindowAttributes winattr;
	XVisualInfo vtmpl;
	int num_match;

	xattr.background_pixel = BlackPixel(dpy, scr);
	xattr.colormap = cmap = DefaultColormap(dpy, scr);

	if(!(win = XCreateWindow(dpy, root, 0, 0, width, height, 0, CopyFromParent,
					InputOutput, CopyFromParent, CWBackPixel | CWColormap, &xattr))) {
		fprintf(stderr, "failed to create window\n");
		return -1;
	}
	XSelectInput(dpy, win, StructureNotifyMask | ExposureMask | KeyPressMask);

	if(XStringListToTextProperty((char**)&title, 1, &txprop)) {
		XSetWMName(dpy, win, &txprop);
		XSetWMIconName(dpy, win, &txprop);
	}
	XSetWMProtocols(dpy, win, &xa_wm_del, 1);

	XGetWindowAttributes(dpy, win, &winattr);
	vtmpl.visualid = winattr.visual->visualid;
	vinf = XGetVisualInfo(dpy, VisualIDMask, &vtmpl, &num_match);

	XMapWindow(dpy, win);
	return 0;
}

static void proc_event(XEvent *ev)
{
	static int mapped;
	KeySym sym;

	switch(ev->type) {
	case Expose:
		if(!mapped || ev->xexpose.count > 0) {
			break;
		}
		draw_window();
		break;

	case MapNotify:
		mapped = 1;
		break;
	case UnmapNotify:
		mapped = 0;
		break;

	case KeyPress:
		if((sym = XLookupKeysym(&ev->xkey, 0)) != NoSymbol) {
			switch(sym) {
			case XK_Escape:
				quit = 1;
				break;

			default:
				break;
			}
		}
		break;

	case ClientMessage:
		if(ev->xclient.message_type == xa_wm_proto) {
			if(ev->xclient.data.l[0] == xa_wm_del) {
				quit = 1;
			}
		}
		break;

	case ConfigureNotify:
		if(ev->xconfigure.width != win_width || ev->xconfigure.height != win_height) {
			printf("configure notify: %dx%d\n", ev->xconfigure.width, ev->xconfigure.height);
			win_width = ev->xconfigure.width;
			win_height = ev->xconfigure.height;
			layout();
		}
		break;

	default:
		break;
	}
}

static long get_msec(void)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return (tv.tv_sec - tv0.tv_sec) * 1000 + (tv.tv_usec - tv0.tv_usec) / 1000;
}
