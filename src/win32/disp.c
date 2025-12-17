#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <windows.h>
#include "disp.h"
#include "xmon.h"
#include "options.h"

struct image_data {
	int foo;
};


int quit;

int win_x, win_y, win_width, win_height, win_visible = 1;

struct font font;

static HINSTANCE hinst;
static HWND win;
static HDC hdc;
static unsigned int wstyle;

int main(int argc, char **argv);

static LRESULT CALLBACK handle_event(HWND win, unsigned int msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE _hinst, HINSTANCE prev, char *cmdline, int cmdshow)
{
	char **argv, *ptr;
	int i, argc = 1;

	hinst = _hinst;

	ptr = cmdline;
	while(*ptr) {
		while(*ptr && isspace(*ptr)) ptr++;
		if(*ptr && !isspace(*ptr)) {
			argc++;
			while(*ptr && !isspace(*ptr)) ptr++;
		}
	}

	if(!(argv = malloc(argc * sizeof *argv))) {
		MessageBox(0, "Failed to allocate command line array", "Fatal", MB_OK);
		return 1;
	}
	argv[0] = "xmon";

	for(i=1; i<argc; i++) {
		while(*cmdline && isspace(*cmdline)) cmdline++;
		ptr = cmdline;
		while(*ptr && !isspace(*ptr)) ptr++;
		*ptr = 0;
		argv[i] = cmdline;
		cmdline = ptr + 1;
	}

	return main(argc, argv);
}

int init_disp(void)
{
	WNDCLASSEX wc = {0};
	TEXTMETRIC tm;
	RECT rect;

	hinst = GetModuleHandle(0);

	wc.cbSize = sizeof wc;
	wc.lpszClassName = "xmon";
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = handle_event;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	if(!RegisterClassEx(&wc)) {
		MessageBox(0, "failed to register window class", "Fatal", MB_OK);
		return -1;
	}

	if(opt.vis.decor) {
		wstyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
	} else {
		wstyle = WS_POPUP;
	}

	if(!(win = CreateWindow("xmon", "xmon", wstyle, opt.x, opt.y, opt.xsz,
					opt.ysz, 0, 0, hinst, 0))) {
		MessageBox(0, "failed to create window", "Fatal", MB_OK);
		return -1;
	}
	hdc = GetDC(win);

	SelectObject(hdc, GetStockObject(NULL_PEN));
	SetBkColor(hdc, GetSysColor(COLOR_WINDOW));

	GetTextMetrics(hdc, &tm);
	font.height = tm.tmHeight;
	font.ascent = tm.tmAscent;
	font.descent = tm.tmDescent;

	GetClientRect(win, &rect);
	win_x = rect.left;
	win_y = rect.top;
	win_width = rect.right - rect.left;
	win_height = rect.bottom - rect.top;

	return 0;
}

void shutdown_disp(void)
{
	CloseWindow(win);
	UnregisterClass("xmon", hinst);
}


int proc_events(long delay)
{
	MSG msg;

	MsgWaitForMultipleObjects(0, 0, 0, delay, QS_ALLEVENTS);

	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_QUIT) return -1;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void resize_window(int x, int y)
{
	RECT rect;
	GetClientRect(win, &rect);
	rect.right = rect.left + x;
	rect.bottom = rect.top + y;
	AdjustWindowRect(&rect, wstyle, 0);
	x = rect.right - rect.left;
	y = rect.bottom - rect.top;

	SetWindowPos(win, 0, 0, 0, x, y, SWP_NOMOVE | SWP_NOOWNERZORDER |
			SWP_NOZORDER | SWP_NOACTIVATE);
}

void map_window(void)
{
	ShowWindow(win, 1);
	UpdateWindow(win);
}

unsigned int alloc_color(unsigned int r, unsigned int g, unsigned int b)
{
	return 0;
}

void set_color(unsigned int color)
{
}

void set_background(unsigned int color)
{
}

void clear_window(void)
{
}

void draw_rect(int x, int y, int width, int height)
{
	Rectangle(hdc, x, y, x + width + 1, y + height + 1);
}

void draw_rects(struct rect *rects, int count)
{
	int i, x, y;
	for(i=0; i<count; i++) {
		x = rects->x;
		y = rects->y;
		Rectangle(hdc, x, y, x + rects->width + 1, y + rects->height + 1);
		rects++;
	}
}

void draw_poly(struct point *v, int count)
{
	Polygon(hdc, (POINT*)v, count);
}

void draw_text(int x, int y, const char *str)
{
	y -= font.ascent;
	TextOut(hdc, x, y, str, strlen(str));
}

void begin_drawing(void)
{
}

void end_drawing(void)
{
	ValidateRect(win, 0);
}

struct image *create_image(unsigned int width, unsigned int height)
{
	return 0;
}

void free_image(struct image *img)
{
}

void blit_image(struct image *img, int x, int y)
{
}

void blit_subimage(struct image *img, int dx, int dy, int sx, int sy,
		unsigned int width, unsigned int height)
{
}

static LRESULT CALLBACK handle_event(HWND win, unsigned int msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		ValidateRect(win, 0);
		draw_window(UI_FRAME | ui_active_widgets);
		break;

	case WM_MOVE:
		win_x = LOWORD(lparam);
		win_y = HIWORD(lparam);
		break;

	case WM_SIZE:
		if(wparam == SIZE_MINIMIZED || wparam == SIZE_MAXHIDE) {
			win_visible = 0;
		} else {
			win_visible = 1;
			win_width = LOWORD(lparam);
			win_height = HIWORD(lparam);
		}
		break;

	case WM_KEYDOWN:
		if(wparam == VK_ESCAPE) {
			quit = 1;
		}
		break;

	default:
		return DefWindowProc(win, msg, wparam, lparam);
	}
	return 0;
}
