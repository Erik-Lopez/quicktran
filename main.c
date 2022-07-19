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

void translate(struct Box **boxes)
{
	// This is the order: src_langbox, src_textbox, dest_langbox, dest_textbox
	wchar_t *src_wlang = (wchar_t*)boxes[0]->content;
	wchar_t *src_text = (wchar_t*)boxes[1]->content;
	wchar_t *dest_wlang = (wchar_t*)boxes[2]->content;
	wchar_t *dest_text = (wchar_t*)boxes[3]->content;

	char *src_lang = wchar_to_char(src_wlang, LANG_CAP);
	char *dest_lang = wchar_to_char(dest_wlang, LANG_CAP);

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
	while(fgets((char*)dest_text, TEXT_CAP, pipe));

	printw("Traducir '%ls' del '%s' al '%s' sería '%s'.", src_text, src_lang, dest_lang, (char*)dest_text);

	pclose(pipe);

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
	box_create(&dest_langbox, newwin(langbox_height, windows_width, main_origin, dest_boxes_col));
	box_create(&src_textbox, newwin(textbox_height, windows_width, textboxes_row, main_origin));
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
