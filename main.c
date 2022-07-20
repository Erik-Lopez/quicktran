#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include <wchar.h>
#include <locale.h>

#include "config.h"
#include "box.h"
#include "popups.h"

char *wchar_to_char(wchar_t *wstr, size_t length)
{
	char *str = (char*)malloc(length);
	for (size_t i = 0; i < length; i++)
		str[i] = (char)wstr[i];
	return str;
}
chtype *char_to_chtype(char *str, size_t length)
{
	chtype *chstr = (chtype *)malloc(length*2);
	for (size_t i = 0; i < length; i++)
		chstr[i] = (chtype){ str[i], A_NORMAL };
	return chstr;
}

void translate(struct Box **boxes)
{
	// This is the order: src_langbox, src_textbox, dest_langbox, dest_textbox
	wchar_t *src_wlang = (wchar_t*)boxes[0]->content;
	wchar_t *src_text = (wchar_t*)boxes[1]->content;
	wchar_t *dest_wlang = (wchar_t*)boxes[2]->content;
	wchar_t *dest_wtext = (wchar_t*)boxes[3]->content;

	char *src_lang = wchar_to_char(src_wlang, LANG_CAP);
	char *dest_lang = wchar_to_char(dest_wlang, LANG_CAP);
	char *dest_text = wchar_to_char(dest_wtext, TEXT_CAP);

	//multiply times 2 because widestring to string conversion
	//TODO: fwprintf() y en general usar widechars
	//
#define BUFSIZE (12 + LANG_CAP*2 + 5 + LANG_CAP*2 + 3 + TEXT_CAP*2 + 1 + 100000)
	char *src_text2 = wchar_to_char(src_text, TEXT_CAP); 
	char *options = "-brief -no-ansi";

	char command[BUFSIZE];
	sprintf(command, "trans -from %s -to %s \"%s\" %s", 
		src_lang, dest_lang, src_text2, options);

	FILE *pipe = popen(command, "r");

	//fgetws(dest_text, TEXT_CAP, pipe);
	while(fgets(dest_text, TEXT_CAP, pipe));

	//printw("Traducir '%ls' del '%s' al '%s' sería '%s'.", src_text, src_lang, dest_lang, (char*)dest_text);
	
	pclose(pipe);

	wclear(boxes[3]->input_window);
	box_addchstr(boxes[3], (chtype *)dest_text);
}

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	start_color();
	quicktran_init_colors();

	WINDOW *debug_window = newwin(0, 0, 0, 0);
	(void) debug_window;

	const int rows = LINES - STDSCR_PADDING*2;
	const int cols = COLS - STDSCR_PADDING*2;

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
	box_create(&src_langbox, newwin(langbox_height, windows_width, main_origin, main_origin), LANGBOX_INNER_PADDING, LANGBOX);
	box_create(&dest_langbox, newwin(langbox_height, windows_width, main_origin, dest_boxes_col), LANGBOX_INNER_PADDING, LANGBOX);
	box_create(&src_textbox, newwin(textbox_height, windows_width, textboxes_row, main_origin), TEXTBOX_INNER_PADDING, TEXTBOX);
	box_create(&dest_textbox, newwin(textbox_height, windows_width, textboxes_row, dest_boxes_col), TEXTBOX_INNER_PADDING, TEXTBOX);

	struct Box *boxes[WIN_AMOUNT] = {&src_langbox, &src_textbox, &dest_langbox, &dest_textbox};

	int focused_box_idx	= 0;
	struct Box *focused_box	= boxes[focused_box_idx];

	// TODO: turn this kind of code into a foreach with vararg functions (Maybe?)
	bkgd(COLOR_PAIR(BACKGROUND_COLOR));
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
