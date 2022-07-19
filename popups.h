#ifndef _POPUPS_H
#define _POPUPS_H

#include <ncurses.h>

void show_debug_win(WINDOW *debug_window, chtype *text)
{
	waddchstr(debug_window, text);
	wrefresh(debug_window);
	wgetch(debug_window);
}

void show_error_win(WINDOW *debug_window, chtype *text)
{
	show_debug_win(debug_window, text);
	endwin();
	exit(1);
}

#endif