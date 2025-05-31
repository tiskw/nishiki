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
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class TermWriter
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and descructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        explicit TermWriter(const TermSize area);
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

        void write(const StringX& lhs, const StringX& rhs, const StringX& ps1, const StringX& ps2,
                   const Vector<StringX>& clines, const StringX& hist_comp,
                   const char* histhint_pre, const char* histhint_post) const noexcept;
        // [Abstract]
        //   Write the given contents to the terminal.
        //
        // [Args]
        //   lhs       (const StringX&)        : [IN] Left hand side of the cursor of the edit string.
        //   rhs       (const StringX&)        : [IN] Right hand side of the cursor of the edit string.
        //   mode      (TextBuffer::Mode)      : [IN] Current editing mode.
        //   clines    (const Vector<StringX>&): [IN] Completion lines to be shown in the terminal.
        //   hist_comp (const StringX&)        : [IN] History completion.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        TermSize area;
        // Size of drawing area.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
