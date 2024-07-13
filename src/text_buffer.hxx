////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: text_buffer.hxx                                                             ///
///                                                                                              ///
/// This file defines the class `TextBuffer` that manages user command editing.                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_BUFFER_HXX
#define TEXT_BUFFER_HXX

// Include the headers of STL.
#include <cstdint>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class TextBuffer
{
    public:

        ////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////

        enum class Mode { INSERT, NORMAL };
        // Constants to represent state of buffer.

        ////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////

        explicit TextBuffer(const TextBuffer::Mode mode = TextBuffer::Mode::INSERT);
        // [Abstract]
        //   Default constructor of TextBuffer.
        //
        // [Args]
        //   mode (TextBuffer::Mode): [IN] Initial mode.

        ////////////////////////////////////////////////////////////////////////
        // Getter and setter functions
        ////////////////////////////////////////////////////////////////////////

        const StringX& get_lhs(void) const noexcept;
        const StringX& get_rhs(void) const noexcept;
        // [Abstract]
        //   Returns left/right hand side of the text buffer.
        //
        // [Returns]
        //   (const StringX&): Left/right hand side of the text buffer.

        TextBuffer::Mode
        get_mode(void) const noexcept;
        // [Abstract]
        //   Returns current editing mode.
        //
        // [Returns]
        //   (TextBuffer::Mode): Current editing mode.

        const std::vector<std::pair<StringX, StringX>>&
        get_storage(void) const noexcept;
        // [Abstract]
        //   Returns all text buffers.
        //
        // [Returns]
        //   (const std::vector<std::pair<StringX, StringX>>&): Text buffers.

        void
        set(const StringX& lhs, const StringX& rhs) noexcept;
        // [Abstract]
        //   Set left/right hand side of the text buffer.
        //
        // [Args]
        //   lhs (const StringX&): Left-hand-side text to be set.
        //   rhs (const StringX&): Right-hand-side text to be set.

        void
        set_mode(TextBuffer::Mode) noexcept;
        // [Abstract]
        //   Set editing mode.
        //
        // [Args]
        //   (TextBuffer::Mode): Editing mode.

        ////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////

        void
        edit(const CharX& cx) noexcept;
        // [Abstract]
        //   Edit buffer.
        //
        // [Args]
        //   cx (CharX): [IN] Input charactor.

        void
        create(const StringX& lhs, const StringX& rhs) noexcept;
        // [Abstract]
        //   Create new editing buffer.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Initial left hand side of cursor.
        //   rhs (const StringX&): [IN] Initial right hand side of cursor.

    private:

        ////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////

        StringX *lhs_ptr, *rhs_ptr;
        // Left-hand-side and right-hand-side of the cursor.
        // This is actually a pointer to string instance in `this->storage`.

        TextBuffer::Mode mode;
        // Editing mode.

        std::vector<std::pair<StringX, StringX>> storage;
        // Buffer storage.

        uint32_t index;
        // Current index of histories.

        ////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////

        void
        edit_insert(const CharX& cx) noexcept;
        // [Abstract]
        //   Edit buffer (insert mode).
        //
        // [Args]
        //   cx (const CharX): [IN] Input character.

        // Edit function (normal mode).
        void
        edit_normal(const CharX& cx) noexcept;
        // [Abstract]
        //   Edit buffer (normal mode).
        //
        // [Args]
        //   cx (const CharX): [IN] Input character.

        void
        move_cursor(int16_t delta) noexcept;
        // [Abstract]
        //   Move cursor.
        //
        // [Args]
        //   delta (int16_t): [IN] Amount of cursor move.

        void
        change_buffer(int16_t delta) noexcept;
        // [Abstract]
        //   Change buffer.
        //
        // [Args]
        //   delta (int16_t): [IN] Amount of cursor move.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
