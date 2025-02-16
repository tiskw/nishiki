////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: text_chooser.cxx                                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "text_chooser.hxx"

// Include the headers of STL.
#include <iostream>

// Include NCurses.
#include <curses.h>

// Include the headers of custom modules.
#include "history_manager.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileChooser: Macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define KEY_CTRL(x) ((x) & 0x1F)

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextChooser: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

TextChooser::TextChooser(void) : idx(0), header(""), is_grep_mode(false)
{   // {{{

    // Get screen size.
    getmaxyx(stdscr, this->h, this->w);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextChooser: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
TextChooser::start(const std::vector<std::string>& lines, int32_t target_index) noexcept
{   // {{{

    // Height of process information list window.
    const int32_t hgt_list = this->h - 4;

    // Initialize returned value.
    std::vector<StringX> result;

    // Initialize index.
    this->idx = 0;

    // Initialize grep status.
    this->is_grep_mode = false;
    this->grep_str     = "";

    // Set header.
    this->header = lines[0];

    // Memorize all texts where initial select flag is false.
    this->items.clear();
    for (uint32_t n = 1; n < lines.size(); ++n)
        this->items.emplace_back(lines[n], false, n - 1);

    // Initialize view.
    this->views.clear();
    for (auto& item : this->items)
        this->views.push_back(&item);

    // Enter to the text chooser session.
    while (true)
    {
        // Update viewed items.
        this->update_viewed_items();

        // Redraw window.
        this->redraw();

        // Get user input.
        const int32_t key = getch();

        // Text chooser has 2 states, normal mode and grep mode.
        if (this->is_grep_mode)
        {
            // Keybind on grep mode.
            switch (key)
            {
                case '\n'         : this->is_grep_mode = false;   break;
                case KEY_BACKSPACE: this->grep_str.pop_back();    break;
                case KEY_CTRL('h'): this->grep_str.pop_back();    break;
                default           : this->grep_str += (char) key; break;
            }
        }
        else
        {
            // Keybind on normal mode.
            switch (key)
            {
                // Keybind to continue the file chooser process.
                case 0x02: this->move_index(-hgt_list); break;
                case 0x06: this->move_index(+hgt_list); break;
                case 'j' : this->move_index(+1);        break;
                case 'k' : this->move_index(-1);        break;
                case ' ' : this->toggle_select();       break;
                case '/' : this->is_grep_mode = true;   break;

                // Keybinds to exit the file chooser process.
                case 'q' : return std::vector<StringX>();
                case '\n': return this->selected_values(target_index);
            }
        }
    }

    return result;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
TextChooser::get_index_on_view(void) const noexcept
{   // {{{

    // Do nothing if the viewed list is empty, or containing only one.
    if (this->views.size() == 0)
        return 0;

    // Initialize range of the interval reduction method.
    std::pair<uint32_t, uint32_t> range = {0, this->views.size() - 1};

    // Continue the interval reduction method while the width of the range is not zero.
    while (range.first != range.second)
    {
        // Compute the middle of the range.
        const uint32_t range_middle = (range.first + range.second) / 2;

        // Compute the corresponding original index for each begin, middle, end of the range.
        const int32_t idx_bgn = std::get<2>(*this->views[range.first]);
        const int32_t idx_mid = std::get<2>(*this->views[range_middle]);
        const int32_t idx_end = std::get<2>(*this->views[range.second]);

        // Finish searching, or update the range.
        if      (idx_bgn == this->idx)    { return range.first;          }
        else if (idx_end == this->idx)    { return range.second;         }
        else if (idx_mid == this->idx)    { return range_middle;         }
        else if (idx_mid <  this->idx)    { range.first  = range_middle; }
        else /* (idx_mid >  this->idx) */ { range.second = range_middle; }
    }

    return range.first;

}   // }}}

void
TextChooser::move_index(int32_t delta) noexcept
{   // {{{

    // Compute index on the viewed list.
    int32_t idx_view = this->get_index_on_view();

    // Update the viewed index.
    idx_view = std::clamp(idx_view + delta, 0, static_cast<int32_t>(this->views.size()) - 1);

    // Map the viewed index to the original index.
    this->idx = std::get<2>(*this->views[idx_view]);

}   // }}}

void
TextChooser::redraw(void) noexcept
{   // {{{

    // Define position and size of drawing area.
    const int32_t x = 1, y = 3;

    // Erase all window contents.
    werase(stdscr);

    // Draw the header line.
    mvwaddstr(stdscr, 0, 1, this->header.c_str());

    // Draw the grep string line.
    mvwaddstr(stdscr, 1, 1, ((this->is_grep_mode ? "/" : "") + this->grep_str).c_str());

    // Compute index on the viewed items.
    const int32_t idx_view = get_index_on_view();

    // Update index.
    this->idx = std::get<2>(*this->views[idx_view]);

    // Compute delta of the index.
    const int32_t idx_delta = idx_view - (idx_view % (this->h - 4));

    // Print contents.
    for (int32_t row = 0; row < MIN((int32_t) (this->views.size() - idx_delta), this->h - 4); ++row)
    {
        // Print contents 1: unpack item.
        const std::string text        = std::get<0>(*(this->views[row + idx_delta]));
        const bool        is_selected = std::get<1>(*(this->views[row + idx_delta]));

        // Print contents 2: create proc info text.
        StringX sx = StringX((is_selected ? "* " : "  ") + text + std::string(w, ' ')).clip(w - 4);

        // Print contents 3: decorate focused item.
        if ((row + idx_delta) == idx_view)
            wattron(stdscr, A_REVERSE);

        // Print contents 4: print text.
        mvwaddstr(stdscr, y + row, x, sx.string().c_str());

        // Print proc contents 5: restore focused item decoration.
        if ((row + idx_delta) == idx_view)
            wattroff(stdscr, A_REVERSE);
    }
 
    // Draw border lines 2: draw horizontal lines.
    mvwhline(stdscr, 2,     1, ACS_HLINE, w - 1);
    mvwhline(stdscr, h - 1, 1, ACS_HLINE, w - 1);

    // Draw border lines 3: draw vertical lines.
    mvwvline(stdscr, 3, 0,     ACS_VLINE, h - 4);
    mvwvline(stdscr, 3, w - 1, ACS_VLINE, h - 4);

    // Draw border lines 4: draw corners and tees.
    mvwaddch(stdscr, 2,     0,     ACS_ULCORNER);
    mvwaddch(stdscr, 2,     w - 1, ACS_URCORNER);
    mvwaddch(stdscr, h - 1, 0,     ACS_LLCORNER);
    mvwaddch(stdscr, h - 1, w - 1, ACS_LRCORNER);

    // Refresh entire window.
    wrefresh(stdscr);

}   // }}}
 
std::vector<StringX>
TextChooser::selected_values(int32_t target_index) const noexcept
{   // {{{

    constexpr auto get_value = [](const std::string& item, int32_t target_index) noexcept -> std::string
    // [Abstract]
    //   Finds process ID on a process information line and returns it.
    //
    // [Args]
    //   item (const std::string&): [IN] Process information (one line of an output of `ps aux` command).
    //
    // [Returns]
    //   (std::string): Process ID.
    {
        // Split the given line of "ps aux" by whiltespace.
        std::vector<std::string> tokens = split(item);

        // Returns all items if the target index is negative, otherwise returns the corresponding item.
        if      (target_index < 0                     ) { return item;                 }
        else if (tokens.size() > (size_t) target_index) { return tokens[target_index]; }
        else                                            { return std::string("");      }
    };

    // Initialize the output list.
    std::vector<StringX> result;

    // Get process ID of all selected processes.
    for (const auto& item : this->items)
    {
        // Unpack item.
        const std::string text        = std::get<0>(item);
        const bool        is_selected = std::get<1>(item);

        if (is_selected)
            result.emplace_back(get_value(text, target_index));
    }

    // Returns current item if nothing is selected.
    if (result.size() == 0)
        result.emplace_back(get_value(std::get<0>(this->items[this->idx]), target_index));

    return result;

}   // }}}

void
TextChooser::toggle_select(void) noexcept
{   // {{{

    // Compute target index.
    const int32_t index = this->idx % this->items.size();

    // Toggle selected/unselected flag.
    std::get<1>(this->items[index]) = !std::get<1>(this->items[index]);

    // Move cursor.
    this->move_index(+1);

}   // }}}

void
TextChooser::update_viewed_items(void) noexcept
{   // {{{

    // Do nothing if not in the grep mode.
    if (not this->is_grep_mode)
        return;
    
    this->views.clear();

    for (auto& item : this->items)
        if (std::get<0>(item).find(this->grep_str) != std::string::npos)
            this->views.push_back(&item);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
choose_hists(void) noexcept
{   // {{{

    // Instanciate history manager class to read history file.
    HistoryManager hist;

    // Get histories.
    std::vector<StringX> hists_x = hist.get_histories();

    // Do nothing and returns empty vector if the command history is empty.
    if (hists_x.size() == 0)
        return hists_x;

    // Insert the first line.
    hists_x.insert(hists_x.begin(), StringX("List of command history"));

    // Convert the history to a vector of std::string.
    constexpr auto to_string = [](const StringX& sx) noexcept -> std::string { return sx.string(); };
    std::vector<std::string> hists = transform<StringX, std::string>(hists_x, to_string);

    // Start the text chooser.
    return TextChooser().start(hists, -1);

}   // }}}

void
choose_hists_and_exit(void) noexcept
{   // {{{

    // Call history chooser and print selected histories.
    for (const StringX& sx : choose_hists())
        std::cout << sx << std::endl;

    // Show terminal cursor forcibly.
    fputs("\033[?25h", stdout);

    exit(EXIT_SUCCESS);

}   // }}}

std::vector<StringX>
choose_procs(void) noexcept
{   // {{{

    // Call process command.
    std::vector<std::string> lines = split(run_command(PS_COMMAND), "\n");

    // Start the text chooser.
    return TextChooser().start(lines, 1);

}   // }}}

void
choose_procs_and_exit(void) noexcept
{   // {{{

    // Call process chooser and print selected process ID(s).
    for (const StringX& sx : choose_procs())
        std::cout << sx << std::endl;

    // Show terminal cursor forcibly.
    fputs("\033[?25h", stdout);

    exit(EXIT_SUCCESS);

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
