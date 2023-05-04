////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ header file: file_chooser.hxx
//
// This file defines the class `FileChooser` that provides NCurses interface for selecting files
// and directories.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_CHOOSER_HXX
#define FILE_CHOOSER_HXX

#include "curses_screen.hxx"

#include <filesystem>

#include "directory.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CursesScreen: A class for Curses stdscr
////////////////////////////////////////////////////////////////////////////////////////////////////
class FileChooser : public CursesScreen
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        FileChooser(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Start session and returns selecetd files.
        std::vector<StringX>
        start(const StringX& root)
        noexcept;

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Size of screen.
        int32_t w, h;

        // Index of selected file.
        int32_t index_cdir;
        int32_t index_prev;

        // Directory instance.
        Directory cdir;
        Directory prev;

        // List of selected/unselected flags.
        std::vector<bool> selected;

        // Grep mode flag.
        bool is_grep_mode;

        // Grep string.
        std::string grep_str;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Change directory.
        void chdir_next() noexcept;
        void chdir_prev() noexcept;

        // Move cursor.
        void
        move_index_cdir(const int32_t delta)
        noexcept;

        // Print list to window.
        void
        print_dir_list(int32_t x, int32_t y, int32_t w, int32_t h, const Directory& dir, int32_t idx, bool sel)
        const noexcept;

        // Print preview window.
        void
        print_preview(int32_t x, int32_t y, int32_t w, int32_t h, const std::string& path)
        const noexcept;

        // Redraw curses window.
        void
        redraw(void)
        const noexcept;

        // Returns selecetd files.
        std::vector<StringX>
        selected_files(const std::filesystem::path root)
        const noexcept;

        // Update directory contents.
        void
        update(void)
        noexcept;

        // Toggle selected/unselected flag of the current item.
        void
        toggle_select(void)
        noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Call file chooser and returns choosed file(s).
std::vector<StringX>
choose_files(const StringX& root)
noexcept;

// Call file chooser and prints choosed file(s).
void
choose_files_and_exit(const StringX& root)
noexcept;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
