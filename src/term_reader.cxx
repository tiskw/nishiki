////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: term_reader.hxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "term_reader.hxx"

#include <csignal>
#include <iostream>
#include <unistd.h>

#include "config.hxx"
#include "file_chooser.hxx"
#include "text_chooser.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition of static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void
signal_handler_sigint(const int signum)
noexcept;
// [Abstract]
//   Signal handler function for SIGINT.
//
// [Args]
//   signum (const int): [IN] Signal number.
//
// [Returns]
//   void


////////////////////////////////////////////////////////////////////////////////////////////////////
// TermReader: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

TermReader::TermReader(void) : fd(STDIN_FILENO)
{   // {{{

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

}   // }}}

TermReader::~TermReader(void)
{   // {{{

    // Restore termios.
    tcsetattr(this->fd, TCSANOW, &this->term);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TermReader: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX
TermReader::getch(void)
noexcept
{   // {{{

    // Read a raw character from STDIN.
    return CharX(std::cin, true);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void
signal_handler_sigint(const int signum)
noexcept
{   // {{{

    if (signum == SIGINT)
    { /* Do nothing */ }

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
