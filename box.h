#ifndef _BOX_H
#define _BOX_H

#include <ncurses.h>

#define WIN_AMOUNT 4
#define TEXT_CAP 500
#define LANG_CAP 500

struct Box
{
	WINDOW *window;
	chtype content[TEXT_CAP];
	chtype *last_element;
};

void box_create(struct Box *b, WINDOW *window)
{
	b->window = window;

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

void change_focused_box(struct Box **boxes, struct Box **focused_box, int *focused_box_idx)
{
	*focused_box_idx = (*focused_box_idx + 1) % WIN_AMOUNT;
	*focused_box = boxes[*focused_box_idx];
}

#endif
