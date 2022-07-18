CFLAGS="-Wall -Wextra -ggdb --pedantic"
CLIBS="-lncurses"

cc -o ./bin/quicktran main.c $CFLAGS $CLIBS
