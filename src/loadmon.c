#include <stdio.h>
#include <string.h>
#include "xmon.h"
#include "options.h"

static struct rect rect;
static int bar_max;

int loadmon_init(void)
{
	bar_max = smon.num_cpus << 7;
	return 0;
}

void loadmon_move(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

void loadmon_resize(int x, int y)
{
	rect.width = x;
	rect.height = y;
}

int loadmon_height(int w)
{
	return font.height + bar_height + 2;
}

void loadmon_draw(void)
{
	char buf[128];
	int baseline, y, val;

	baseline = rect.y + font.height - font.descent - 1;

	set_color(uicolor[COL_BG]);
	draw_rect(rect.x, rect.y, rect.width, font.height);

	sprintf(buf, "LOAD %.2f", smon.loadavg[0]);

	set_color(uicolor[COL_FG]);
	draw_text(rect.x, baseline, buf);

	y = baseline + font.descent + 1 + BEVEL;
	val = (int)(smon.loadavg[0] * 1024.0);
	draw_bar(rect.x, y, rect.width, val, bar_max);
}
