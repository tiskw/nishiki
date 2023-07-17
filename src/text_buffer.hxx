////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: text_buffer.hxx                                                             ///
///                                                                                              ///
/// This file defines the class `TextBuffer` that manages user command editing.                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TEXT_BUFFER_HXX
#define TEXT_BUFFER_HXX

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
        // [Abstract]
        //   Constants to represent state of buffer.


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
        // [Args]
        //   void
        //
        // [Returns]
        //   (const StringX&): Left/right hand side of the text buffer.

        TextBuffer::Mode
        get_mode(void)
        const noexcept;
        // [Abstract]
        //   Returns current editing mode.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (TextBuffer::Mode): Current editing mode.

        const std::vector<std::pair<StringX, StringX>>&
        get_storage(void)
        const noexcept;
        // [Abstract]
        //   Returns all text buffers.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (const std::vector<std::pair<StringX, StringX>>&): Text buffers.

        void set_lhs(const StringX& text) noexcept;
        void set_rhs(const StringX& text) noexcept;
        // [Abstract]
        //   Set left/right hand side of the text buffer.
        //
        // [Args]
        //   text (const StringX&): Text to be set.
        //
        // [Returns]
        //   void

        void
        set_mode(TextBuffer::Mode)
        noexcept;
        // [Abstract]
        //   Set editing mode.
        //
        // [Args]
        //   (TextBuffer::Mode): Editing mode.
        //
        // [Returns]
        //   void

        ////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////

        StringX
        string()
        const noexcept;
        // [Abstract]
        //   Convert buffet to a string.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (StringX): String expression of text buffer.

        void
        edit(const CharX& cx)
        noexcept;
        // [Abstract]
        //   Edit buffer.
        //
        // [Args]
        //   cx (CharX): [IN] Input charactor.
        //
        // [Returns]
        //   void

        void
        create(const StringX& lhs, const StringX& rhs)
        noexcept;
        // [Abstract]
        //   Create new editing buffer.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Initial left hand side of cursor.
        //   rhs (const StringX&): [IN] Initial right hand side of cursor.
        //
        // [Returns]
        //   void


    private:

        ////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////

        StringX *lhs_ptr, *rhs_ptr;
        // [Abstract]
        //   Left-hand-side and right-hand-side of the cursor.
        //   This is actually a pointer to string instance in `this->storage`.

        TextBuffer::Mode mode;
        // [Abstract]
        //   Editing mode.

        std::vector<std::pair<StringX, StringX>> storage;
        // [Abstract]
        //   Buffer storage.

        uint32_t index;
        // [Abstract]
        //   Current index of histories.

        ////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////

        void
        edit_insert(const CharX& cx)
        noexcept;
        // [Abstract]
        //   Edit buffer (insert mode).
        //
        // [Args]
        //   cx (const CharX): [IN] Input character.
        //
        // [Returns]
        //   void

        // Edit function (normal mode).
        void
        edit_normal(const CharX& cx)
        noexcept;
        // [Abstract]
        //  Edit buffer (normal mode).
        //
        // [Args]
        //   cx (const CharX): [IN] Input character.
        //
        // [Returns]
        //   void

        void
        move_cursor(int16_t delta)
        noexcept;
        // [Abstract]
        //   Move cursor.
        //
        // [Args]
        //   delta (int16_t): [IN] Amount of cursor move.
        //
        // [Returns]
        //   void

        void
        change_buffer(int16_t delta)
        noexcept;
        // [Abstract]
        //   Change buffer.
        //
        // [Args]
        //   delta (int16_t): [IN] Amount of cursor move.
        //
        // [Returns]
        //   void
};


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
