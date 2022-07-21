/* See COPYING file for copyright and license details. */
#ifndef _POPUPS_H
#define _POPUPS_H

#include <ncurses.h>

void show_debug_win(WINDOW *debug_window, char *text)
{
	waddstr(debug_window, text);
	wrefresh(debug_window);
	wgetch(debug_window);
}

void show_error_win(WINDOW *debug_window, char *text)
{
	show_debug_win(debug_window, text);
	endwin();
	exit(1);
}

#endif
