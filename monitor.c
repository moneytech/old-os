/* <http://www.jamesmolloy.co.uk/tutorial_html/3.-The%20Screen.html> */
#include "monitor.h"
#include "klib.h"
#include <stdarg.h>

#define VGA_WIDTH  (80u)
#define VGA_HEIGHT (25u)

static size_t cursor_y = 0, 
              cursor_x = 0;

static monitor_color foreground = COLOR_WHITE,
                     background = COLOR_BLACK;

static uint16_t *video_memory = (uint16_t*) 0xB8000; /*vga video memory*/

static uint8_t make_color(monitor_color fg, monitor_color bg)
{
	return fg | (bg << 4u);
}
 
static uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | (color16 << 8u);
}

static void move_cursor(void) 
{
        uint16_t cursor_location = (cursor_y * VGA_WIDTH) + cursor_x;
        outb(0x3D4, 14); /* send high byte to VGA */
        outb(0x3D5, cursor_location >> 8u); /* send the cursor value */
        outb(0x3D4, 15); /* send the low byte*/
        outb(0x3D5, cursor_location); /* send the low cursor byte*/
}

static void scroll(void)
{
        uint16_t blank = make_vgaentry(' ', make_color(foreground, background));

        if(cursor_y >= VGA_HEIGHT)
        {
                size_t i;
                for(i = 0; i < ((VGA_HEIGHT-1)*VGA_WIDTH); i++)
                        video_memory[i] = video_memory[i + VGA_WIDTH];

                for(i = (VGA_HEIGHT-1)*VGA_WIDTH; i < VGA_HEIGHT*VGA_WIDTH; i++)
                        video_memory[i] = blank;
                cursor_y = VGA_HEIGHT - 1;
        }
}

void monitor_putc(char c)
{
        /*handle characters*/
        if(c == 0x08 /*backspace*/ && cursor_x) {
                cursor_x--;
        } else if(c == 0x09 /*tab*/) {
                cursor_x = (cursor_x + 8) & ~(8-1);
        } else if(c == '\r') {
                cursor_x = 0;
        } else if(c == '\n') {
                cursor_x = 0;
                cursor_y++;
        } else {
                uint16_t *location = video_memory + (cursor_y * VGA_WIDTH + cursor_x);
                *location = make_vgaentry(c, make_color(foreground, background));
                cursor_x++;
        }

        /* reached the end of the line? */
        if(cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
        }
        scroll();
        move_cursor();
}

void monitor_clear(void)
{
        size_t i;
        uint16_t blank = make_vgaentry(' ', make_color(foreground, background));
        for(i = 0; i < VGA_WIDTH*VGA_HEIGHT; i++)
                video_memory[i] = blank;
        cursor_x = 0;
        cursor_y = 0;
        move_cursor();
}

void monitor_puts(char *s)
{
        size_t i = 0;
        while(s[i])
                monitor_putc(s[i++]);
}

void monitor_printu(uint32_t d)
{
        char v[33] = "";
        ku32tostr(v, 33, d, 10);
        monitor_puts(v);
}

void monitor_printd(int32_t d)
{
        char v[34] = "";
        ks32tostr(v, 33, d, 10);
        monitor_puts(v);
}

void monitor_printf(char *fmt, ...)
{
        va_list ap;
        int32_t  a;
        uint32_t b;
        char f, c, *s;
        va_start(ap, fmt);
        while(*fmt) {
                if('%' == (f = *fmt++)) {
                        switch(f = *fmt++) {
                        case '0': goto finish;
                        case '%': monitor_putc(f);
                                  break;
                        case 'c': c = va_arg(ap, int);
                                  monitor_putc(c);
                                  break;
                        case 's': s = va_arg(ap, char*);
                                  monitor_puts(s);
                                  break;
                        case 'u': b = va_arg(ap, uint32_t);
                                  monitor_printu(b);
                                  break;
                        case 'd': a = va_arg(ap, int32_t);
                                  monitor_printd(a);
                                  break;
                        default: /*error*/
                                  goto finish;
                        }
                } else {
                        monitor_putc(f);
                }
        }
finish:
        va_end(ap);
}

void monitor_set_background_color(monitor_color color)
{
        background = color;
}

void monitor_set_foreground_color(monitor_color color)
{
        foreground = color;
}

void monitor_default_colors(void) {
        background = COLOR_BLACK;
        foreground = COLOR_WHITE;
}
