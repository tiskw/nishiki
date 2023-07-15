////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: file_chooser.hxx                                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "file_chooser.hxx"

#include <iostream>
#include <ncurses.h>

#include "preview.hxx"
#include "utils.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// FileChooser: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

FileChooser::FileChooser() : index_cdir(0), index_prev(0), is_grep_mode(false)
{   // {{{

    // Get screen size.
    getmaxyx(stdscr, this->h, this->w);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// FileChooser: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
FileChooser::start(const StringX& root)
noexcept
{   // {{{

    // Height of process information list window.
    const int32_t hgt_list = this->h - 4;

    // Initialize indices.
    this->index_cdir = 0;
    this->index_prev = 0;

    // Set current directory.
    this->cdir.set(std::filesystem::path(root.string()));

    // Enter to the loop of user input and status update.
    while (true)
    {
        // Update member variables.
        this->update();

        // Redraw window.
        this->redraw();

        // Get user input.
        int32_t key = getch();

        // The file chooser has two modes, normal mode and grep mode. 
        if (this->is_grep_mode)
        {
            // Keybind on grep mode.
            switch (key)
            {
                case '\n': this->is_grep_mode = false;   break;
                case  127: this->grep_str.pop_back();    break;
                default  : this->grep_str += (char) key; break;
            }

            // If still in the grep mode.
            if (this->is_grep_mode)
            {
                for (uint32_t n = this->index_cdir; n < this->cdir.size(); ++n)
                    if (this->cdir[n].find(this->grep_str) != std::string::npos)
                        { this->index_cdir = n; break; }
            }

            // If exited from grep mode.
            else
            {
                this->grep_str = "";
            }
        }
        else
        {
            // Keybind on normal mode.
            switch (key)
            {
                // Keybind to continue the file chooser process.
                case 0x02: this->move_index_cdir(-hgt_list);         break;
                case 0x06: this->move_index_cdir(+hgt_list);         break;
                case '/' : this->is_grep_mode = true;                break;
                case '-' : this->chdir_prev();                       break;
                case 'G' : this->index_cdir = this->cdir.size() - 1; break;
                case 'h' : this->chdir_prev();                       break;
                case 'j' : this->move_index_cdir(+1);                break;
                case 'k' : this->move_index_cdir(-1);                break;
                case 'l' : this->chdir_next();                       break;
                case '0' : this->index_cdir = 0;                     break;
                case ' ' : this->toggle_select();                    break;

                // Keybinds to exit the file chooser process.
                case 'q' : return std::vector<StringX>();
                case '\n': return this->selected_files(root.string());
            }
        }
    }

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
FileChooser::chdir_next(void)
noexcept
{   // {{{

    // Do nothing if the directory is empty.
    if ((this->cdir.size() == 1) && (this->cdir[0] == DIRECTORY_NO_ITEM))
        return;

    // DO nothing if the selected item is an empty string.
    if (this->cdir[this->index_cdir].size() == 0)
        return;

    // Get the string of the selected item.
    std::string s = this->cdir[this->index_cdir];

    // Strip '/' at the end of the string if exists.
    if (s.back() == '/')
        s.pop_back();

    // Append the item string to the path.
    // If the directory is actually changed, reset index.
    if (this->cdir.append(s))
        this->index_cdir = 0;

}   // }}}

void
FileChooser::chdir_prev(void)
noexcept
{   // {{{

    // Get the previous path and set it to the current directory variable.
    this->cdir.set(std::filesystem::canonical(this->cdir.get().parent_path()));

    // Update the current index.
    this->index_cdir = this->index_prev;

}   // }}}

void
FileChooser::move_index_cdir(const int32_t delta)
noexcept
{   // {{{

    this->index_cdir = MAX(0, MIN((int32_t) this->index_cdir + delta, (int32_t) this->cdir.size() - 1));

}   // }}}

void
FileChooser::print_dir_list(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const Directory& dir, const int32_t idx, const bool sel)
const noexcept
{   // {{{

    // Compute delta of the index.
    const int32_t idx_delta = idx - (idx % h);

    // Print directory contents.
    for (int32_t row = 0; row < h; ++row)
    {
        // Print directory contents 1: exit for-loop if no item remained.
        if (((uint32_t) (row + idx_delta)) >= dir.size())
            break;

        // Print directory contents 2: determine text to print.
        StringX text = StringX(dir[row + idx_delta] + std::string(w, ' '));
        if (sel == true)
            text = ((this->selected[row + idx_delta]) ? StringX("* ") : StringX("  ")) + text;

        // Print directory contents 3: colorize text.
        wattrset(stdscr, COLOR_PAIR(dir.color(row + idx_delta)));

        // Print directory contents 4: decorate focused item.
        if ((row + idx_delta) == idx)
            wattron(stdscr, A_REVERSE);

        // Print directory contents 5: print text.
        mvwaddstr(stdscr, y + row, x, text.clip(w).string().c_str());

        // Print directory contents 6: restore focused item decoration.
        if ((row + idx_delta) == idx)
            wattroff(stdscr, A_REVERSE);
    }

}   // }}}

void
FileChooser::print_preview(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const std::string& path)
const noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Set curses color.
    //
    // [Args]
    //   color_code (const int16_t): [IN] Color code (ANSI SGR style).
    //
    // [Returns]
    //   void
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto set_color = [](const int16_t color_code) noexcept
    {
        switch (color_code)
        {
            case  0: wattrset(stdscr, COLOR_PAIR(0)); break;
            case 30: wattrset(stdscr, COLOR_PAIR(0)); break;
            case 31: wattrset(stdscr, COLOR_PAIR(1)); break;
            case 32: wattrset(stdscr, COLOR_PAIR(2)); break;
            case 33: wattrset(stdscr, COLOR_PAIR(3)); break;
            case 34: wattrset(stdscr, COLOR_PAIR(4)); break;
            case 35: wattrset(stdscr, COLOR_PAIR(5)); break;
            case 36: wattrset(stdscr, COLOR_PAIR(6)); break;
            case 37: wattrset(stdscr, COLOR_PAIR(7)); break;
            default: wattrset(stdscr, A_NORMAL);      break;
        }
    };
 
    // Get preview results as a vector of string.
    std::vector<StringX> lines = preview(path, w, h);

    // Draw preview results.
    for (size_t row = 0; row < lines.size(); ++row)
    {
        // Initialize color.
        wattrset(stdscr, A_NORMAL);

        // Initialize column position.
        int16_t col = 0;

        for (const CharX& cx : lines[row])
        {
            if (cx.width == 0)
            {
                // Parse ANSI color sequence.
                std::pair<int16_t, int16_t> color = cx.ansi_color();

                // Set color.
                set_color(color.first);
                set_color(color.second);
            }
            else
            {
                // Print the character.
                mvwaddstr(stdscr, y + row, x + col, cx.string().c_str());

                // Move column position.
                col += cx.size;
            }
        }
    }

}   // }}}

void
FileChooser::redraw(void)
const noexcept
{   // {{{

    // Get screen size.
    int32_t w, h;
    getmaxyx(stdscr, h, w);

    // Compute size of each sub-window.
    int32_t w1 = w * 0.15;
    int32_t w2 = w * 0.45;
    int32_t w3 = w - w1 - w2;

    // Erase all window contents.
    werase(stdscr);

    // Print current directory path.
    mvwaddstr(stdscr, 0, 0, (this->cdir.get().string() + "/").c_str());

    // Print pattern string.
    waddstr(stdscr, this->grep_str.c_str());

    // Print asterisk mark with highlight if grep mode.
    if (this->is_grep_mode)
        wattron(stdscr, A_REVERSE);
    waddstr(stdscr, "*");
    wattroff(stdscr, A_REVERSE);

    // Print current directory contents.
    this->print_dir_list(w1 + 1, 2, w2 - 2, h - 3, this->cdir, this->index_cdir, true);

    // Print previous directory contents.
    this->print_dir_list(1, 2, w1 - 2, h - 3, this->prev, this->index_prev, false);

    // Print preview contents.
    this->print_preview(w1 + w2 + 1, 2, w3 - 2, h - 3, this->cdir.get().string() + "/" + this->cdir[this->index_cdir]);

    // Draw border lines 1: restore attributes.
    attrset(A_NORMAL);

    // Draw border lines 2: draw horizontal lines.
    mvwhline(stdscr, 1,     1,           ACS_HLINE, w1 - 1);
    mvwhline(stdscr, h - 1, 1,           ACS_HLINE, w1 - 1);
    mvwhline(stdscr, 1,     1 + w1,      ACS_HLINE, w2 - 1);
    mvwhline(stdscr, h - 1, 1 + w1,      ACS_HLINE, w2 - 1);
    mvwhline(stdscr, 1,     1 + w1 + w2, ACS_HLINE, w3 - 2);
    mvwhline(stdscr, h - 1, 1 + w1 + w2, ACS_HLINE, w3 - 2);

    // Draw border lines 3: draw vertical lines.
    mvwvline(stdscr, 2, 0,       ACS_VLINE, h - 3);
    mvwvline(stdscr, 2, w1,      ACS_VLINE, h - 3);
    mvwvline(stdscr, 2, w1 + w2, ACS_VLINE, h - 3);
    mvwvline(stdscr, 2, w - 1  , ACS_VLINE, h - 3);

    // Draw border lines 4: draw corners and tees.
    mvwaddch(stdscr, 1,     0,       ACS_ULCORNER);
    mvwaddch(stdscr, 1,     w1,      ACS_TTEE);
    mvwaddch(stdscr, 1,     w1 + w2, ACS_TTEE);
    mvwaddch(stdscr, 1,     w - 1,   ACS_URCORNER);
    mvwaddch(stdscr, h - 1, 0,       ACS_LLCORNER);
    mvwaddch(stdscr, h - 1, w1,      ACS_BTEE);
    mvwaddch(stdscr, h - 1, w1 + w2, ACS_BTEE);
    mvwaddch(stdscr, h - 1, w - 1,   ACS_LRCORNER);

    // Refresh entire window.
    wrefresh(stdscr);

}   // }}}

std::vector<StringX>
FileChooser::selected_files(std::filesystem::path root)
const noexcept
{   // {{{

    // Initialize returned value.
    std::vector<StringX> result;

    // Collect selected items.
    for (uint32_t i = 0; i < this->cdir.size(); ++i)
        if (this->selected[i % this->selected.size()])
            result.emplace_back(this->cdir.get_relative(i, root));

    // Returns current item if nothing is selected.
    if (result.size() == 0)
    {
        // Get current path as a string.
        std::string s = this->cdir.get_relative(this->index_cdir, root);

        // If the string contains whitespace, then enclose it by single quote.
        if (s.find_first_not_of(" \t") != std::string::npos)
            s = '\'' + s + '\'';

        // Add to the result list.
        result.emplace_back(s);
    }

    return result;

}   // }}}

void
FileChooser::toggle_select(void)
noexcept
{   // {{{

    // Compute target index.
    int32_t index = this->index_cdir % this->cdir.size();

    // Toggle selected/unselected flag.
    this->selected[index] = !(this->selected[index]);

    // Move cursor.
    this->move_index_cdir(+1);

}   // }}}

void
FileChooser::update(void)
noexcept
{   // {{{

    // Set previous directory.
    this->prev.set(this->cdir.get().parent_path());

    // Update directory contents.
    const bool updated_cdir = this->cdir.update();
    const bool updated_prev = this->prev.update();

    // Append '/' at the end of directory name if not exists.
    std::string name = this->cdir.get().filename().string();
    if (name.back() != '/')
        name.push_back('/');

    // Find appropriate location of the focus index of previous directory.
    if (updated_prev)
    {
        for (uint32_t n = 0; n < this->prev.size(); ++n)
            if (this->prev[n] == name)
                { this->index_prev = n; break; }
    }

    // Clean and initialize selected vector.
    if (updated_cdir)
    {
        this->selected.clear();
        this->selected.resize(this->cdir.size(), false);
    }

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
choose_files(const StringX& root)
noexcept
{   // {{{

    return FileChooser().start(root);

}   // }}}

void
choose_files_and_exit(const StringX& root)
noexcept
{   // {{{

    for (const StringX& sx : choose_files(root))
        std::cout << sx << std::endl;

    exit(EXIT_SUCCESS);

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
