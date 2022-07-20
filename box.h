#ifndef _BOX_H
#define _BOX_H

#include <ncurses.h>
#include "config.h"

enum BoxType { LANGBOX = 0, TEXTBOX };

struct Box
{
	WINDOW *window;
	WINDOW *input_window;	// A subwindow of `window` where we write. (To avoid the frame)
	char content[TEXT_CAP];
	char *last_element;
	enum BoxType type;
};

void box_create(struct Box *b, WINDOW *window, int inner_padding, enum BoxType type)
{
	int window_rows, window_cols;
	getmaxyx(window, window_rows, window_cols);

	b->window = window;
	b->input_window = derwin(window, 
				window_rows - 2*inner_padding, // rows without the sidepads
				window_cols - 2*inner_padding, // cols without the sidepads
				inner_padding, inner_padding);
	b->type = type;

	for (int i = 0; i < TEXT_CAP; i++)
		b->content[i] = 0;

	b->last_element = &(b->content[0]);
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

	box->last_element--;
	*(box->last_element) = 0;

	int y, x;
	getyx(box->input_window, y, x);
	mvwdelch(box->input_window, y, x-1);
}

void box_addchtype(struct Box *box, char ch)
{
	if (box_isfull(box) == TRUE) {
		endwin();
		exit(1);
	}

	*(box->last_element) = ch;
	box->last_element++;
	waddch(box->input_window, ch);
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
	while (!box_isempty(box))
		box_delchtype(box);
}

void change_focused_box(struct Box **boxes, struct Box **focused_box, int *focused_box_idx)
{
	*focused_box_idx = (*focused_box_idx + 1) % WIN_AMOUNT;
	*focused_box = boxes[*focused_box_idx];
}

#endif
