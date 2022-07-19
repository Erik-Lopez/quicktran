#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include "config.h"

#define WIN_AMOUNT 4
#define TEXT_CAP 6
#define LANG_CAP 6

void show_debug_win(WINDOW *debug_window, chtype *text)
{
	waddchstr(debug_window, text);
	wrefresh(debug_window);
	wgetch(debug_window);
}

void show_error_win(WINDOW *debug_window, chtype *text)
{
	show_debug_win(debug_window, text);
	endwin();
	exit(1);
}

int main()
{
	initscr();
	noecho();
	start_color();
	init_pair(1, COLOR_MAGENTA, COLOR_WHITE);
	WINDOW *debug_window = newwin(0, 0, 0, 0);

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
	WINDOW *focused_window = wins[focused_window_idx];

	// TODO: turn this kind of code into a foreach with vararg functions (Maybe?)
	
	for (int i = 0; i < WIN_AMOUNT; i++) {
		keypad(wins[i], TRUE);
		wbkgd(wins[i], COLOR_PAIR(1));
		wnoutrefresh(wins[i]);
	}
	doupdate();

	chtype ch;
	int y, x;
	//int last_item_idx = 0;		// src_lang[last_item_idx + 1] should *always* be 0.
	// all `last_item_idx` has been replaced with `last_char_indices[focused_window_idx]`
	
	// TODO: we ABSOLUTELY need a Box struct with a last_char_idx, a buffer and a window
	chtype src_lang[LANG_CAP] = {0};    // ideally should be char*, but let's ignore that
	int last_src_lang_char_index = 0;
	chtype dest_lang[LANG_CAP] = {0};   // also should be char*
	int last_dest_lang_char_index = 0;
	chtype src_text[TEXT_CAP] = {0};
	int last_src_text_char_index = 0;
	chtype dest_text[TEXT_CAP] = {0};
	int last_dest_text_char_index = 0;

	// Same order as the windows arrays
	int last_char_indices[WIN_AMOUNT] = { last_src_lang_char_index, last_src_text_char_index, last_dest_lang_char_index, last_dest_text_char_index};

	// Same order as wins[] array.
	chtype *contents[WIN_AMOUNT] = { src_lang, src_text, dest_lang, dest_text };

	while ((ch = wgetch(focused_window)) != '~') {
		switch (ch) {
		case '\t':
			focused_window_idx = (focused_window_idx + 1) % WIN_AMOUNT;
			focused_window = wins[focused_window_idx];
			break;
		case KEY_BACKSPACE:
			// TODO: put into a function
			//last_item_idx--;
			last_char_indices[focused_window_idx]--;
			contents[focused_window_idx][last_char_indices[focused_window_idx]] = 0;
			getyx(focused_window, y, x);
			mvwdelch(focused_window, y, x-1);
			break;
		default:
			// TODO: solve the mess between char*, int* and wchar_t* 
			// Maybe done ?

			// TODO: put into a function
			if (last_char_indices[focused_window_idx] + 2 == TEXT_CAP) {
				show_debug_win(debug_window, src_text);
				show_debug_win(debug_window, dest_text);
				show_debug_win(debug_window, src_lang);
				show_error_win(debug_window, dest_lang);
				continue;
			}

			contents[focused_window_idx][last_char_indices[focused_window_idx]] = ch;
			waddch(focused_window, ch);
			last_char_indices[focused_window_idx]++;
		}
		
		// TODO: Put this into a function (Maybe?)
		for (int i = 0; i < WIN_AMOUNT; i++) {
			wnoutrefresh(wins[i]);
		}
		doupdate();
	}

	endwin();
}
