////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: curses_screen.cxx                                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "curses_screen.hxx"

#include <locale.h>
#include <ncurses.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
// CursesScreen: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CursesScreen::CursesScreen(void)
{   // {{{

    // Set locale to enable UTF-8 display.
    setlocale(LC_ALL, "");

    // Start curses.
    initscr();

    // Start color.
    start_color();
    use_default_colors();

    // Initialize colors using NCurses default color index.
    //   - COLOR_RED     1
    //   - COLOR_GREEN   2
    //   - COLOR_YELLOW  3
    //   - COLOR_BLUE    4
    //   - COLOR_MAGENTA 5
    //   - COLOR_CYAN    6
    //   - COLOR_WHITE   7
    for (int32_t color_index = 1; color_index < 8; ++color_index)
        init_pair(color_index, color_index, -1);

    // Enable noecho mode.
    noecho();

    // Enable cbreak mode.
    cbreak();

    // Enable keypad.
    keypad(stdscr, true);

    // Hide curses cursor.
    curs_set(0);

}   // }}}

CursesScreen::~CursesScreen(void)
{   // {{{

    // Finalize curses.
    endwin();

    // Hide terminal cursor.
    fputs("\033[?25l", stdout);
    fflush(stdout);

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
