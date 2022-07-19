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


NCURSES_BOOL box_isfull(struct Box *box)
{
	return box->last_element + 2 == box->content + TEXT_CAP;
}

NCURSES_BOOL box_isempty(struct Box *box)
{
	return box->last_element == box->content;
}


void box_delchtype(struct Box *box)
{
	if (box_isempty(box)) {
		endwin();
		exit(1);
	}

	box->last_element--;
	*(box->last_element) = 0;

	int y, x;
	getyx(box->window, y, x);
	mvwdelch(box->window, y, x-1);
}

void box_addchtype(struct Box *box, chtype ch)
{
	if (box_isfull(box) == TRUE) {
		endwin();
		exit(1);
	}

	*(box->last_element) = ch;
	box->last_element++;
	waddch(box->window, ch);
}

void box_refreshwins(struct Box **boxes)
{
	for (int i = 0; i < WIN_AMOUNT; i++)
		wnoutrefresh(boxes[i]->window);

	doupdate();
}

void change_focused_box(struct Box **boxes, struct Box *focused_box, int *focused_box_idx)
{
	*focused_box_idx = (*focused_box_idx + 1) % WIN_AMOUNT;
	focused_box = boxes[*focused_box_idx];
}

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

	while ((ch = wgetch(focused_box->window)) != '~') {
		switch (ch) {
		case '\t':
			change_focused_box(&boxes, focused_box, &focused_box_idx);
			break;
		case KEY_BACKSPACE:
			box_delchtype(focused_box);
			break;
		default:
			box_addchtype(focused_box, ch);
		}
		
		box_refreshwins(boxes);
	}

	endwin();
}
