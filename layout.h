/* See COPYING file for copyright and license details. */
#ifndef _LAYOUT_H
#define _LAYOUT_H

int get_available_hspace();
int get_available_vspace();
int get_langbox_height();
int get_windows_width();
int get_textbox_height();
void quicktran_create_boxes(struct Box **boxes);
void style_boxes(struct Box **boxes);

int get_available_hspace()
{
	const int padded_window_cols = COLS - STDSCR_PADDING*2;
	const int available_hspace = padded_window_cols - TEXT_TEXT_PADDING;

	return available_hspace;
}

int get_available_vspace()
{
	const int padded_window_rows	= LINES - STDSCR_PADDING*2;
	const int available_vspace	= padded_window_rows - TEXT_LANG_PADDING;

	return available_vspace;
}

int get_langbox_height()
{
	const int available_vspace = get_available_vspace();
	/*
	 * We know that `available_vspace = textbox_height + langbox_height`
	 * And also that `textbox_height = langbox_height * TEXT_TO_LANG_RATIO`
	 * Hence: `available_vspace = langbox_height * TEXT_TO_LANG_RATIO + langbox_height`
	 * or: `available_vspace = langbox_height * (TEXT_TO_LANG_RATIO + 1)`
	 * In other words: `langbox_height = available_vspace / (TEXT_TO_LANG_RATIO + 1)`
	 */
	const int langbox_height = available_vspace / (TEXTH_TO_LANGH_RATIO + 1);
	return langbox_height;
}

int get_windows_width()
{
	const int available_hspace = get_available_hspace();
	const int windows_width = available_hspace / 2;

	return windows_width;
}

int get_textbox_height()
{
	const int langbox_height = get_langbox_height();
	const int textbox_height = langbox_height * TEXTH_TO_LANGH_RATIO;

	return textbox_height;
}

void quicktran_create_boxes(struct Box **boxes)
{
	const int langbox_height	= get_langbox_height();
	const int textbox_height	= get_textbox_height();
	const int windows_width		= get_windows_width();

	const int main_origin		= STDSCR_PADDING;
	const int dest_boxes_col	= STDSCR_PADDING + windows_width + TEXT_TEXT_PADDING;
	const int textboxes_row		= STDSCR_PADDING + langbox_height + TEXT_LANG_PADDING;

	box_create(boxes[0], newwin(langbox_height, windows_width, main_origin, main_origin), LANGBOX_INNER_PADDING, LANGBOX);
	box_create(boxes[1], newwin(textbox_height, windows_width, textboxes_row, main_origin), TEXTBOX_INNER_PADDING, TEXTBOX);
	box_create(boxes[2], newwin(langbox_height, windows_width, main_origin, dest_boxes_col), LANGBOX_INNER_PADDING, LANGBOX);
	box_create(boxes[3], newwin(textbox_height, windows_width, textboxes_row, dest_boxes_col), TEXTBOX_INNER_PADDING, TEXTBOX);	
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

		keypad(boxes[i]->window, TRUE);
		box(boxes[i]->window, 0, 0);

		wnoutrefresh(boxes[i]->window);
	}

	doupdate();
}

#endif
