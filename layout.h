/* See COPYING file for copyright and license details. */
#ifndef _LAYOUT_H
#define _LAYOUT_H

int get_available_hspace();
int get_available_vspace();
int get_langbox_height();
int get_windows_width();
int get_textbox_height();

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

#endif
