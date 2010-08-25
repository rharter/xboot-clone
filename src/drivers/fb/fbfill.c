/*
 * drivers/fb/fbfill.c
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
#include <xboot.h>
#include <malloc.h>
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/graphic.h>
#include <fb/fbfill.h>

/*
 * generic filler that works for every supported mode
 */
void bitmap_fill_rect_generic(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			set_bitmap_pixel(bitmap, x + i, y + j, c);
		}
	}
}

/*
 * optimized filler for direct color 32 bit modes
 */
void bitmap_fill_rect_direct32(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u32 * p;
	x_u32 skip;
	x_u32 i, j;

	p = (x_u32 *)get_bitmap_pointer(bitmap, x, y);
	skip = bitmap->info.pitch - bitmap->info.bytes_per_pixel * w;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			*p++ = cpu_to_le32(c);
		}

		p = (x_u32 *)(((x_u8 *)p) + skip);
	}
}

/*
 * optimized filler for direct color 24 bit modes
 */
void bitmap_fill_rect_direct24(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p;
	x_u32 skip;
	x_u32 i, j;
	x_u8 fill0 = (x_u8)((c >> 0) & 0xff);
	x_u8 fill1 = (x_u8)((c >> 8) & 0xff);
	x_u8 fill2 = (x_u8)((c >> 16) & 0xff);

	p = (x_u8 *)get_bitmap_pointer(bitmap, x, y);
	skip = bitmap->info.pitch - bitmap->info.bytes_per_pixel * w;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			*p++ = fill0;
			*p++ = fill1;
			*p++ = fill2;
		}

		p += skip;
	}
}

/*
 * optimized filler for direct color 16 bit modes
 */
void bitmap_fill_rect_direct16(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p;
	x_u32 skip;
	x_u32 i, j;
	x_u8 fill0 = (x_u8)((c >> 0) & 0xff);
	x_u8 fill1 = (x_u8)((c >> 8) & 0xff);

	p = (x_u8 *)get_bitmap_pointer(bitmap, x, y);
	skip = bitmap->info.pitch - bitmap->info.bytes_per_pixel * w;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			*p++ = fill0;
			*p++ = fill1;
		}

		p += skip;
	}
}

/*
 * optimized filler for direct color 8 bit modes
 */
void bitmap_fill_rect_direct8(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p;
	x_u32 skip;
	x_u32 i, j;
	x_u8 fill = (x_u8)(c & 0xff);

	p = (x_u8 *)get_bitmap_pointer(bitmap, x, y);
	skip = bitmap->info.pitch - bitmap->info.bytes_per_pixel * w;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			*p++ = fill;
		}

		p += skip;
	}
}