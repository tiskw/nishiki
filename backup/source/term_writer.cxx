////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: term_writer.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "term_writer.hxx"

// Include the headers of STL.
#include <numeric>
#include <unistd.h>
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

TermWriter::TermWriter(uint16_t height) : hgt(height)
{   // {{{

    // Get terminal size.
    get_terminal_size(&this->wid, nullptr);

    // Hide cursor.
    std::cout << "\033[?25l";

    // Move the cursor to the bottom of the drawing area.
    for (uint16_t n = 0; n < (this->hgt - 1); ++n)
        std::cout << '\n';

}   // }}}

TermWriter::~TermWriter(void)
{   // {{{

    // Clear drawing area.
    std::cout << "\033[0J";

    // Show cursor.
    std::cout << "\033[?25h";

    // Erase drawing area.
    std::cout << "\033[" << this->hgt << "F" << "\033[0J" << std::endl;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
TermWriter::write(const std::string& prompt, const StringX& lhs, const StringX& rhs, const std::vector<StringX>& clines, const StringX& hist_comp) const noexcept
{   // {{{

    constexpr auto generate_editing_line = [](const StringX& lhs, const StringX& rhs, const StringX& hist_comp) noexcept -> StringX
    // [Abstract]
    //   Computes and returns editing line.
    //
    // [Args]
    //   lhs       (const StringX&): [IN] Left hand side of the cursor of the edit string.
    //   rhs       (const StringX&): [IN] Right hand side of the cursor of the edit string.
    //   hist_comp (const StringX&): [IN] History completion.
    //
    // [Returns]
    //   (StringX): Editing line.
    {
        // Case 1: only `lhs` is non-empty string.
        if ((rhs.size() == 0) and (hist_comp.size() == 0))
            return lhs.colorize() + "\033[7m \033[m";

        // Case 2: `rhs` is empty.
        else if (rhs.size() == 0)
            return lhs.colorize() + "\033[7m" + hist_comp.front() + "\033[m" + config.histhint_pre + hist_comp.substr(1) + config.histhint_post;

        // Case 3: others.
        else
            return (lhs + "\033[7m" + rhs.front() + "\033[27m" + rhs.substr(1)).colorize();
    };

    // Computes editing line.
    StringX eline = StringX(prompt) + generate_editing_line(lhs, rhs, hist_comp);

    // Resume the cursor position.
    std::cout << "\033[" << (this->hgt - 1) << "F";

    // Print editing lines.
    std::vector<StringX> eline_chunks = eline.chunk(this->wid - 1);
    for (const StringX& sx : eline_chunks)
        std::cout << sx << "\033[0K" << std::endl;

    //
    uint16_t n_clines = this->hgt - eline_chunks.size();

    for (uint16_t i = 0; i < n_clines; ++i)
    {
        // Write a completion line.
        std::cout << clines[i] << "\033[0K";

        // Do not write newline at the end of completion lines.
        if (i == (n_clines - 1)) std::cout << std::flush;
        else                     std::cout << '\n';
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
