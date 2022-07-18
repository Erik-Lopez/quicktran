#include <ncurses.h>
#include <stdlib.h>
#include "config.h"

#define WIN_AMOUNT 4

int main()
{
	initscr();

	WINDOW *main = subwin(stdscr, LINES - STDSCR_PADDING*2, COLS - STDSCR_PADDING*2, STDSCR_PADDING, STDSCR_PADDING);

	int rows, cols;
	getmaxyx(main, rows, cols);

	int available_hspace = cols - TEXT_TEXT_PADDING;
	int available_vspace = rows - TEXT_LANG_PADDING;

	// TODO: Turn all this horrible math into functions.

	int windows_width	= available_hspace / 2;

	/*
	 * We know that `available_vspace = textbox_height + langbox_height`
	 * And also that `textbox_height = langbox_height * TEXT_TO_LANG_RATIO`
	 * Hence: `available_vspace = langbox_height * TEXT_TO_LANG_RATIO + langbox_height`
	 * or: `available_vspace = langbox_height * (TEXT_TO_LANG_RATIO + 1)`
	 * In other words: `langbox_height = available_vspace / (TEXT_TO_LANG_RATIO + 1)`
	 */

	int langbox_height	= available_vspace / (TEXT_TO_LANG_RATIO + 1);
	int textbox_height	= TEXT_TO_LANG_RATIO * langbox_height;

	WINDOW *src_langbox	= newwin(langbox_height, windows_width, 
					STDSCR_PADDING, STDSCR_PADDING);
	WINDOW *dest_langbox	= newwin(langbox_height, windows_width, 
					STDSCR_PADDING, 
					STDSCR_PADDING + windows_width + TEXT_TEXT_PADDING);

	WINDOW *src_textbox	= newwin(textbox_height, windows_width, 
					STDSCR_PADDING + langbox_height + TEXT_LANG_PADDING,
					STDSCR_PADDING);
	WINDOW *dest_textbox	= newwin(textbox_height, windows_width,
					STDSCR_PADDING + langbox_height + TEXT_LANG_PADDING,
					STDSCR_PADDING + windows_width + TEXT_TEXT_PADDING);

	WINDOW *wins[WIN_AMOUNT] = { src_langbox, src_textbox, dest_langbox, dest_textbox };

	start_color();
	init_pair(1, COLOR_MAGENTA, COLOR_WHITE);

	for (int i = 0; i < WIN_AMOUNT; i++) {
		// TODO: panic() function.
		if (wins[i] == NULL)
			exit(1);

		wbkgd(wins[i], COLOR_PAIR(1));
		wprintw(wins[i], "Window %d\n", i);
		wnoutrefresh(wins[i]);
	}
	doupdate();

	wgetch(wins[0]);
	endwin();
}
