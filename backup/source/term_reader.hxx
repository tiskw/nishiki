////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: term_reader.hxx                                                             ///
///                                                                                              ///
/// This file defines the class `TermReader` that manages user input from terminal.              ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TERM_READER_HXX
#define TERM_READER_HXX

// Include the headers of STL.
#include <termios.h>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class TermReader
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

         TermReader(void);
        ~TermReader(void);
        // [Abstract]
        //   Default constructor and destructor of TermReader.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        CharX getch(void) noexcept;
        // [Abstract]
        //   Get valid UTF-8 character from STDIN and returns it. If the acquired character is
        //   registered in the keybind, convert the character to a binded string (most of the
        //   binded string will be added to the stack).
        //
        // [Returns]
        //   (CharX): Captured character.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Class static constants
        ////////////////////////////////////////////////////////////////////////////////////////////

        static const uint8_t buffer_size = 9;
        // Size of the variable "buffer".
        // This size includes the margin to store the string end character '\0'.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        int fd;
        // File descriptor to read input characters.

        struct termios term;
        // Copy of the original termios. This class changes the termios, and this member
        // variable is used to save the original termios and restore when this class is deleted.

        char buffer[buffer_size];
        // Buffer to store characters read from STDIN.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
