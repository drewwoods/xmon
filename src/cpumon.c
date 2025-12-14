#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmon.h"
#include "options.h"

#ifdef BUILD_WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#define GRAD_COLORS	4

static struct color *rgbcolors;
static unsigned int *colors;
static int rshift, gshift, bshift;
static struct rect rect, view_rect, lb_rect;
static struct image *img;

struct color grad[GRAD_COLORS] = {
	{5, 12, 80},
	{128, 8, 4},
	{255, 128, 64},
	{255, 250, 220}
};

static int resize_framebuf(unsigned int width, unsigned int height);
static int mask_to_shift(unsigned int mask);

int cpumon_init(void)
{
	int i;

	if(opt.cpu.ncolors <= 2) {
		return -1;
	}
	if(!(colors = malloc(opt.cpu.ncolors * (sizeof *colors + sizeof *rgbcolors)))) {
		fprintf(stderr, "CPU: failed to allocate array of %d colors\n", opt.cpu.ncolors);
		return -1;
	}
	rgbcolors = (struct color*)(colors + opt.cpu.ncolors);

	for(i=0; i<opt.cpu.ncolors; i++) {
		int seg = i * (GRAD_COLORS - 1) / opt.cpu.ncolors;
		int t = i * (GRAD_COLORS - 1) % opt.cpu.ncolors;
		struct color *c0 = grad + seg;
		struct color *c1 = c0 + 1;

		rgbcolors[i].r = c0->r + (c1->r - c0->r) * t / (opt.cpu.ncolors - 1);
		rgbcolors[i].g = c0->g + (c1->g - c0->g) * t / (opt.cpu.ncolors - 1);
		rgbcolors[i].b = c0->b + (c1->b - c0->b) * t / (opt.cpu.ncolors - 1);
		colors[i] = alloc_color(rgbcolors[i].r, rgbcolors[i].g, rgbcolors[i].b);
	}

	return 0;
}

void cpumon_destroy(void)
{
	free_image(img);
}

void cpumon_move(int x, int y)
{
	rect.x = x;
	rect.y = y;

	view_rect.x = rect.x + BEVEL;
	view_rect.y = rect.y + BEVEL + font.height;

	lb_rect.x = view_rect.x;
	lb_rect.y = view_rect.y - BEVEL - font.height - 1;
}

void cpumon_resize(int x, int y)
{
	rect.width = x;
	rect.height = y;

	view_rect.width = x - BEVEL * 2;
	view_rect.height = y - BEVEL * 2 - font.height;

	lb_rect.width = view_rect.width;
	lb_rect.height = font.height;

	resize_framebuf(view_rect.width, view_rect.height);
}

int cpumon_height(int w)
{
	int h = w;
	int min_h = font.height + 2 * BEVEL + 8;
	return h < min_h ? min_h : h;
}

void cpumon_update(void)
{
	unsigned char *fb, *row;
	unsigned int *row32;
	int *cpucol;
	int cpunum, dcpu, col0;
	unsigned int i, row_offs, cur, x;

	if(!img) return;

	fb = (unsigned char*)img->pixels;

	cpucol = alloca(smon.num_cpus * sizeof *cpucol);

	for(i=0; i<smon.num_cpus; i++) {
		int usage = smon.cpu[i];
		if(usage >= 128) usage = 127;
		cpucol[i] = (usage * opt.cpu.ncolors) >> 7;
	}

	row_offs = (img->height - 1) * img->pitch;

	/* scroll up */
	memmove(fb, fb + img->pitch, row_offs);

	/* draw the bottom line with the current stats */
	row = fb + row_offs;

	switch(img->bpp) {
	case 8:
		for(i=0; i<img->width; i++) {
			cur = i * smon.num_cpus / img->width;
			col0 = cpucol[cur];
			*row++ = colors[col0];
		}
		break;

	case 32:
		row32 = (unsigned int*)row;
		for(i=0; i<img->width; i++) {
			struct color *rgb;
			cur = i * smon.num_cpus / img->width;
			col0 = cpucol[cur];
			rgb = rgbcolors + col0;

			*row32++ = (rgb->r << rshift) | (rgb->g << gshift) | (rgb->b << bshift);
		}
		break;

	default:
		break;
	}
}

void cpumon_draw(void)
{
	int baseline;
	char buf[128];

	draw_frame(view_rect.x - BEVEL, view_rect.y - BEVEL, view_rect.width + BEVEL * 2,
			view_rect.height + BEVEL * 2, -BEVEL);

	set_color(uicolor[COL_BG]);
	draw_rect(lb_rect.x, lb_rect.y, lb_rect.width, lb_rect.height);

	baseline = lb_rect.y + lb_rect.height - font.descent;
	sprintf(buf, "CPU %3d%%", smon.single * 100 >> 7);
	set_color(uicolor[COL_FG]);
	draw_text(lb_rect.x, baseline, buf);

	if(!img) return;

	blit_image(img, view_rect.x, view_rect.y);
}

static int resize_framebuf(unsigned int width, unsigned int height)
{
	if(img && width == img->width && height == img->height) {
		return 0;
	}

	free_image(img);

	if(width <= 0 || height <= 0) {
		return -1;
	}

	if(!(img = create_image(width, height))) {
		return -1;
	}

	rshift = mask_to_shift(img->rmask);
	gshift = mask_to_shift(img->gmask);
	bshift = mask_to_shift(img->bmask);
	return 0;
}

static int mask_to_shift(unsigned int mask)
{
	int i;
	for(i=0; i<32; i++) {
		if(mask & 1) return i;
		mask >>= 1;
	}
	return 0;
}
