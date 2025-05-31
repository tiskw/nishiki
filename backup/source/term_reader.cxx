////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: term_reader.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "term_reader.hxx"

// Include the headers of STL.
#include <csignal>
#include <cstring>
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void
signal_handler_sigint(const int signum) noexcept
// [Abstract]
//   Signal handler function for SIGINT.
//
// [Args]
//   signum (const int): [IN] Signal number.
//
// [Returns]
//   void
//
{   // {{{

    if (signum == SIGINT)
    { /* Do nothing */ }

}   // }}}

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
    term_cpy.c_lflag    &= ~(ICANON | ECHO);
    term_cpy.c_lflag    |= ISIG;
    term_cpy.c_iflag    &= ~ICRNL;
    term_cpy.c_cc[VMIN]  = 0;
    term_cpy.c_cc[VTIME] = 10;

    // NOTE: termios.c_cc[VMIN] and termios.c_cc[VTIME].
    //       <https://manpages.debian.org/bookworm/manpages-dev/termios.3.en.html>
    // 
    // MIN == 0, TIME == 0 (polling read):
    //     If data is available, read(2) returns immediately, with the lesser of the number of bytes
    //     available, or the number of bytes requested. If no data is available, read(2) returns 0.
    //
    // MIN > 0, TIME == 0 (blocking read):
    //     read(2) blocks until MIN bytes are available, and returns up to the number of bytes
    //     requested.
    //
    // MIN == 0, TIME > 0 (read with timeout):
    //     TIME specifies the limit for a timer in tenths of a second. The timer is started when
    //     read(2) is called. read(2) returns either when at least one byte of data is available,
    //     or when the timer expires. If the timer expires without any input becoming available,
    //     read(2) returns 0. If data is already available at the time of the call to read(2),
    //     the call behaves as though the data was received immediately after the call.
    //
    // MIN > 0, TIME > 0 (read with interbyte timeout):
    //     TIME specifies the limit for a timer in tenths of a second. Once an initial byte of
    //     input becomes available, the timer is restarted after each further byte is received.
    //     read(2) returns when any of the following conditions is met:

    // Set the cbreak termios to STDIN.
    tcsetattr(this->fd, TCSAFLUSH, &term_cpy);

    // Set signal handler for SIGINT.
    signal(SIGINT, signal_handler_sigint);

    // Initialize the buffer. The member function "getch" expect the buffer
    // to be initialized by zero, at least the first byte of the buffer.
    std::memset(this->buffer, 0, this->buffer_size);

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
TermReader::getch(void) noexcept
{   // {{{

    // Read new characters from STDIN of no data remained in the buffer.
    if (this->buffer[0] == 0)
    {
        // Initialize the buffer before reading from STDIN.
        std::memset(this->buffer, 0, this->buffer_size);

        // Read characters from STDIN.
        // Notes:
        //   * The terminal attributes are changed by the constructor of this class.
        //   * The last byte of the buffer should be kept as zero.
        while (read(STDIN_FILENO, buffer, this->buffer_size - 1) == 0)
            ; // Just repeat the "read" function if timed out.
    }

    // Construct a new character from the buffer.
    CharX cx = CharX(this->buffer);

    // Remove the data used bu the CharX.
    for (uint8_t i = 0; i < (this->buffer_size - cx.size); ++i)
        this->buffer[i] = this->buffer[i + cx.size];

    return cx;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
