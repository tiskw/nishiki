////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: text_buffer.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "text_buffer.hxx"

// Include the headers of custom modules.
#include "char_x.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextBuffer: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

TextBuffer::TextBuffer(const StringX& lhs, const StringX& rhs, const Deque<StringX>& hists) : mode(Mode::INSERT)
{   // {{{

    // Create new buffers in the storage.
    for (const StringX& s : hists)
        this->storage.emplace_back(s, StringX(""));
    this->storage.emplace_back(lhs, rhs);

    // Initialize storage index.
    this->index = this->storage.size() - 1;

    // Update lhs/rhs pointer.
    this->lhs_ptr = &this->storage[this->index].first;
    this->rhs_ptr = &this->storage[this->index].second;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextBuffer: Getter and setter functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Getter for "lhs" and "rhs".
const StringX& TextBuffer::get_lhs (void) const noexcept { return *this->lhs_ptr; }
const StringX& TextBuffer::get_rhs (void) const noexcept { return *this->rhs_ptr; }

// Getter for "mode".
TextBuffer::Mode TextBuffer::get_mode() const noexcept
{ return this->mode; };

// Setter for "mode".
void TextBuffer::set_mode(TextBuffer::Mode mode) noexcept
{ this->mode = mode; };

// Getter for "storage".
const std::vector<std::pair<StringX, StringX>>&
TextBuffer::get_storage(void) const noexcept
{ return this->storage; };

// Setter for "lhs" and "rhs".
void TextBuffer::set(const StringX& lhs, const StringX& rhs) noexcept
{ *this->lhs_ptr = lhs; *this->rhs_ptr = rhs; };

////////////////////////////////////////////////////////////////////////////////////////////////////
// TextBuffer: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void TextBuffer::edit(const CharX& cx) noexcept
{   // {{{

    switch (this->mode)
    {
        case TextBuffer::Mode::INSERT: this->edit_insert(cx); break;
        case TextBuffer::Mode::NORMAL: this->edit_normal(cx); break;
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void TextBuffer::edit_insert(const CharX& cx) noexcept
{   // {{{

    switch (cx.value)
    {
        // Special keys.
        case 0x08: this->lhs_ptr->pop(StringX::Pos::END); break;  // ^H (Backspace)
        case 0x7F: this->lhs_ptr->pop(StringX::Pos::END); break;  // ^? (Backspace)

        // Move cursor.
        case CHARX_VALUE_KEY_RIGHT: this->move_cursor(+1); break;
        case CHARX_VALUE_KEY_LEFT : this->move_cursor(-1); break;

        // Change text buffer.
        case CHARX_VALUE_KEY_DOWN: this->change_buffer(+1); break;
        case CHARX_VALUE_KEY_UP  : this->change_buffer(-1); break;

        // Mode transition.
        case 0x1B: this->mode = TextBuffer::Mode::NORMAL; break;

        // Default: key input.
        default: *(this->lhs_ptr) += cx.printable(); break;
    }

}   // }}}

void TextBuffer::edit_normal(const CharX& cx) noexcept
{   // {{{

    switch (cx.value)
    {
        // Move cursor.
        case 'l': this->move_cursor(+1);                     break;
        case 'h': this->move_cursor(-1);                     break;
        case '$': this->move_cursor( this->rhs_ptr->size()); break;
        case '0': this->move_cursor(-this->lhs_ptr->size()); break;

        // Move cursor with mode transition.
        case 'a': this->move_cursor(+1);                     break;
        case 'A': this->move_cursor( this->rhs_ptr->size()); break;
        case 'I': this->move_cursor(-this->lhs_ptr->size()); break;

        // Change text buffer.
        case 'j': this->change_buffer(+1); break;
        case 'k': this->change_buffer(-1); break;

        // Edit text.
        case 'x': this->rhs_ptr->pop(StringX::Pos::BEGIN); break;

        // Erase line.
        case 'S': this->lhs_ptr->clear(); [[fallthrough]];
        case 'D': this->rhs_ptr->clear(); break;

        // Default: do nothing.
    }

    // Mode transition.
    if ((cx.value == 'a') or (cx.value == 'A') or (cx.value == 'i') or (cx.value == 'I') or (cx.value == 'S'))
        this->mode = TextBuffer::Mode::INSERT;

}   // }}}

void TextBuffer::move_cursor(int16_t delta) noexcept
{   // {{{

    while (delta != 0)
    {
        // Move forward.
        if (delta > 0 and this->rhs_ptr->size() > 0)
            this->lhs_ptr->push_back(this->rhs_ptr->pop(StringX::Pos::BEGIN));

        // Move backward.
        if (delta < 0 and this->lhs_ptr->size() > 0)
            this->rhs_ptr->push_front(this->lhs_ptr->pop(StringX::Pos::END));

        // Update delta.
        delta += (delta > 0) ? -1 : 1;
    }

}   // }}}

void TextBuffer::change_buffer(int16_t delta) noexcept
{   // {{{

    // Update storage index.
    if      (delta > 0 and this->index < (this->storage.size() - 1)) { this->index += 1; }
    else if (delta < 0 and this->index > 0                         ) { this->index -= 1; }

    // Update lhs/rhs pointer.
    this->lhs_ptr = &this->storage[this->index].first;
    this->rhs_ptr = &this->storage[this->index].second;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
