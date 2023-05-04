////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: term_reader.cxx
//
// This file defines the class `TermReader` that manages user input from terminal.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "term_reader.hxx"

#include <csignal>
#include <iostream>
#include <unistd.h>

#include "config.hxx"
#include "file_chooser.hxx"
#include "text_chooser.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// File local functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Signal handler function for SIGINT.
//
// [Args]
//   signum (const int): [IN] Signal number.
//
// [Returns]
//   void
//
static void
signal_handler_sigint(const int signum)
noexcept
{
    if (signum == SIGINT)
    { /* Do nothing */ }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermReader: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Default constructor of TeamReader.
//
// [Args]
//   void
//
TermReader::TermReader(void) : fd(STDIN_FILENO)
{
    // Copy current termios.
    tcgetattr(this->fd, &this->term);

    // Create a copy of the current termios.
    struct termios term_cpy = this->term;

    // Change termios setting to cbreak mode.
    term_cpy.c_lflag     &= ~(ICANON | ECHO);
    term_cpy.c_lflag     |= ISIG;
    term_cpy.c_iflag     &= ~ICRNL;
    term_cpy.c_cc[VMIN]   = 1;
    term_cpy.c_cc[VTIME]  = 0;

    // Set the cbreak termios to STDIN.
    tcsetattr(this->fd, TCSAFLUSH, &term_cpy);

    // Set signal handler for SIGINT.
    signal(SIGINT, signal_handler_sigint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermReader: Destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Default destructor of TeamReader.
//
// [Args]
//   void
//
TermReader::~TermReader(void)
{
    // Restore termios.
    tcsetattr(this->fd, TCSANOW, &this->term);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermReader: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Get valid UTF-8 character from STDIN and returns it. If the acquired character is registered
// in the keybind, convert the character to a binded string (most of the binded string will be
// added to the stack).
//
// [Args]
//   void
//
// [Returns]
//   (CharX): Captured character.
//
CharX
TermReader::getch(void)
noexcept
{
    // Read a raw character from STDIN.
    return CharX(std::cin, true);
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
