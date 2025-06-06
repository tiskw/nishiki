////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: term_writer.hxx                                                             ///
///                                                                                              ///
/// This file defines the class `TermWriter` that manages output to terminal.                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TERM_WRITER_HXX
#define TERM_WRITER_HXX

// Include the headers of STL.
#include <cstdint>

// Include the headers of custom modules.
#include "string_x.hxx"
#include "text_buffer.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class TermWriter
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and descructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        explicit TermWriter(uint16_t height);
        // [Abstract]
        //   Default constructor of TermWriter.
        //
        // [Args]
        //   height (uint16_t): [IN] Height of drawing area.

        ~TermWriter(void);
        // [Abstract]
        //   Default destructor of TermWriter.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        write(const std::string& prompt, const StringX& lhs, const StringX& rhs, const std::vector<StringX>& clines, const StringX& hist_comp) const noexcept;
        // [Abstract]
        //   Write the given contents to the terminal.
        //   Layout of the contents is the following.
        //
        //     [PROMPT1         ][PROMPT2]
        //     [PROMPT3] [USER INPUT     ]
        //     [USER INPUT               ]
        //     [COMPLETION               ]
        //
        // [Args]
        //   lhs       (const StringX&)             : [IN] Left hand side of the cursor of the edit string.
        //   rhs       (const StringX&)             : [IN] Right hand side of the cursor of the edit string.
        //   mode      (TextBuffer::Mode)           : [IN] Current editing mode.
        //   clines    (const std::vector<StringX>&): [IN] Completion lines to be shown in the terminal.
        //   hist_comp (const StringX&)             : [IN] History completion.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        uint16_t wid, hgt;
        // Terminal size.

        StringX prompt_head;
        // Header line of the prompt.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
