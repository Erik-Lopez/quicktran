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
#include "translate.h"

#define CTRL(x) ((x) & 0x1f)

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	nonl();
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
		case CTRL('n'):
			offset_focused_box(boxes, &focused_box, &focused_box_idx, 1);
			break;
		case CTRL('e'):
			offset_focused_box(boxes, &focused_box, &focused_box_idx, -1);
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
		case '\r':
			ch = '\n';
			box_addchtype(focused_box, (char)ch);
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
