////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ header file: text_buffer.hxx
//
// This file defines the class `TextBuffer` that manages user command editing.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_BUFFER_HXX
#define TEXT_BUFFER_HXX

#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextBuffer: A class for managing editing buffer
////////////////////////////////////////////////////////////////////////////////////////////////////
class TextBuffer
{
    public:

        ////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////

        enum class Mode { INSERT, NORMAL };

        ////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////

        explicit TextBuffer(const TextBuffer::Mode mode = TextBuffer::Mode::INSERT);

        ////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////

        Mode get_mode(void) const noexcept;

        const StringX& get_lhs(void) const noexcept;
        const StringX& get_rhs(void) const noexcept;

        const std::vector<std::pair<StringX, StringX>>& get_storage(void) const noexcept;

        void set_lhs(const StringX& lhs) noexcept;
        void set_rhs(const StringX& rhs) noexcept;

        ////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////

        // Convert buffet to a string.
        StringX
        string()
        const noexcept;

        // Edit function.
        void
        edit(const CharX& cx)
        noexcept;

        // Create new buffer and go to it.
        void
        create(const StringX& lhs, const StringX& rhs)
        noexcept;

    private:

        ////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////

        // Left-hand-side and right-hand-side of the cursor.
        // This is actually a pointer to string instance in `this->storage`.
        StringX *lhs_ptr, *rhs_ptr;

        // Editing mode.
        TextBuffer::Mode mode;

        // Buffer storage.
        std::vector<std::pair<StringX, StringX>> storage;

        // Current index of histories.
        uint32_t index;

        ////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////

        // Edit function (insert mode).
        void
        edit_insert(const CharX& cx)
        noexcept;

        // Edit function (normal mode).
        void
        edit_normal(const CharX& cx)
        noexcept;

        // Move cursor.
        void
        move_cursor(int16_t delta)
        noexcept;

        // Change buffer.
        void
        change_buffer(int16_t delta)
        noexcept;
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
