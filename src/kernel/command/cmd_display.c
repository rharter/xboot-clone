/*
 * kernel/command/cmd_display.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <shell/command.h>
#include <fs/fsapi.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbpixel.h>
#include <fb/fbscale.h>
#include <fb/fbviewport.h>
#include <fb/graphic.h>


#if	defined(CONFIG_COMMAND_DISPLAY) && (CONFIG_COMMAND_DISPLAY > 0)

static x_s32 display(x_s32 argc, const x_s8 **argv)
{
	struct stat st;
	struct fb * fb;
	struct bitmap *bitmap, *scaled;
	struct rect old, new;
	x_u32 width, height;

	if(argc != 3)
	{
		printk("usage:\r\n    display <dev> <file>\r\n");
		return -1;
	}

	fb = search_framebuffer((char *)argv[1]);
	if(!fb)
	{
		printk("display: '%s' does not a valid framebuffer\r\n", (char *)argv[1]);
		return -1;
	}

	if(stat((const char *)argv[2], &st) != 0)
	{
		printk("display: cannot access %s: No such file or directory\r\n", (char *)argv[2]);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printk("display: the file %s is a directory\r\n", (char *)argv[2]);
		return -1;
	}

	if(bitmap_load(&bitmap, (char *)argv[2]) != TRUE)
	{
		printk("display: cannot load file %s\r\n", (char *)argv[2]);
		return -1;
	}

	width = fb->info->bitmap.info.width;
	height = fb->info->bitmap.info.height;

	if(bitmap_create_scaled(&scaled, width, height, bitmap, BITMAP_SCALE_METHOD_BEST) != TRUE)
	{
		printk("display: cannot scale the bitmap\r\n");
		bitmap_destroy(bitmap);
		return -1;
	}

	rect_set(&new, 0, 0, width, height);
	fb_get_viewport(fb, &old);
	fb_set_viewport(fb, &new);
	fb_blit_bitmap(fb, scaled, BLIT_MODE_REPLACE, 0, 0, width, height, 0, 0);
	fb_set_viewport(fb, &old);

	bitmap_destroy(bitmap);
	bitmap_destroy(scaled);

	return 0;
}

static struct command display_cmd = {
	.name		= "display",
	.func		= display,
	.desc		= "display a picture on the screen\r\n",
	.usage		= "display <dev> <file>\r\n",
	.help		= "    show images on the screen\r\n"
};

static __init void display_cmd_init(void)
{
	if(!command_register(&display_cmd))
		LOG_E("register 'display' command fail");
}

static __exit void display_cmd_exit(void)
{
	if(!command_unregister(&display_cmd))
		LOG_E("unregister 'display' command fail");
}

module_init(display_cmd_init, LEVEL_COMMAND);
module_exit(display_cmd_exit, LEVEL_COMMAND);

#endif