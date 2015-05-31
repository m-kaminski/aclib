/******
 *
 * helper header for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski. 
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#ifndef __SPECIAL_KEYS
#define __SPECIAL_KEYS
//Alt+letter or Esc,than letter
static const uint32_t     altMask = 0x00400000;

//Control+letter
static const uint32_t controlMask = 0x00200000;

//!special key pressed
static const uint32_t specialMask = 0x56000000;

//basic key mask
static const uint32_t basicKeyMask= 0x0000007f;

/*!
 * Special ascii keys as teletype mnemonics.
 */
enum ASCII
{
	LF   = 0xa,
	CR   = 0xd,
	EoF  = 0x4
};

/*!
 * Special key names. May be used for comparizons against key object
 * (please refer to handbook for use example)
 *
 */
enum Special
{
	Escape  = 0x1b,
	Tab     = 0x561f0000, // on most systems: shift+tab
	BackTab,
	F1,
	F2,
	F3,
	F4,

	F5,
	F6,
	F7,
	F8,

	F9,
	F10,
	F11,
	F12,
//shifted
	F13,
	F14,
	F15,
	F16,

	F17,
	F18,
	F19,
	F20,

	F21,
	F22,
	F23,
	F24,
//??
	F25,
	F26,
	F27,
	F28,

	F29,
	F30,
	F31,
	F32,

	F33,
	F34,
	F35,
	F36,

	Up,
	Down,
	Right,
	Left,

	CtrlUp,
	CtrlDown,
	CtrlRight,
	CtrlLeft,

	Enter,
	Delete,
	Backspace,
	CtrlBackspace,

	CtrlW,
	CtrlSpace,

	Insert,
	Home,
	PageUp,
	PageDown,
	End
};

/**
 * Helper function for debugging
 */
const char *get_key_name(int key);


void putc_utf8(int c);
void trans_utf8(char *s8, int *s32);

/**
 * Reads key from terminal;
 * returns either a 21-bit unicode character (single key)
 * or Special Key (one of abovementioned)
 */
int decode_key_pressed();

/**
 * returns true if key is one of above-defined special keys.
 */
bool is_special(int key);
#endif
