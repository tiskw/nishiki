////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: term_writer.hxx                                                             ///
///                                                                                              ///
/// This file defines the class `TermWriter` that manages output to terminal.                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TERM_WRITER_HXX
#define TERM_WRITER_HXX

#include <cstdio>
#include <map>
#include <string>
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

         TermWriter(void);
        ~TermWriter(void);


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        write(const StringX& lhs, const StringX& rhs, TextBuffer::Mode mode, const std::vector<StringX>& clines, const StringX& hist_comp)
        const noexcept;
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
        //
        // [Returns]
        //   void


    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        unsigned short wid, hgt;
        // [Abstract]
        //   Terminal size.

        StringX prompt_head;
        // [Abstract]
        //   Header line of the prompt.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        update(void)
        noexcept;
        // [Abstract]
        //   Update auxiliary information.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void
};


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
