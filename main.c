#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include "config.h"

#define WIN_AMOUNT 4
#define CONTENT_CAP 500

int main()
{
	initscr();
	noecho();
	start_color();
	init_pair(1, COLOR_MAGENTA, COLOR_WHITE);

	const WINDOW *main = subwin(stdscr, LINES - STDSCR_PADDING*2, COLS - STDSCR_PADDING*2, STDSCR_PADDING, STDSCR_PADDING);

	int rows, cols;
	getmaxyx(main, rows, cols);

	const int available_hspace = cols - TEXT_TEXT_PADDING;
	const int available_vspace = rows - TEXT_LANG_PADDING;

	// TODO: Turn all this horrible math into functions.

	const int windows_width	= available_hspace / 2;

	/*
	 * We know that `available_vspace = textbox_height + langbox_height`
	 * And also that `textbox_height = langbox_height * TEXT_TO_LANG_RATIO`
	 * Hence: `available_vspace = langbox_height * TEXT_TO_LANG_RATIO + langbox_height`
	 * or: `available_vspace = langbox_height * (TEXT_TO_LANG_RATIO + 1)`
	 * In other words: `langbox_height = available_vspace / (TEXT_TO_LANG_RATIO + 1)`
	 */

	const int langbox_height	= available_vspace / (TEXT_TO_LANG_RATIO + 1);
	const int textbox_height	= TEXT_TO_LANG_RATIO * langbox_height;

	const int main_origin		= STDSCR_PADDING;
	const int dest_boxes_col	= STDSCR_PADDING + windows_width + TEXT_TEXT_PADDING;
	const int textboxes_row		= STDSCR_PADDING + langbox_height + TEXT_LANG_PADDING;

	WINDOW *src_langbox	= newwin(langbox_height, windows_width, 
					main_origin, main_origin);
	WINDOW *dest_langbox	= newwin(langbox_height, windows_width, 
					main_origin, dest_boxes_col);
	WINDOW *src_textbox	= newwin(textbox_height, windows_width, 
					textboxes_row, main_origin);
	WINDOW *dest_textbox	= newwin(textbox_height, windows_width,
					textboxes_row, dest_boxes_col);

	WINDOW *wins[WIN_AMOUNT] = { src_langbox, src_textbox, dest_langbox, dest_textbox };

	/* Variable pointing to some of the previous windows. */
	int focused_window_idx = 0;
	WINDOW *focused_window = wins[0];

	// TODO: turn this kind of code into a foreach with vararg functions (Maybe?)
	
	for (int i = 0; i < WIN_AMOUNT; i++) {
		keypad(wins[i], TRUE);
		wbkgd(wins[i], COLOR_PAIR(1));
		wnoutrefresh(wins[i]);
	}
	doupdate();

	int ch;
	int y, x;

	while ((ch = wgetch(focused_window)) != '~') {
		switch (ch) {
		case '\t':
			focused_window_idx = (focused_window_idx + 1) % WIN_AMOUNT;
			focused_window = wins[focused_window_idx];
			break;
		case KEY_BACKSPACE:
			getyx(focused_window, y, x);
			mvwdelch(focused_window, y, x-1);
			break;
		default:
			waddch(focused_window, ch);
		}
		
		// TODO: Put this into a function (Maybe?)
		for (int i = 0; i < WIN_AMOUNT; i++) {
			wnoutrefresh(wins[i]);
		}
		doupdate();
	}

	endwin();
}
