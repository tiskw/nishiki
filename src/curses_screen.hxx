////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: curses_screen.cxx                                                           ///
///                                                                                              ///
/// This file defines `CursesScreen` which is a base class for the curses-based class like       ///
/// FileChooser and TextChooser.                                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CURSES_SCREEN_HXX
#define CURSES_SCREEN_HXX

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class CursesScreen
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

         CursesScreen(void);
        ~CursesScreen(void);
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
