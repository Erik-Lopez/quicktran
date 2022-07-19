#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include "config.h"

#define WIN_AMOUNT 4
#define TEXT_CAP 12
#define LANG_CAP 12

struct Box
{
	WINDOW *window;
	chtype content[TEXT_CAP];
	chtype *last_element;
};

void create_box(struct Box *b, WINDOW *window)
{
	b->window = window;

	for (int i = 0; i < TEXT_CAP; i++)
		b->content[i] = 0;

	b->last_element = &(b->content[0]);
}

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

	struct Box src_langbox, src_textbox, dest_langbox, dest_textbox;
	create_box(&src_langbox, newwin(langbox_height, windows_width, main_origin, main_origin));
	create_box(&src_textbox, newwin(langbox_height, windows_width, main_origin, dest_boxes_col));
	create_box(&dest_langbox, newwin(textbox_height, windows_width, textboxes_row, main_origin));
	create_box(&dest_textbox, newwin(textbox_height, windows_width, textboxes_row, dest_boxes_col));

	struct Box *boxes[WIN_AMOUNT] = {&src_langbox, &src_textbox, &dest_langbox, &dest_textbox};

	int focused_box_idx	= 0;
	struct Box *focused_box	= boxes[focused_box_idx];

	// TODO: turn this kind of code into a foreach with vararg functions (Maybe?)
	
	for (int i = 0; i < WIN_AMOUNT; i++) {
		keypad(boxes[i]->window, TRUE);
		wbkgd(boxes[i]->window, COLOR_PAIR(1));
		wnoutrefresh(boxes[i]->window);
	}
	doupdate();

	chtype ch;
	int y, x;
	
	while ((ch = wgetch(focused_box->window)) != '~') {
		switch (ch) {
		case '\t':
			focused_box_idx = (focused_box_idx + 1) % WIN_AMOUNT;
			focused_box = boxes[focused_box_idx];
			break;
		case KEY_BACKSPACE:
			// TODO: put into a function
			focused_box->last_element--;
			*(focused_box->last_element) = 0;

			getyx(focused_box->window, y, x);
			mvwdelch(focused_box->window, y, x-1);
			break;
		default:
			// TODO: put into a function
			if (focused_box->last_element + 2 == focused_box->content + TEXT_CAP) {
				show_debug_win(debug_window, boxes[0]->content);
				show_debug_win(debug_window, boxes[1]->content);
				show_debug_win(debug_window, boxes[2]->content);
				show_error_win(debug_window, boxes[3]->content);
				continue;
			}

			*(focused_box->last_element) = ch;
			focused_box->last_element++;
			waddch(focused_box->window, ch);
		}
		
		// TODO: Put this into a function (Maybe?)
		for (int i = 0; i < WIN_AMOUNT; i++) {
			wnoutrefresh(boxes[i]->window);
		}
		doupdate();
	}

	endwin();
}
