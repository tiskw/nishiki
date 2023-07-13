////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: term_writer.cxx
//
// This file defines the class `TermWriter` that manages output to terminal.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "term_writer.hxx"

#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

#include "config.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Default constructor.
//
// [Args]
//   void
//
TermWriter::TermWriter(void)
{
    // Get terminal size.
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    this->hgt = ws.ws_row;
    this->wid = ws.ws_col;

    // Hide cursor.
    std::cout << "\033[?25l";

    // Update auxiliary information.
    this->update();

    // Move the cursor to the bottom of the drawing area.
    for (uint16_t n = 0; n < config.area_hgt; ++n)
        std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Destructor.
//
// [Args]
//   void
//
TermWriter::~TermWriter(void)
{
    // Clear drawing area.
    std::cout << "\033[0J";

    // Show cursor.
    std::cout << "\033[?25h";

    // Erase drawing area.
    std::cout << "\033[" << config.area_hgt << "F" << "\033[0J" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Write the given contents to the terminal.
// Layout of the contents is the following.
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
//
void
TermWriter::write(const StringX& lhs, const StringX& rhs, TextBuffer::Mode mode, const std::vector<StringX>& clines, const StringX& hist_comp)
const noexcept
{
    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Computes and returns editing line.
    //
    // [Args]
    //   lhs       (const StringX&): [IN] Left hand side of the cursor of the edit string.
    //   rhs       (const StringX&): [IN] Right hand side of the cursor of the edit string.
    //   hist_comp (const StringX&): [IN] History completion.
    //
    // [Returns]
    //   (StringX): Editing line.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto generate_editing_line = [](const StringX& lhs, const StringX& rhs, const StringX& hist_comp)
    {
        // Case 1: only `lhs` is non-empty string.
        if ((rhs.size() == 0) and (hist_comp.size() == 0))
            return lhs.colorize() + StringX("\033[7m \033[m");

        // Case 2: `rhs` is empty.
        else if (rhs.size() == 0)
            return lhs.colorize() + StringX("\033[7m") + hist_comp.front() + StringX("\033[m\033[97m") + hist_comp.substr(1) + StringX("\033[m");

        // Case 3: others.
        else
            return (lhs + CharX("\033[7m") + rhs.front() + CharX("\033[27m") + rhs.substr(1)).colorize();
    };

    // Computes editing line.
    StringX eline = generate_editing_line(lhs, rhs, hist_comp);

    // Append Prompt before the editing line.
    if      (mode == TextBuffer::Mode::INSERT) eline = StringX(config.prompt3_ins) + eline;
    else if (mode == TextBuffer::Mode::NORMAL) eline = StringX(config.prompt3_nor) + eline;

    // Print header, prompts, editing line, and completion candidates.
    std::cout << "\033[" << config.area_hgt << "F" << std::endl;
    std::cout << this->prompt_head     << "\033[0K" << std::endl;
    std::cout << eline.clip(this->wid) << "\033[0K" << std::endl;

    for (size_t i = 0; i < clines.size(); ++i)
    {
        // Write completion lines.
        std::cout << clines[i] << "\033[0K";

        // Do not write newline at the end of completion lines.
        if (i == (clines.size() - 1)) std::cout << std::flush;
        else                          std::cout << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TermWriter: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

///// FUNCTION /////
//
// Update auxiliary information.
//
// [Args]
//   void
//
// [Returns]
//   void
//
void
TermWriter::update(void)
noexcept
{
    // A container to store auxiliary information.
    std::map<std::string, std::string> info;

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // A function to replace auxiliary information tag.
    //
    // [Args]
    //   src (const std::string&): [IN] Source string.
    //
    // [Returns]
    //   (std::string): Replaced string.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto replace_info = [](const std::string& src, const std::map<std::string, std::string>& info)
    {
        std::string result = src;

        for (auto pair : info)
            result = replace(result, pair.first, pair.second);

        return result;
    };

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
    size_t  len_ws  = MAX(0, this->wid - prompt1.width() - prompt2.width() - 1);
    this->prompt_head = prompt1 + CharX(" ") * len_ws + prompt2;

    if (this->prompt_head.width() >= this->wid)
    {
        // Convert current directory string to StringX instance.
        StringX cwd = StringX(info["{cwd}"]);

        // Determine width of current working directory path.
        uint16_t len_cut = this->prompt_head.width() - this->wid + 1;
        uint16_t len_cwd = StringX(info["{cwd}"]).width() - len_cut;

        // Clip length of current directory path and replace.
        info["{cwd}"] = cwd.clip(len_cwd).string();

        prompt1 = StringX(replace_info(config.prompt1, info));
        prompt2 = StringX(replace_info(config.prompt2, info));
        len_ws  = MAX(0, this->wid - prompt1.width() - prompt2.width());
        this->prompt_head = prompt1 + CharX(" ") * len_ws + prompt2;
    }
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
