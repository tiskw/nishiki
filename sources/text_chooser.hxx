////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: text_chooser.hxx                                                            ///
///                                                                                              ///
/// This file defines the class `TextChooser` that provides NCurses interface for selecting      ///
/// the given texts.                                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_CHOOSER_HXX
#define TEXT_CHOOSER_HXX

// Include the headers of STL.
#include <cstdint>

// Include the headers of custom modules.
#include "curses_screen.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

#define PS_COMMAND ("ps aux")

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class TextChooser : public CursesScreen
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        TextChooser(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<StringX>
        start(const std::vector<std::string>& lines, int32_t target_index) noexcept;
        // [Abstract]
        //   Start session and returns selecetd files.
        //
        // [Args]
        //   lines        (const std::vector<std::string>&): [IN] Selection targets.
        //   target_index (int)                            : [IN] Index of the target column.
        //                                                        Note that -1 means enter line.
        //
        // [Returns]
        //   (std::vector<StringX>): List of selected items.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t idx;
        // Index of the current focused item.

        int32_t w, h;
        // Screen size.

        std::vector<std::tuple<std::string, bool, uint32_t>> items;
        // List of items where each item is composed by a pair of text and selected flag.

        std::vector<std::tuple<std::string, bool, uint32_t>*> views;
        // List of viewed items.

        std::string header;
        // Header string.

        bool is_grep_mode;
        // True if in a grep mode.

        std::string grep_str;
        // String to filter items.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t
        get_index_on_view(void) const noexcept;
        // [Abstract]
        //   Returns the index on viewed items. If some grep string is set, the number of viewed
        //   items is not equivalent with the number of original items, and this->idx is a index
        //   on the original list. This function returns a index on the viewed list. For searching
        //   the viewed index, the interval reduction method is used in this function.
        //
        // [Returns]
        //   (int32_t): Index on the viewed list.

        void
        move_index(int32_t delta) noexcept;
        // [Abstract]
        //   Move cursor.
        //
        // [Args]
        //   delta (int32_t): [IN] Amount of cursor movement.

        // Redraw curses window.
        void
        redraw(void) noexcept;
        // [Abstract]
        //   Redraw curses window.

        std::vector<StringX>
        selected_values(int32_t target_index) const noexcept;
        // [Abstract]
        //   Returns selecetd values.
        //
        // [Args]
        //   target_index (int32_t): [IN] Index of the target value.
        //
        // [Returns]
        //   (std::vector<StringX>): List of selected values.

        void
        toggle_select(void) noexcept;
        // [Abstract]
        //   Toggle selected/unselected flag of the current item.

        void
        update_viewed_items(void) noexcept;
        // [Abstract]
        //   Update items.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
choose_hists(void) noexcept;
// [Abstract]
//   Call history chooser and returns choosed file(s).
//
// [Returns]
//   (std::vector<StringX>): A list of selected histories.

void
choose_hists_and_exit(void) noexcept;
// [Abstract]
//   Call history chooser and prints choosed item(s).

std::vector<StringX>
choose_procs(void) noexcept;
// [Abstract]
//   Call process chooser and returns choosed process ID(s).
//
// [Returns]
//   (std::vector<StringX>): A list of selected process ID(s).

void
choose_procs_and_exit(void) noexcept;
// [Abstract]
//   Call process chooser and prints choosed process ID(s).
//
// [Returns]
//   void

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
