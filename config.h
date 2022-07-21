/* See COPYING file for copyright and license details. */
#ifndef _CONFIG_H
#define _CONFIG_H

#include <ncurses.h>

// dont touch this.
#define WIN_AMOUNT 4
#define TEXT_CAP 500
#define LANG_CAP TEXT_CAP

#define TEXTBOX_COLOR 1
#define LANGBOX_COLOR 2
#define BACKGROUND_COLOR 3

/*
 * STDSCR_PADDING	-> The padding of all elements to the border of stdscr.
 * TEXT_TEXT_PADDING	-> The padding between textboxes.
 * TEXT_LANG_PADDING	-> The padding between textboxes and language input boxes.
 * TEXT_TO_LANG_RATIO	-> The ratio between the textboxes' size and the language input boxes' size.
 */

const int STDSCR_PADDING	= 1;
const int TEXT_TEXT_PADDING	= 1;
const int TEXT_LANG_PADDING	= 1;
const int TEXTBOX_INNER_PADDING	= 1;
const int LANGBOX_INNER_PADDING	= 1;
const double TEXTH_TO_LANGH_RATIO= 4.0;

/* Colors */
void quicktran_init_colors()
{
/*                     ID		Foreground	Background */
	init_pair(TEXTBOX_COLOR,	COLOR_WHITE,	COLOR_BLACK);
	init_pair(LANGBOX_COLOR,	COLOR_RED,	COLOR_BLACK);
	init_pair(BACKGROUND_COLOR,	COLOR_WHITE,	COLOR_BLACK);
}

#endif
