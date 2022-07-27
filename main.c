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
#include "layout.h"

void translate(struct Box **boxes);

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	start_color();
	quicktran_init_colors();

	struct Box src_langbox, src_textbox, dest_langbox, dest_textbox;
	struct Box *boxes[WIN_AMOUNT] = {&src_langbox, &src_textbox, &dest_langbox, &dest_textbox};

	quicktran_create_boxes(boxes);
	style_boxes(boxes);

	int focused_box_idx	= 0;
	struct Box *focused_box	= boxes[focused_box_idx];

	int ch = '=';
	int previous_focus_idx;
	do {
		switch (ch) {
		case '\t':
			change_focused_box(boxes, &focused_box, &focused_box_idx);
			break;
		case ']':
			/* Translate and keep the cursor in its current box. */
			previous_focus_idx = focused_box_idx - 1;
			translate(boxes);
			focused_box_idx = previous_focus_idx;
			change_focused_box(boxes, &focused_box, &focused_box_idx);
			break;
		case KEY_BACKSPACE:
			box_delchtype(focused_box);	
			break;
		case '=':		
			box_clear(focused_box);
			break;
		default:
			box_addchtype(focused_box, (char)ch);
			break;
		}
		
		ch = wgetch(focused_box->input_window);
		box_refreshwins(boxes);
	} while (ch != '~');

	endwin();
}

void translate(struct Box **boxes)
{
	char *src_lang = boxes[0]->content;
	char *src_text = boxes[1]->content;
	char *dest_lang = boxes[2]->content;
	char *dest_text = boxes[3]->content;

	box_clear(boxes[3]);

	char *options = "-brief -no-ansi -no-warn";

#define BUFSIZE (strlen(options) + LANG_CAP*2 + TEXT_CAP*2 + 1)
	char command[BUFSIZE];
	snprintf(command, BUFSIZE, "trans -from \"%s\" -to \"%s\" \"%s\" %s", 
		src_lang, dest_lang, src_text, options);

	FILE *pipe = popen(command, "r");

	int i;
	char ch;
	for (i = 0; (ch = fgetc(pipe)) != -1; i++)
		dest_text[i] = ch;
	dest_text[i - 1] = 0; // remove '\n' left by trans

	pclose(pipe);

	box_addchstr(boxes[3], dest_text);
	box_refreshwins(boxes);
}
