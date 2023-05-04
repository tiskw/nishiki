////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ header file: text_chooser.hxx
//
// This file defines the class `TextChooser` that provides NCurses interface for selecting
// the given texts.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_CHOOSER_HXX
#define TEXT_CHOOSER_HXX

#include "curses_screen.hxx"

#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

#define PS_COMMAND ("ps aux")

////////////////////////////////////////////////////////////////////////////////////////////////////
// ProcChooser: A class for choosing process ID
////////////////////////////////////////////////////////////////////////////////////////////////////
class TextChooser : public CursesScreen
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        TextChooser(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Start session and returns selecetd files.
        std::vector<StringX>
        start(const std::vector<std::string>& lines)
        noexcept;

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Index of the current focused item.
        int32_t idx;

        // Screen size.
        int32_t w, h;

        // List of items where each item is composed by a pair of text and selected flag.
        std::vector<std::tuple<std::string, bool, uint32_t>> items;

        // List of viewed items.
        std::vector<std::tuple<std::string, bool, uint32_t>*> views;

        // Header string.
        std::string header;

        // True if in a grep mode.
        bool is_grep_mode;

        // String to filter items.
        std::string grep_str;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Compute index on viewed items.
        int32_t
        get_index_on_view(void)
        const noexcept;

        // Move cursor.
        void
        move_index(int32_t delta)
        noexcept;

        // Redraw curses window.
        void
        redraw(void)
        noexcept;

        // Returns selecetd files.
        std::vector<StringX>
        selected_procs(void)
        const noexcept;

        // Toggle selected/unselected flag of the current item.
        void
        toggle_select(void)
        noexcept;

        // Update items.
        void
        update_viewed_items(void)
        noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Call hitsory chooser and returns choosed history(s).
std::vector<StringX> choose_hists(void) noexcept;

// Call history chooser and prints choosed history(s).
void choose_hists_and_exit(void) noexcept;

// Call process chooser and returns choosed process ID(s).
std::vector<StringX> choose_procs(void) noexcept;

// Call process chooser and prints choosed process ID(s).
void choose_procs_and_exit(void) noexcept;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
