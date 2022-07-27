/* See COPYING file for copyright and license details. */
#ifndef _BOX_H
#define _BOX_H

#include <ncurses.h>

#include "config.h"
#include "layout.h"

enum BoxType { LANGBOX = 0, TEXTBOX };
enum BoxDirection { UP = 0, DOWN, LEFT, RIGHT };

void find_last_nonwhitespace_char_position(WINDOW *window, int row, int *x)
{
	int maxx = getmaxx(window) - 1;
	for (int i = maxx; i > 0; i--) {
		int ch = mvwinch(window, row, i);
		if ((char)ch != ' ' && (char)ch != '\n') {
			*x = i;
			break;
		}
	}
}
struct Box
{
	WINDOW *window;
	WINDOW *input_window;	// A subwindow of `window` where we write. (To avoid the frame)
	char content[TEXT_CAP];
	char *last_element;
	enum BoxType type;
};

void box_create(struct Box *box, enum BoxType type, enum BoxDirection direction)
{
	const int langbox_height	= get_langbox_height();
	const int textbox_height	= get_textbox_height();
	const int windows_width		= get_windows_width();

	const int main_origin		= STDSCR_PADDING;
	const int right_boxes_col	= STDSCR_PADDING + windows_width + TEXT_TEXT_PADDING;
	const int textboxes_row		= STDSCR_PADDING + langbox_height + TEXT_LANG_PADDING;

	int row, col, height, inner_padding;
	WINDOW *window;

	if (direction == LEFT)
		col = main_origin;
	else if (direction == RIGHT)
		col = right_boxes_col;
	else
		exit(1);

	if (type == LANGBOX) {
		height = langbox_height;
		row = main_origin;
		inner_padding = LANGBOX_INNER_PADDING;
	} else if (type == TEXTBOX) {
		height = textbox_height;
		row = textboxes_row;
		inner_padding = TEXTBOX_INNER_PADDING;
	} else 
		exit(1);

	window = newwin(height, windows_width, row, col);
	box->window = window;

	box->input_window = derwin(window, 
				height - 2*inner_padding, // rows without the sidepads
				windows_width - 2*inner_padding, // cols without the sidepads
				inner_padding, inner_padding);
	box->type = type;

	for (int i = 0; i < TEXT_CAP; i++)
		box->content[i] = 0;

	box->last_element = &(box->content[0]);
}

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
		flash();
		return;
	}

	int y, x;
	getyx(box->input_window, y, x);

	if (x == 0) {
		y--;
		find_last_nonwhitespace_char_position(box->input_window, y, &x);
		wmove(box->input_window, y, x + 1);
	} else {
		box->last_element--;
		*(box->last_element) = 0;
		x--;
		mvwdelch(box->input_window, y, x);
	}

}

void box_addchtype(struct Box *box, char ch)
{
	if (box_isfull(box) == TRUE) {
		// TODO: Add scrolling
		endwin();
		exit(1);
	}

	*(box->last_element) = ch;
	box->last_element++;

	waddch(box->input_window, ch);

	// Adding it and then removing it for keeping focused_box the same.
	if (box->type == LANGBOX && isspace(ch))
		box_delchtype(box);
}

void box_addchstr(struct Box *box, char *chstr)
{
	while (*chstr != 0) {
		box_addchtype(box, *chstr);
		chstr++;
	}
}

void box_refreshwins(struct Box **boxes)
{
	for (int i = 0; i < WIN_AMOUNT; i++) {
		wnoutrefresh(boxes[i]->window);
		wnoutrefresh(boxes[i]->input_window);
	}

	doupdate();
}

void box_clear(struct Box *box)
{
	wclear(box->input_window);
	box->last_element = box->content;
	box->content[0] = 0;
}

void change_focused_box(struct Box **boxes, struct Box **focused_box, int *focused_box_idx)
{
	*focused_box_idx = (*focused_box_idx + 1) % WIN_AMOUNT;
	*focused_box = boxes[*focused_box_idx];

	int y, x;
	getyx(boxes[*focused_box_idx]->input_window, y, x);
	wmove(boxes[*focused_box_idx]->input_window, y, x);
}

void quicktran_create_boxes(struct Box **boxes)
{
	box_create(boxes[0], LANGBOX, LEFT);
	box_create(boxes[1], TEXTBOX, LEFT);
	box_create(boxes[2], LANGBOX, RIGHT);
	box_create(boxes[3], TEXTBOX, RIGHT);
}

void style_boxes(struct Box **boxes)
{
	bkgd(COLOR_PAIR(BACKGROUND_COLOR));
	refresh();
	for (int i = 0; i < WIN_AMOUNT; i++) {
		if (boxes[i]->type == LANGBOX)
			wbkgd(boxes[i]->window, COLOR_PAIR(LANGBOX_COLOR));
		else if (boxes[i]->type == TEXTBOX)
			wbkgd(boxes[i]->window, COLOR_PAIR(TEXTBOX_COLOR));

		keypad(boxes[i]->input_window, TRUE);
		box(boxes[i]->window, 0, 0);

		wnoutrefresh(boxes[i]->window);
	}

	doupdate();
}

#endif
