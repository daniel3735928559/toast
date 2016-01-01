#include "font.h"

u32 cursor_x;
u32 cursor_y;

struct pixel
{
	u8 blue;
	u8 green;
	u8 red;
} __attribute__((packed));

typedef struct pixel pixel_t;

pixel_t *videoram;
u32 width;
u32 height;

void monitor_init(pixel_t *vidstart)
{
	videoram = vidstart;
	width = 800;
	height = 600;
	cursor_x = 0;
	cursor_y = 0;
}

void plot_pixel(u32 x, u32 y, pixel_t px)
{
	pixel_t *p = videoram + (y*width + x);
	p->red = px.red;
	p->green = px.green;
	p->blue = px.blue;
}

void put_bmp(u8 *bmp, u32 w, u32 h, u32 x, u32 y, u8 r, u8 g, u8 b)
{
	u32 bit = 0;
	pixel_t px;
	while(bit < w*h)
	{
		u32 idx = bit / 8;
		u32 off = (8 - (bit % 8)) % 8;
		if(bmp[idx] & (1 << off))
		{
			px.red = r;
			px.green = g;
			px.blue = b;
		}
		else
		{
			px.red = 0;
			px.green = 0;
			px.blue = 0;
		}
		u32 x_off = bit % w;//(w - bit % w) % w;//to flip
		u32 y_off = bit / h;
		plot_pixel(x + x_off, y + y_off, px);
		bit++;
	}
}

void cls()
{
	pixel_t *px = videoram;
	while(px < videoram + width * height)
	{
		px->red = px->green = px->blue = 0x00000000;
		px += 1;
	}
	cursor_x = 0;
	cursor_y = 0;
}

void newline()
{
	cursor_x = 0;
	cursor_y += fheight;
}

void scroll()
{
  cursor_y = 0;
  //return;
}

void putch_rgb(char c, u8 r, u8 g, u8 b)
{
	if(c == '\n')
		newline();
	else if(c == '\r')
		cursor_x = 0;
	else if(c == 8 && cursor_x)
		cursor_x -= fwidth;
	else if(c == 9 || c == '\t')
	        cursor_x = (cursor_x + 8*fwidth) - (cursor_x % (8*fwidth));
	else if(c >= 32)
	{
		put_bmp(font[c], fwidth, fheight, cursor_x, cursor_y, r, g, b);
		cursor_x += fwidth;
		if(cursor_x >= width)
			newline();
	}
	if(cursor_y + fheight >= height)
		scroll();
}

void putch(char c)
{
	putch_rgb(c, 0xff, 0xff, 0xff);
}

//char digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void print_hex_32(u32 x)
{
	char output[8];
	u8 i, len = 0;
	for(i = 0; i < 8; i++)
	{
		u8 d = x & 0x0f;
		if(d)
			len = i;
		if(d > 9)
			output[7 - i] = (55 + d);
		else
			output[7 - i] = (48 + d);
		x = x >> 4;
	}
	for(i = 7-len; i < 8; i++)
		putch(output[i]);
}

void print_dec_32(u32 x)
{
	char output[10];
	u8 i = 0;
	while(x)
	{
		output[i] = (48 + (x % 10));
		i++;
		x /= 10;
	}
	while(i > 0)
		putch(output[--i]);
}

void print_bin_32(u32 x)
{
	char output[32];
	u8 len = 0;
	u8 i = 0;
	u32 a = 1;
	while(a)
	{
		if(x & a)
		{
			output[i] = '1';
			len = i+1;
		}
		else
			output[i] = '0';
		a = a << 1;
		i++;
	}
	while(len)
		putch(output[--len]);
}

void prints(char *s)
{
	u32 i = 0;
	while(s[i])
		putch(s[i++]);
}

void bitmap_print(bitmap_t *bmap)
{
	u32 *i;
	for(i = bmap->bits; i < bmap->bits + bmap->size; i += 1)
	{
		print_bin_32(*i);
	}
	putch('\n');
}

void print_addr(void *ptr){
  print_hex_32((u32)(ptr));
}
