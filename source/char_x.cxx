////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: char_x.cxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "char_x.hxx"

// Include the headers of STL.
#include <sstream>

// Include the headers of custom modules.
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX::CharX(const CharX& cx) : value(cx.value), size(cx.size), width(cx.width)
{ /* Do nothing, initializer lists only. */ }

CharX::CharX(uint64_t value, uint16_t size, uint16_t width) : value(value), size(size), width(width)
{ /* Do nothing, initializer lists only. */ }

CharX::CharX(const char c) : value(c), size(1), width(0)
{ /* Do nothing, initializer lists only. */ }

CharX::CharX(const char* ptr) : value(0), size(0), width(0)
{ CharX::construct_from_char_pointer(*this, ptr); }

CharX::CharX(const char* ptr, uint16_t& read_bytes) : value(0), size(0), width(0)
{ read_bytes = CharX::construct_from_char_pointer(*this, ptr) - ptr; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX& CharX::operator = (const CharX& cx) noexcept
{   // {{{

    this->value = cx.value;
    this->size  = cx.size;
    this->width = cx.width;

    return *this;

}   // }}}

StringX CharX::operator * (uint16_t n_repeat) const noexcept
{   // {{{

    StringX result;

    for (uint16_t n = 0; n < n_repeat; ++n)
        result += *this;

    return result;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX CharX::printable(void) const noexcept
{   // {{{

    if (this->value <= 0x1F)
        return StringX("^") + CharX(0x40 + this->value, 1, 1);

    if (this->value == 0x7F)
        return StringX("^?");
 
    return StringX("") + *this;

}   // }}}

String CharX::string(void) const noexcept
{   // {{{

    char buffer[32];
    int8_t index = 0;

    // Print Control Sequence Introducer (CSI) commands.
    if ((this->value & 0xFFFF) == 0x5B1B)
    {
        buffer[0] = 0x1B;
        buffer[1] = 0x5B;
        index = 2;

        for (uint16_t n = 2; n < (this->size - 1); ++n)
        {
            // Get the target byte.
            const int16_t x = (this->value >> (8 * n)) & 0xFF;

            // Convert the byte to char array.
            // NOTE: Use can use "std::to_string" or "itoa" functions instead,
            //       but these functions are fataly slow.
            if (x >= 100) { buffer[index++] = '0' + (x / 100) % 10; }
            if (x >=  10) { buffer[index++] = '0' + (x / 10)  % 10; }
            if (x >=   0) { buffer[index++] = '0' +  x        % 10; }

            // Append semicolon if not the last number.
            if (n != (this->size - 2))
                buffer[index++] = ';';
        }

        // Appand the last charactor of CSI command.
        if (this->size > 2)
            buffer[index++] = (this->value >> (8 * (this->size - 1))) & 0xFF;

        // Finalize the buffer.
        buffer[index] = '\0';
    }

    // Print other character.
    else
    {
        // Output the given character to the stream.
        for (uint16_t n = 0; n < this->size; ++n)
            buffer[index++] = (char) ((this->value >> (8 * n)) & 0xFF);

        // Finalize the buffer.
        buffer[index] = '\0';
    }

    return String(buffer);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void CharX::append_byte(uint8_t c) noexcept
{   // {{{

    this->value |= (((uint64_t) c) << (8 * this->size));
    this->size++;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t CharX::get_utf8_byte_size(char ch) noexcept
{   // {{{

    if      ((ch & 0x80) == 0x00) return (uint8_t) 1;
    else if ((ch & 0xE0) == 0xC0) return (uint8_t) 2;
    else if ((ch & 0xF0) == 0xE0) return (uint8_t) 3;
    else if ((ch & 0xF8) == 0xF0) return (uint8_t) 4;
    else                          return (uint8_t) 1;

}   // }}}

uint8_t CharX::get_utf8_width(uint64_t val) noexcept
{   // {{{

    if      (                      (val <= 0x0000FF)) return (uint8_t) 1;
    else if ((0xEFBDB1 <= val) and (val <= 0xEFBE9F)) return (uint8_t) 1;
    else                                              return (uint8_t) 2;

}   // }}}

const char* CharX::construct_from_char_pointer(CharX& cx, const char* str) noexcept
{   // {{{

    // Do nothing.
    if ((*str == '\0') or (*str == 127))
        return str;

    if (*str == '\x1B') { return CharX::construct_ansi_escseq(cx, str); }
    else                { return CharX::construct_normal_char(cx, str); }

}   // }}}

const char* CharX::construct_ansi_escseq(CharX& cx, const char* str) noexcept
{   // {{{

    constexpr auto process_csi = [](CharX& cx, const char*& str) -> const char*
    // [Abstract]
    //   Process Control Sequence Introducer (CSI) commands.
    //
    // [Args]
    //   self (CharX*)     : [OUT] Myself (target of the constructor).
    //   str  (const char*): [IN ] Input stream of the source.
    {
        // Temporal buffer and it's index.
        char buffer[8];
        uint8_t idx = 0;

        while (cx.size <= 8)
        {
            // Get 1 byte.
            const uint8_t c = *str++;

            if (c == ';')
            {
                // Append the integer written in the buffer.
                if (idx > 0)
                    cx.append_byte(atoi(buffer));

                // Reset the index of the buffer.
                idx = 0;
            }
            else if ((('a' <= c) and (c <= 'z')) or (('A' <= c) and (c <= 'Z')))
            {
                // Append the integer written in the buffer.
                if (idx > 0)
                    cx.append_byte(atoi(buffer));

                // Append the last character and exit.
                cx.append_byte(c);
                return str;
            }
            else
            {
                buffer[idx] = c;
                buffer[++idx] = '\0';
            }
        }

        return str;
    };

    // Initialize the value and size using the first byte of the input stream.
    cx.value = (uint64_t) *str++;
    cx.size  = 1;

    // If there is no next character, do nothing.
    if (*str == '\0')
        return str;

    // Get the next byte.
    cx.append_byte(*str++);

    // If the input value is (ESC + EOF) or (ESC + 255), replace it to ESC only.
    if      (cx.value == 0x1A1B) { cx = CharX(0x1B, 1, 0); return str; }
    else if (cx.value == 0xFF1B) { cx = CharX(0x1B, 1, 0); return str; }
    else if (cx.value == 0x5B1B) { return process_csi(cx, str);        }
    else                         { return str; /* Do nothing */        }

}   // }}}

const char* CharX::construct_normal_char(CharX& cx, const char* str) noexcept
{   // {{{

    // Compute the byte size of the character.
    cx.size = CharX::get_utf8_byte_size(*str);

    // Read values and update the member variable.
    for (uint16_t i = 0; (i < cx.size) and (*str != '\0'); ++i)
        cx.value |= (((uint64_t) *str++) << (8 * i));

    // Compute the width of the character.
    cx.width = CharX::get_utf8_width(cx.value);

    return str;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
