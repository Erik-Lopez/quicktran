#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include <locale.h>

#include "config.h"
#include "box.h"
#include "popups.h"

#define WIN_AMOUNT 4
#define TEXT_CAP 500
#define LANG_CAP 500

void translate(struct Box **boxes)
{
	// This is the order: src_langbox, src_textbox, dest_langbox, dest_textbox
	chtype *src_lang = boxes[0]->content;
	chtype *src_text = boxes[1]->content;
	chtype *dest_lang = boxes[2]->content;
	chtype *dest_text = boxes[3]->content;

	//printw("Traducir '%ls' del '%ls' al '%ls' sería '%ls'.", src_text, src_lang, dest_lang, dest_text);

	getch();
	exit(1);
}

int main()
{
	setlocale(LC_ALL, "");
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
	box_create(&src_langbox, newwin(langbox_height, windows_width, main_origin, main_origin));
	box_create(&src_textbox, newwin(langbox_height, windows_width, main_origin, dest_boxes_col));
	box_create(&dest_langbox, newwin(textbox_height, windows_width, textboxes_row, main_origin));
	box_create(&dest_textbox, newwin(textbox_height, windows_width, textboxes_row, dest_boxes_col));

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
			change_focused_box(boxes, &focused_box, &focused_box_idx);
			break;
		case ']':
			translate(boxes);
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
