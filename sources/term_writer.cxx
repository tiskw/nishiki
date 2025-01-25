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

TermWriter::TermWriter(void)
{   // {{{

    // Get terminal size.
    get_terminal_size(&this->wid, &this->hgt);

    // Hide cursor.
    std::cout << "\033[?25l";

    // Update auxiliary information.
    this->update();

    // Move the cursor to the bottom of the drawing area.
    for (uint16_t n = 0; n < config.area_hgt; ++n)
        std::cout << std::endl;

}   // }}}

TermWriter::~TermWriter(void)
{   // {{{

    // Clear drawing area.
    std::cout << "\033[0J";

    // Show cursor.
    std::cout << "\033[?25h";

    // Erase drawing area.
    std::cout << "\033[" << config.area_hgt << "F" << "\033[0J" << std::endl;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
TermWriter::write(const StringX& lhs, const StringX& rhs, TextBuffer::Mode mode, const std::vector<StringX>& clines, const StringX& hist_comp) const noexcept
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
    StringX eline = generate_editing_line(lhs, rhs, hist_comp);

    // Append Prompt before the editing line.
    if      (mode == TextBuffer::Mode::INSERT) eline = StringX(config.prompt3_ins) + eline;
    else if (mode == TextBuffer::Mode::NORMAL) eline = StringX(config.prompt3_nor) + eline;

    // Print header and prompts.
    std::cout << "\033[" << config.area_hgt << "F" << std::endl;
    std::cout << this->prompt_head << "\033[0K" << std::endl;

    // Print editing lines.
    std::vector<StringX> eline_chunks = eline.chunk(this->wid - 2);
    for (const StringX& sx : eline_chunks)
        std::cout << sx << "\033[0K" << std::endl;

    // Compute the number of completion lines to be printed.
    int16_t n_clines = clines.size() - std::max(0, static_cast<int16_t>(eline_chunks.size()) - 1);

    for (int16_t i = 0; i < n_clines; ++i)
    {
        // Write a completion line.
        std::cout << clines[i] << "\033[0K";

        // Do not write newline at the end of completion lines.
        if (i == (n_clines - 1)) std::cout << std::flush;
        else                     std::cout << std::endl;
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
TermWriter::update(void) noexcept
{   // {{{

    constexpr auto replace_info = [](const std::string& src, const std::map<std::string, std::string>& info) noexcept -> std::string
    // [Abstract]
    //   A function to replace all auxiliary information tag.
    //
    // [Args]
    //   src  (const std::string&)                       : [IN] Source string.
    //   info (const std::map<std::string, std::string>&): [IN] Replace targets.
    //
    // [Returns]
    //   (std::string): Replaced string.
    {
        constexpr auto replace_item = [](const std::string& src, const std::pair<std::string, std::string>& item) noexcept -> std::string
        // [Abstract]
        //   A function to replace one auxiliary information tag.
        //
        // [Args]
        //   src  (const std::string&)                        : [IN] Source string.
        //   item (const std::pair<std::string, std::string>&): [IN] Replace target.
        //
        // [Returns]
        //   (std::string): Replaced string.
        {
            return replace(src, item.first, item.second);
        };

        return std::accumulate(info.begin(), info.end(), src, replace_item);
    };

    // A container to store auxiliary information.
    std::map<std::string, std::string> info;

    // Get user name.
    info["{user}"] = getenv("USER");

    // Get host name.
    char hostname[128];
    gethostname(hostname, 128);
    info["{host}"] = hostname;

    // Get date and time.
    info["{date}"] = get_date();
    info["{time}"] = get_time();

    // Get current working directory.
    info["{cwd}"] = get_cwd();

    // Get git branch info.
    info["{git}"] = get_git_branch_info();

    // Replace all auxiliary information tag and create header prompt.
    StringX prompt1 = StringX(replace_info(config.prompt1, info));
    StringX prompt2 = StringX(replace_info(config.prompt2, info));
    size_t  len_ws  = std::max(0, this->wid - prompt1.width() - prompt2.width() - 1);
    this->prompt_head = prompt1 + CharX(" ") * len_ws + prompt2;

    if (this->prompt_head.width() >= this->wid)
    {
        // Convert current directory string to StringX instance.
        const StringX cwd = StringX(info["{cwd}"]);

        // Determine width of current working directory path.
        const uint16_t len_cut = this->prompt_head.width() - this->wid + 1;
        const uint16_t len_cwd = StringX(info["{cwd}"]).width() - len_cut;

        // Clip length of current directory path and replace.
        info["{cwd}"] = cwd.clip(len_cwd).string();

        prompt1 = StringX(replace_info(config.prompt1, info));
        prompt2 = StringX(replace_info(config.prompt2, info));
        len_ws  = std::max(0, this->wid - prompt1.width() - prompt2.width());
        this->prompt_head = prompt1 + CharX(" ") * len_ws + prompt2;
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
