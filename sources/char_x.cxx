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

CharX::CharX(const uint64_t value, const uint16_t size, const uint16_t width) : value(value), size(size), width(width)
{ /* Do nothing, initializer lists only. */ }

CharX::CharX(const char* ptr) : value(0), size(0), width(0)
{ CharX::construct_from_char_pointer(this, ptr); }

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX&
CharX::operator = (const CharX& cx) noexcept
{   // {{{

    this->value = cx.value;
    this->size  = cx.size;
    this->width = cx.width;

    return *this;

}   // }}}

StringX
CharX::operator * (uint16_t n_repeat) const noexcept
{   // {{{

    StringX result;

    for (uint16_t n = 0; n < n_repeat; ++n)
        result += *this;

    return result;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::pair<int16_t, int16_t>
CharX::ansi_color(void) const noexcept
{   // {{{

    constexpr auto parse_to_int = [](uint64_t value, uint8_t index) noexcept -> std::pair<int8_t, int8_t>
    // [Abstract]
    //   Parse CharX value to a pair of color indices.
    //
    // [Args]
    //   value (uint64_t): Value of the target CharX instance.
    //   index (uint8_t) : Starting byte index.
    //
    // [Returns]
    //   (std::pair<int8_t, int8_t>): Parsed pair of integers.
    {
        // Initialize the target string to be parsed to int.
        std::string target;

        while (index < 8)
        {
            // Get target character.
            target += 0xFF & (value >> (8 * index++));

            // Break the while-loop if the character `c` is not a digit.
            if ((target.back() < '0') || ('9' < target.back())) break;
        }

        try         { return std::make_pair((uint8_t) std::stoi(target), index); }
        catch (...) { return std::make_pair((uint8_t) -1,                index); }
    };

    // Do nothing if the target character is not ANSI SGR sequence.
    const char byte_1st = 0xFF & (this->value);
    const char byte_2nd = 0xFF & (this->value >> 8);
    if ((byte_1st != '\x1b') || (byte_2nd != '['))
        return std::make_pair(-1, -1);

    // Parse foreground and background color code.
    const std::pair<int8_t, uint8_t> result_fg = parse_to_int(this->value, 2);
    const std::pair<int8_t, uint8_t> result_bg = parse_to_int(this->value, result_fg.second);

    return std::make_pair(result_fg.first, result_bg.first);

}   // }}}

std::string
CharX::string(void) const noexcept
{   // {{{

    constexpr auto append_to_string = [](std::string& s, uint8_t c) noexcept -> void
    // [Abstract]
    //
    // [Args]
    //   s (std::string&): Value of the target CharX instance.
    //   c (uint8_t)     : Starting byte index.
    //
    // [Notes]
    //   * 0x00 ~ 0x1F: Caret expression (^@, ^A, ..., ^Z, ^[, ..., ^_).
    //   * 0x20 ~ 0x7E: Simple ANSI code.
    //   * 0x7F       : Caret expression (^?).
    //   * Otherwise  : Unprintable character.
    {
        if      (c <= 0x1F) { s += std::string("^") + (char) (0x40 + c);      }
        else if (c <= 0x7e) { s += std::string() +  (char) c;                 }
        else if (c == 0x7F) { s += "^?";                                      }
        else                { s += "[UNPRINTABLE:" + std::to_string(c) + "]"; }
    };

    // 
    std::string s = "";

    for (uint64_t v = this->value; v != 0; v = v >> 8)
        append_to_string(s, v & 0xFF);

    return s;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t
CharX::get_utf8_byte_size(char ch) noexcept
{   // {{{

    if      ((ch & 0x80) == 0x00) return (uint8_t) 1;
    else if ((ch & 0xE0) == 0xC0) return (uint8_t) 2;
    else if ((ch & 0xF0) == 0xE0) return (uint8_t) 3;
    else if ((ch & 0xF8) == 0xF0) return (uint8_t) 4;
    else                          return (uint8_t) 1;

}   // }}}

uint8_t
CharX::get_utf8_width(uint64_t val) noexcept
{   // {{{

    if      (                      (val <= 0x0000FF)) return (uint8_t) 1;
    else if ((0xEFBDB1 <= val) and (val <= 0xEFBE9F)) return (uint8_t) 1;
    else                                              return (uint8_t) 2;

}   // }}}

void
CharX::construct_from_char_pointer(CharX* cx, const char* ptr) noexcept
{   // {{{

    // Convert char pointer to a string stream.
    std::stringstream ss(ptr);

    // Construct CharX instance.
    construct_from_string_stream(cx, ss);

}   // }}}

void
CharX::construct_from_string_stream(CharX* cx, std::istream& sin) noexcept
{   // {{{

    // Read the first byte.
    const uint8_t c_first = sin.get();

    if      (c_first == '\0'  ) /* Do nothing! */;
    else if (c_first == '\x1b') CharX::construct_ansi_escseq(cx, c_first, sin);
    else                        CharX::construct_normal_char(cx, c_first, sin);

}   // }}}

void
CharX::construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin) noexcept
{   // {{{

    // Initialize the value and size using the first byte of the input stream.
    self->value = (uint64_t) c_first;
    self->size  = 1;

    // Maximum length of ANSI escape sequence is 8.
    while (self->size <= 8)
    {
        // Get 1 byte.
        const uint8_t c = sin.get();

        // Exit if the end of the string.
        if ((c == '\0') or (c == 255) or (sin.eof() == true))
            return;

        // Write the byte to the value and increment the size.
        self->value |= (((uint64_t) c) << (8 * self->size));
        self->size++;

        // Exit if an alphabet found.
        if ((('a' <= c) and (c <= 'z')) or (('A' <= c) and (c <= 'Z')))
            return;
    }

}   // }}}

void
CharX::construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin) noexcept
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

std::ostream&
operator << (std::ostream& stream, const CharX& cx) noexcept
{   // {{{

    // Output the given character to the stream.
    for (uint16_t n = 0; n < cx.size; ++n)
        stream << (char) ((cx.value >> (8 * n)) & 0xFF);

    return stream;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
