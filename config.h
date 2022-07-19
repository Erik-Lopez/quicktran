#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * STDSCR_PADDING	-> The padding of all elements to the border of stdscr.
 * TEXT_TEXT_PADDING	-> The padding between textboxes.
 * TEXT_LANG_PADDING	-> The padding between textboxes and language input boxes.
 * TEXT_TO_LANG_RATIO	-> The ratio between the textboxes' size and the language input boxes' size.
 */

const int STDSCR_PADDING	= 1;
const int TEXT_TEXT_PADDING	= 1;
const int TEXT_LANG_PADDING	= 1;
const double TEXT_TO_LANG_RATIO	= 4.0;

// dont touch this.
#define WIN_AMOUNT 4
#define TEXT_CAP 500
#define LANG_CAP TEXT_CAP

#endif
