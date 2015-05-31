/******
 *
 * helper library source file for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski. 
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "vt100codes.h"
#include "special_keys.h"



#define KEYD(x) case x: return # x
const char *get_key_name(int key)
{
	switch (key)
	{
		KEYD(Enter);
		KEYD(Tab);
		KEYD(Escape);
		KEYD(Delete);
		KEYD(Backspace);
		KEYD(CtrlBackspace);

		KEYD(F1);
		KEYD(F2);
		KEYD(F3);
		KEYD(F4);

		KEYD(F5);
		KEYD(F6);
		KEYD(F7);
		KEYD(F8);

		KEYD(F9);
		KEYD(F10);
		KEYD(F11);
		KEYD(F12);

	case Left: return "\342\206\220";
	case Up: return "\342\206\221";
	case Right: return "\342\206\222";
	case Down: return "\342\206\223";

		KEYD(CtrlUp);
		KEYD(CtrlDown);
		KEYD(CtrlRight);
		KEYD(CtrlLeft);

		KEYD(Insert);
		KEYD(Home);
		KEYD(PageUp);
		KEYD(PageDown);
		KEYD(End);

	default:
		return "unknown";
	}
}

/*simple macros for enabling and disabling utf-8 validation */
#ifdef DO_VALIDATE_UTF_8_OUTPUT
# define VALIDATE_TRAILING(x)            \
	if ( (c[x]&0xC0)!=0x80)          \
			return -1;
#else
# define VALIDATE_TRAILING(x)
#endif /* DO_VALIDATE_UTF_8_OUTPUT */

/**
 * helper function for decode_key_pressed. Parses utf8 into 32-bit unicode
 */
int decode_regular_key_pressed()
{
	int result;
	unsigned char c[4];
	c[0] = getc(stdin);

	if ( (c[0] & 0xF8) == 0xF0) /* 4 byte char */
	{

		c[1] = getc(stdin);
		VALIDATE_TRAILING(1); /* only some values for trailing bytes
				       * are valid */

		c[2] = getc(stdin);
		VALIDATE_TRAILING(2);

		c[3] = getc(stdin);
		VALIDATE_TRAILING(3);
		c[0] &= 0x0F;
		c[1] &= 0x7F;
		c[2] &= 0x7F;
		c[3] &= 0x7F;
		result = c[3]+(((uint32_t)c[2])<<6)+(((uint32_t)c[1])<<12)
			+(((uint32_t)c[0])<<18);

		if (result < (1<<16)) {
			/* overlong UTF8 encoding FORBIDDEN
			 * according to RFC 3629 */
			return -1;
		}
	} else if ((c[0] & 0xF0) == 0xE0) /* 3 byte char */ {

		c[1] = getc(stdin);
		VALIDATE_TRAILING(1);

		c[2] = getc(stdin);
		VALIDATE_TRAILING(2);
		c[0] &= 0x1F;
		c[1] &= 0x7F;
		c[2] &= 0x7F;
		result = c[2]+(((uint32_t)c[1])<<6)+(((uint32_t)c[0])<<12);

		if (result < (1<<11)) {
			/* overlong UTF8 encoding FORBIDDEN
			 * according to RFC 3629 */
			return -1;
		}
	} else if ((c[0] & 0xE0) == 0xC0) /* 2 byte char */ {

		c[1] = getc(stdin);
		VALIDATE_TRAILING(1);

		c[0] &= 0x3F;
		c[1] &= 0x7F;

		result = c[1]+(((uint32_t)c[0])<<6);
		if (result < (1<<7)) {
			/* overlong UTF8 encoding FORBIDDEN
			 * according to RFC 3629 */
			return -1;
		}
	} else {
		if ((c[0] & 0x80) == 0) {
			/* 1 byte char */
			result=c[0];
		} else {
			/* invalid first byte of UTF encoding */
			return -1;
		}
	}
	return result;
}

void putc_utf8(int c)
{
	if (c<(1<<7)) /* 7 bit Unicode encoded as plain ascii */ {
		putc(c, stdout);
		return;
	}
	if (c<(1<<11)) /* 11 bit Unicode encoded in 2 UTF-8 bytes */ {
		putc((c>>6)|0xC0, stdout);
		putc((c&0x3F)|0x80, stdout);
		return;
	}
	if (c<(1<<16)) /* 16 bit Unicode encoded in 3 UTF-8 bytes */ {
		putc(((c>>12))|0xE0, stdout);
		putc(((c>>6)&0x3F)|0x80, stdout);
		putc((c&0x3F)|0x80, stdout);
		return;
	}
	if (c<(1<<21))/* 21 bit Unicode encoded in 4 UTF-8 bytes */ {
		putc(((c>>18))|0xF0, stdout);
		putc(((c>>12)&0x3F)|0x80, stdout);
		putc(((c>>6)&0x3F)|0x80, stdout);
		putc((c&0x3F)|0x80, stdout);
		return;
	}
}



void trans_utf8(char *s8, int *s32)
{
	while (1) {
		if (!*s32) {
			*s8 = 0;
			break;
		}

		int c = *s32++;
		if (c<(1<<7))/* 7 bit Unicode encoded as plain ascii */ {
			(*s8++) = (c);
			continue;
		}

		if (c<(1<<11))/* 11 bit Unicode encoded in 2 UTF-8 bytes */ {
			(*s8++) = ((c>>6)|0xC0);
			(*s8++) = ((c&0x3F)|0x80);
			continue;
		}

		if (c<(1<<16))/* 16 bit Unicode encoded in 3 UTF-8 bytes */ {
			(*s8++) = (((c>>12))|0xE0);
			(*s8++) = (((c>>6)&0x3F)|0x80);
			(*s8++) = ((c&0x3F)|0x80);
			continue;
		}

		if (c<(1<<21))/* 21 bit Unicode encoded in 4 UTF-8 bytes */ {
			(*s8++) = (((c>>18))|0xF0);
			(*s8++) = (((c>>12)&0x3F)|0x80);
			(*s8++) = (((c>>6)&0x3F)|0x80);
			(*s8++) = ((c&0x3F)|0x80);
			continue;
		}
	}
}


int decode_key_pressed()
{
	int c = getc(stdin);
	int d;
	int c_tmp;
	if (c == LF)
		return Enter;
	if (c == '\t')
		return Tab;
	if (c == 127)
		return Backspace;
	if (c == 8)
		return CtrlBackspace;
	if (c == EOF || c == -1) {
		return EoF;
	}
	if (c ==23) { /* ETB or Ctrl+W */
		return CtrlW;
	}
	if (c == 0) {
		return CtrlSpace;
	}
	if (c!=Escape)
	{
		ungetc(c, stdin);
		return decode_regular_key_pressed();
	}
	c_tmp=c = getc(stdin);
	if (c == 'O') /* capital letter O, not digit 0 */ {
		c = getc(stdin);
		if (c<'P' || c>'S')
			return -1;/* Unrecognized character sequence - unrecognized key */
		return F1+ c-'P';
	}

	if (c == '[')
	{
		switch (getc(stdin)) 
		{
		case 'A':
			return (Up);

		case 'B':
			return (Down);

		case 'C':
			return (Right);

		case 'D':
			return (Left);

		case '1': /* 1,2,3,4,5, 7,8,9 =  F1..F8 */
			c = getc(stdin);
			d = getc(stdin);
			if (c==';' && d=='5') {
				char e = getc(stdin);
				switch(e) {
				case 'A':
					return (CtrlUp);
				case 'B':
					return (CtrlDown);
				case 'C':
					return (CtrlRight);
				case 'D':
					return (CtrlLeft);
				}
			}
			if (d != 0x7e) {
				return -1;
			}
			if (c=='6') {
				return -1;
				/* Unrecognized character sequence - unrecognized key */
			}
			if (c>'9') {
				return -1;
				/* Unrecognized character sequence - unrecognized key */
			}
			if (c>'6')c--; /* code sequence 1,2,3,4,5, 7,8,9 (note,
					* that 6 is omitted) */
			return (F1+ c-'1');

		case '2': /* 0,1,3,4 =  F9..F12; only '~' - INS */
			c = getc(stdin);
			if (c=='~')
				return (Insert);
			if (c=='2')
				return -1;/* Unrecognized character sequence - unrecognized key */
			if (c>'4')
				return -1;/* Unrecognized character sequence - unrecognized key */
			if (c>'2')
				c--;
			if (getc(stdin) != 0x7e) // != '~'
				return -1;/* Unrecognized character sequence - unrecognized key */
			return (F9+ c-'0');

		case '3': /* <ESC>[3~ - delete */
			d = getc(stdin);
			if (d != 0x7e) {
				return -1; /* unrecognized sequence */
			}
			return (Delete);

		case '5':
			d = getc(stdin);
			if (d != 0x7e) {
				return -1; /* unrecognized sequence */
			}
			return (PageUp);

		case '6':
			d = getc(stdin);
			if (d != 0x7e)
			{
				return -1; /* unrecognized sequence */
			}
			return (PageDown);
		case '7':/* alternative coding */
			d = getc(stdin);
			if (d != 0x7e)
			{
				return -1; /* unrecognized sequence */
			}
			return (Home);

		case '8':/* alternative coding */
			d = getc(stdin);
			if (d != 0x7e)
			{
				return -1; /* unrecognized sequence */
			}
			return (End);

		case 'F':
			return (End);

		case 'H':
			return (Home);


		default:
			return -1;
			/* Unrecognized character sequence - unrecognized key */
		}
	}

	//input.UnGet();
	ungetc(c_tmp,stdin);
	return (Escape);


}

bool is_special(int key)
{
	return	(key < ' ') ||
		((key & specialMask) == specialMask);
}
