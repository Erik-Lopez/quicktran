/* See COPYING file for copyright and license details. */
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <assert.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>

#include "config.h"
#include "box.h"
#include "popups.h"
#include "layout.h"

void translate(struct Box **boxes);

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	start_color();
	quicktran_init_colors();

	WINDOW *debug_window = newwin(0, 0, 0, 0);
	(void) debug_window;

	struct Box src_langbox, src_textbox, dest_langbox, dest_textbox;
	struct Box *boxes[WIN_AMOUNT] = {&src_langbox, &src_textbox, &dest_langbox, &dest_textbox};

	quicktran_create_boxes(boxes);
	style_boxes(boxes);

	int focused_box_idx	= 0;
	struct Box *focused_box	= boxes[focused_box_idx];

	int ch;
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
		case '!':
			show_debug_win(debug_window, focused_box->content);
			box_refreshwins(boxes);
			break;
		default:
			box_addchtype(focused_box, (char)ch);
		}
		
		box_refreshwins(boxes);
	}

	endwin();
}

void translate(struct Box **boxes)
{
	char *src_lang = boxes[0]->content;
	char *src_text = boxes[1]->content;
	char *dest_lang = boxes[2]->content;
	char *dest_text = boxes[3]->content;

	box_clear(boxes[3]);

	char *options = "-brief -no-ansi";

#define BUFSIZE (strlen("trans -brief -no-ansi -from '' -to '' ''") + LANG_CAP*2 + TEXT_CAP*2 + 1)
	char command[BUFSIZE];
	snprintf(command, BUFSIZE, "trans -from \"%s\" -to \"%s\" \"%s\" %s", 
		src_lang, dest_lang, src_text, options);

	FILE *pipe = popen(command, "r");

	char ch;
	for (int i = 0; (ch = fgetc(pipe)) != -1; i++)
		dest_text[i] = ch;

	pclose(pipe);

	box_addchstr(boxes[3], dest_text);
	box_refreshwins(boxes);
}
