# Quicktran

Quicktran is a quick terminal-based translator.

It's an NCurses interface for the [translate-shell](https://github.com/soimort/translate-shell/) program.

# Installation

## Building

To build Quicktran, simply enter the following command:

```console
$ ./build.sh
```

## Dependencies

- [translate-shell](https://github.com/soimort/translate-shell).
- `ncurses` (and `lib32-ncurses`)

# Usage

To run quicktran:

```console
$ quicktran
```

You'll get 4 boxes in which to write all the necessary information like so:

![Quicktran usage](https://github.com/Erik-Lopez/quicktran/blob/media/usage.gif)

## Controls

- `Ctrl + n` -> Jump to next box (`TAB` also works).
- `Ctrl + e` -> Jump to previous box.
- `]` -> Translate.
- `=` -> Clear all of the text inside a box.
- `~` -> Quit.

# Configuration

Everything you can customize is located in the `config.h` file.
