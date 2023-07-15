////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: file_chooser.hxx                                                            ///
///                                                                                              ///
/// This file defines the class `FileChooser` that provides NCurses interface for selecting      ///
/// files and directories.                                                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_CHOOSER_HXX
#define FILE_CHOOSER_HXX

#include "curses_screen.hxx"

#include <filesystem>

#include "directory.hxx"
#include "string_x.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class FileChooser : public CursesScreen
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and descructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        FileChooser(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<StringX>
        start(const StringX& root)
        noexcept;
        // [Abstract]
        //   Start session and returns selecetd files.
        //
        // [Args]
        //   root (const StringX&): [IN] Initial directory of the file chooser.
        //
        // [Returns]
        //   (std::vector<StringX>): List of selected files.
        //

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t w, h;
        // [Abstract]
        //   Size of screen.

        int32_t index_cdir;
        int32_t index_prev;
        // [Abstract]
        //   Index of selected file.

        Directory cdir;
        Directory prev;
        // [Abstract]
        //   Directory instance.

        std::vector<bool> selected;
        // [Abstract]
        //   List of selected/unselected flags.

        bool is_grep_mode;
        // [Abstract]
        //   Grep mode flag.

        std::string grep_str;
        // [Abstract]
        //   Grep string.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void chdir_next(void) noexcept;
        void chdir_prev(void) noexcept;
        // [Abstract]
        //   Move to the next/previous directory.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void

        void
        move_index_cdir(const int32_t delta)
        noexcept;
        // [Abstract]
        //   Move cursor. User can specify the amount of movement.
        //
        // [Args]
        //   delta (const int32_t): [IN] Amount of cursor movement.
        //
        // [Returns]
        //   void

        void
        print_dir_list(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const Directory& dir, const int32_t idx, const bool sel)
        const noexcept;
        // [Abstract]
        //   Print list to window.
        //
        // [Args]
        //   x   (const int32_t)    : [IN] Left-top position of drawing area.
        //   y   (const int32_t)    : [IN] Left-top position of drawing area.
        //   w   (const int32_t)    : [IN] Width of drawing area.
        //   h   (const int32_t)    : [IN] Height of drawing area.
        //   dir (const Directorty&): [IN] List of items.
        //   idx (const int32_t)    : [IN] Index of forcused item.
        //   sel (const bool)       : [IN] Show selected/unselected marker if true.
        //
        // [Returns]
        //   void

        void
        print_preview(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const std::string& path)
        const noexcept;
        // [Abstract]
        //   Print preview window.
        //
        // [Args]
        //   x    (const int32_t)     : [IN] Left-top position of drawing area.
        //   y    (const int32_t)     : [IN] Left-top position of drawing area.
        //   w    (const int32_t)     : [IN] Width of drawing area.
        //   h    (const int32_t)     : [IN] Height of drawing area.
        //   path (const std::string&): [IN] Path to the current file.
        //
        // [Returns]
        //   void

        void
        redraw(void)
        const noexcept;
        // [Abstract]
        //   Redraw curses window.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   void

        std::vector<StringX>
        selected_files(const std::filesystem::path root)
        const noexcept;
        // [Abstract]
        //   Returns selecetd files.
        //
        // [Args]
        //   path (std::filesystem::path): [IN] Root of relative path.
        //
        // [Returns]
        //   (std::vector<StringX>): A vector of selected items.

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
        //

        void
        update(void)
        noexcept;
        // [Abstract]
        //   Update directory contents.
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
choose_files(const StringX& root)
noexcept;
// [Abstract]
//   Call file chooser and returns choosed file(s).
//
// [Args]
//   root (const StringX&): [IN] Path of the starting directory.
//
// [Returns]
//   (std::vector<StringX>): A list of choosed file paths.

void
choose_files_and_exit(const StringX& root)
noexcept;
// [Abstract]
//   Call file chooser and prints choosed file(s).
//
// [Args]
//   root (const StringX&): [IN] Path of the starting directory.
//
// [Returns]
//   void


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
