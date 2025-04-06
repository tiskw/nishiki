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

CharX::CharX(const char* ptr) : value(0), size(0), width(0)
{ CharX::construct_from_char_pointer(this, ptr); }

CharX::CharX(std::istream& sin) : value(0), size(0), width(0)
{ CharX::construct_from_string_stream(this, sin); }

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

    String result;

    for (uint16_t n = 0; n < this->size; ++n)
        result += (char) ((this->value >> (8 * n)) & 0xFF);

    return result;

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

void CharX::construct_from_char_pointer(CharX* cx, const char* ptr) noexcept
{   // {{{

    // Convert char pointer to a string stream.
    std::stringstream ss(ptr);

    // Construct CharX instance.
    construct_from_string_stream(cx, ss);

}   // }}}

void CharX::construct_from_string_stream(CharX* cx, std::istream& sin) noexcept
{   // {{{

    // Read the first byte.
    const uint8_t c_first = sin.get();

    // Do nothing.
    if ((c_first == '\0') or (c_first == 255))
        return;

    if (c_first == '\x1B') { CharX::construct_ansi_escseq(cx, c_first, sin); }
    else                   { CharX::construct_normal_char(cx, c_first, sin); }

}   // }}}

void CharX::construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin) noexcept
{   // {{{

    constexpr auto process_csi = [](CharX* self, std::istream& sin) -> void
    // [Abstract]
    //   Process Control Sequence Introducer (CSI) commands.
    //
    // [Args]
    //   self (CharX*)       : [OUT] Myself (target of the constructor).
    //   sin  (std::istream&): [IN ] Input stream of the source.
    {
        // Temporal buffer and it's index.
        char buffer[8];
        uint8_t idx = 0;

        while (self->size <= 8)
        {
            // Get 1 byte.
            const uint8_t c = sin.get();

            if (c == ';')
            {
                // Append the integer written in the buffer.
                if (idx > 0)
                    self->append_byte(atoi(buffer));

                // Reset the index of the buffer.
                idx = 0;
            }
            else if ((('a' <= c) and (c <= 'z')) or (('A' <= c) and (c <= 'Z')))
            {
                // Append the integer written in the buffer.
                if (idx > 0)
                    self->append_byte(atoi(buffer));

                // Append the last character and exit.
                self->append_byte(c);
                return;
            }
            else
            {
                buffer[idx] = c;
                buffer[++idx] = '\0';
            }
        }
    };

    // Initialize the value and size using the first byte of the input stream.
    self->value = (uint64_t) c_first;
    self->size  = 1;

    // If there is no next character, do nothing.
    if (sin.eof() == true)
        return;

    // Get the next byte.
    self->append_byte(sin.get());

    // If the input value is (ESC + EOF) or (ESC + 255), replace it to ESC only.
    if      (self->value == 0x1A1B) { *self = CharX(0x1B, 1, 0); }
    else if (self->value == 0x5B1B) { process_csi(self, sin);    }
    else if (self->value == 0xFF1B) { *self = CharX(0x1B, 1, 0); }
    else                            { /* Do nothing */           }

}   // }}}

void CharX::construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin) noexcept
{   // {{{

    // Compute the byte size of the character.
    self->size = CharX::get_utf8_byte_size(c_first);

    // Initialize the value using the first byte of the input stream.
    self->value = (uint64_t) c_first;

    // Read values and update the member variable.
    for (uint16_t i = 1; i < self->size; ++i)
        self->value |= (((uint64_t) sin.get()) << (8 * i));

    // Compute the width of the character.
    self->width = CharX::get_utf8_width(self->value);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& stream, const CharX& cx) noexcept
{   // {{{

    // Print Control Sequence Introducer (CSI) commands.
    if ((cx.value & 0xFFFF) == 0x5B1B)
    {
        char buffer[32] = {0x1B, 0x5B, 0x00};
        int8_t index = 2;

        for (uint16_t n = 2; n < (cx.size - 1); ++n)
        {
            // Get the target byte.
            const int16_t x = (cx.value >> (8 * n)) & 0xFF;

            // Convert the byte to char array.
            // NOTE: Use can use "std::to_string" or "itoa" functions instead,
            //       but these functions are fataly slow.
            if (x >= 100) { buffer[index++] = '0' + (x / 100) % 10; }
            if (x >=  10) { buffer[index++] = '0' + (x / 10)  % 10; }
            if (x >=   0) { buffer[index++] = '0' +  x        % 10; }

            // Append semicolon if not the last number.
            if (n != (cx.size - 2))
                buffer[index++] = ';';
        }

        // Appand the last charactor of CSI command.
        if (cx.size > 2)
            buffer[index++] = (cx.value >> (8 * (cx.size - 1))) & 0xFF;

        // Finalize the buffer.
        buffer[index] = '\0';

        // Flush the buffer to the output stream.
        stream << buffer;

        return stream;
    }

    // Print other character.
    else
    {
        // Output the given character to the stream.
        for (uint16_t n = 0; n < cx.size; ++n)
            stream << (char) ((cx.value >> (8 * n)) & 0xFF);
    }

    return stream;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
