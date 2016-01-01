#include "scancodes.h"

u8 shift_down = 0;

void keyboard_handler()
{
	u8 sc = inb(0x60);
	if(sc)
	{
		if(sc & (0x80))
		{
			sc &= 0x7f;
			if(scancodes[sc] == LSHIFT_KEY || scancodes[sc] == RSHIFT_KEY)
				shift_down = 0;
		}
		else
		{
			if(scancodes[sc] == LSHIFT_KEY || scancodes[sc] == RSHIFT_KEY)
			{
				shift_down = 1;
			}
			else
			{
				s8 input;
				input = (shift_down ? scancodes_shift[sc] : scancodes[sc]);
				if(input > 0)
					putch(input);
			}
		}
	}
	outb(0x20, 0x20);
}
