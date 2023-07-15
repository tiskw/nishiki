////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: text_chooser.hxx                                                            ///
///                                                                                              ///
/// This file defines the class `TextChooser` that provides NCurses interface for selecting      ///
/// the given texts.                                                                             ///
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
        start(const std::vector<std::string>& lines)
        noexcept;
        // [Abstract]
        //   Start session and returns selecetd files.
        //
        // [Args]
        //   lines (const std::vector<std::string>&): [IN] Selection targets.
        //
        // [Returns]
        //   (std::vector<StringX>): List of selected items.


    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t idx;
        // [Abstract]
        //   Index of the current focused item.

        int32_t w, h;
        // [Abstract]
        //   Screen size.

        std::vector<std::tuple<std::string, bool, uint32_t>> items;
        // [Abstract]
        //   List of items where each item is composed by a pair of text and selected flag.

        std::vector<std::tuple<std::string, bool, uint32_t>*> views;
        // [Abstract]
        //   List of viewed items.

        std::string header;
        // [Abstract]
        //   Header string.

        bool is_grep_mode;
        // [Abstract]
        //   True if in a grep mode.

        std::string grep_str;
        // [Abstract]
        //   String to filter items.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t
        get_index_on_view(void)
        const noexcept;
        // [Abstract]
        //   Returns the index on viewed items.
        //   If some grep string is set, the number of viewed items is not equivalent with the number of
        //   original items, and this->idx is a index on the original list. This function returns a index
        //   on the viewed list. For searching the viewed index, the interval reduction method is used
        //   in this function.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (int32_t): Index on the viewed list.

        void
        move_index(int32_t delta)
        noexcept;
        // [Abstract]
        //   Move cursor.
        //
        // [Args]
        //   delta (int32_t): [IN] Amount of cursor movement.
        //
        // [Returns]
        //   void

        // Redraw curses window.
        void
        redraw(void)
        noexcept;
        // [Abstract]
        //   Redraw curses window.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void

        std::vector<StringX>
        selected_procs(void)
        const noexcept;
        // [Abstract]
        //   Returns selecetd files.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::vector<StringX>): List of selected process IDs.

        void
        toggle_select(void)
        noexcept;
        // [Abstract]
        //   Toggle selected/unselected flag of the current item.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void

        void
        update_viewed_items(void)
        noexcept;
        // [Abstract]
        //   Update items.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
choose_hists(void)
noexcept;
// [Abstract]
//   Call history chooser and returns choosed file(s).
//
// [Args]
//   void
//
// [Returns]
//   (std::vector<StringX>): A list of selected histories.

void
choose_hists_and_exit(void)
noexcept;
// [Abstract]
//   Call history chooser and prints choosed item(s).
//
// [Args]
//   void
//
// [Returns]
//   void

std::vector<StringX>
choose_procs(void)
noexcept;
// [Abstract]
//   Call process chooser and returns choosed process ID(s).
//
// [Args]
//   void
//
// [Returns]
//   (std::vector<StringX>): A list of selected process ID(s).

void
choose_procs_and_exit(void)
noexcept;
// [Abstract]
//   Call process chooser and prints choosed process ID(s).
//
// [Args]
//   void
//
// [Returns]
//   void


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
