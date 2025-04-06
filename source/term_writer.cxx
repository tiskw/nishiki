////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: term_writer.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "term_writer.hxx"

// Include the headers of STL.
#include <unistd.h>
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static StringX generate_editing_line(const StringX& lhs, const StringX& rhs, const StringX& hist_comp,
                                     const char* histhint_pre, const char* histhint_post) noexcept
// [Abstract]
//   Computes and returns editing line.
//
// [Args]
//   lhs           (const StringX&): [IN] Left hand side of the cursor of the edit string.
//   rhs           (const StringX&): [IN] Right hand side of the cursor of the edit string.
//   hist_comp     (const StringX&): [IN] History completion.
//   histhint_pre  (const char*)   : [IN]
//   histhint_post (const char*)   : [IN]
//
// [Returns]
//   (StringX): Editing line.
//
{   // {{{

    // Case 1: only `lhs` is non-empty string.
    if ((rhs.size() == 0) and (hist_comp.size() == 0))
        return lhs.colorize() + StringX("\033[7m \033[m");

    // Case 2: `rhs` is empty.
    else if (rhs.size() == 0)
        return lhs.colorize() + StringX("\033[7m") + hist_comp.front() + StringX("\033[m")
               + StringX(histhint_pre) + hist_comp.substr(1) + StringX(histhint_post);

    // Case 3: others.
    else
        return (lhs + CharX("\033[7m") + rhs.front() + CharX("\033[27m") + rhs.substr(1)).colorize();

};  // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

TermWriter::TermWriter(const TermSize area) : area(area)
{   // {{{

    // Hide cursor.
    std::cout << "\033[?25l";

    // Move the cursor to the bottom of the drawing area.
    for (uint16_t n = 1; n < this->area.rows; ++n)
        std::cout << std::endl;

}   // }}}

TermWriter::~TermWriter(void)
{   // {{{

    // Erase drawing area.
    std::cout << "\033[" << (this->area.rows - 1) << "F" << "\033[0J";

    // Show cursor.
    std::cout << "\033[?25h" << std::flush;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void TermWriter::write(const StringX& lhs, const StringX& rhs, const StringX& ps1, const StringX& ps2,
                       const Vector<StringX>& clines, const StringX& hist_comp,
                       const char* histhint_pre, const char* histhint_post) const noexcept
{   // {{{

    // Computes editing line.
    StringX eline = generate_editing_line(lhs, rhs, hist_comp, histhint_pre, histhint_post);

    // Resume the cursor position.
    std::cout << "\033[" << (this->area.rows - 1) << "F" << std::flush;

    // Print editing lines.
    Vector<StringX> eline_chunks = eline.chunk(this->area.cols - std::max(ps1.width(), ps2.width()) - 1);
    for (uint16_t n = 0; n < eline_chunks.size(); ++n)
        std::cout << ((n == 0) ? ps1 : ps2) << eline_chunks[n] << "\033[0K" << std::endl;

    // Compute the number of completion lines.
    const uint16_t n_clines = this->area.rows - eline_chunks.size();

    // Print the completion lines.
    for (uint16_t n = 0; n < n_clines; ++n)
    {
        // Write a completion line.
        if (n < clines.size())
            std::cout << clines[n] << "\033[0K";

        // Do not write newline at the end of completion lines.
        if (n == (n_clines - 1)) { std::cout << std::flush; }
        else                     { std::cout << std::endl;  }
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
