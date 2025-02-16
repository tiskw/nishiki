////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: char_x.hxx                                                                  ///
///                                                                                              ///
/// Extended character class and it's iterator class.                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHAR_X_HXX
#define CHAR_X_HXX

// Include the headers of STL.
#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Global constants
////////////////////////////////////////////////////////////////////////////////////////////////////

#define CHARX_VALUE_KEY_UP    (0x415b1b)  // ^[[A => [0x1b,0x5b,0x41] => 0x415b1b
#define CHARX_VALUE_KEY_DOWN  (0x425b1b)  // ^[[B => [0x1b,0x5b,0x42] => 0x425b1b
#define CHARX_VALUE_KEY_RIGHT (0x435b1b)  // ^[[C => [0x1b,0x5b,0x43] => 0x435b1b
#define CHARX_VALUE_KEY_LEFT  (0x445b1b)  // ^[[D => [0x1b,0x5b,0x44] => 0x445b1b

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class prototype definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class StringX;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class CharX
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        CharX(const CharX& cx);
        // [Abstract]
        //   Default constructor of CharX.
        //
        // [Args]
        //   cx (const CharX&): [IN] Source character to be copied.

        explicit CharX(const uint64_t value = 0, const uint16_t size = 0, const uint16_t width = 0);
        // [Abstract]
        //   Constructor of CharX.
        //
        // [Args]
        //   value (uint64_t): [IN] Value of the UTF8 character.
        //   size  (uint16_t): [IN] Size of the UTF8 character.
        //   width (uint16_t): [IN] Width of the UTF8 character.

        explicit CharX(const char* ptr);
        // [Abstract]
        //   Constructor of CharX.
        //
        // [Args]
        //   ptr (const char*): [IN] 

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        CharX&
        operator = (const CharX& cx) noexcept;
        // [Abstract]
        //   Assignment operator.
        //
        // [Args]
        //   cx (const CharX&): [IN] A character to be assigned.
        //
        // [Returns]
        //   (StringX): Repeated string.

        StringX
        operator * (uint16_t n_repeat) const noexcept;
        // [Abstract]
        //   Multiple operator.
        //
        // [Args]
        //   n_repeat (uint16_t): [IN] Number of repeats.
        //
        // [Returns]
        //   (StringX): Repeated string.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<int16_t, int16_t>
        ansi_color(void) const noexcept;
        // [Abstract]
        //   Returns SGR (Select Graphic Rendition) parameters of ANSI escape sequence.
        //   Note that this function assumed that the current character is ANSI escape sequence.
        //
        // [Returns]
        //   (std::pair<int16_t, int16_t>): A pair of foreground and background color index.

        std::string
        string(void) const noexcept;
        // [Abstract]
        //   Convert the character to printable std::string.
        //
        // [Returns]
        //   (std::string): String expression of CharX.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Public member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        uint64_t value;
        // Value of the UTF8 character.

        uint16_t size;
        // Size of the UTF8 character.

        uint16_t width;
        // Width of the UTF8 character.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static uint8_t
        get_utf8_byte_size(char ch) noexcept;
        // [Abstract]
        //   Returns number of bytes of UTF8 character from the first byte of the character.
        //
        // [Args]
        //   ch (char): [IN] First byte of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Number of bytes of UTF8 character.

        static uint8_t
        get_utf8_width(uint64_t val) noexcept;
        // [Abstract]
        //   Returns width of the given UTF8 character.
        //
        // [Args]
        //   ch (uint64_t): [IN] Value of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Width of UTF8 character.

        static void
        construct_from_char_pointer(CharX* cx, const char* sin) noexcept;
        // [Abstract]
        //   Constructor from char pointer.
        //
        // [Args]
        //   self (CharX*)     : [OUT] Myself (target of the constructor).
        //   ptr  (const char*): [IN ] Input stream of the source.

        static void
        construct_from_string_stream(CharX* cx, std::istream& sin) noexcept;
        // [Abstract]
        //   Constructor from input stream.
        //
        // [Args]
        //   self (CharX*)       : [OUT] Myself (target of the constructor).
        //   sin  (std::istream&): [IN ] Input stream of the source.

        static void
        construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin) noexcept;
        // [Abstract]
        //   Constructor from input stream for ANSI escape sequences.
        //
        // [Args]
        //   self    (CharX*)     : [OUT] Myself (target of the constructor).
        //   c_first (uint8_t)    : [IN ] First character of the input stream.
        //   sin     (const char*): [IN ] Input stream of the source (`c_first` is not contained).

        static void
        construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin) noexcept;
        // [Abstract]
        //   Constructor from input stream for normal characters.
        //
        // [Args]
        //   self    (CharX*)     : [OUT] Myself (target of the constructor).
        //   c_first (uint8_t)    : [IN ] First character of the input stream.
        //   sin     (const char*): [IN ] Input stream of the source (`c_first` is not contained).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Write the given character to the stream.
std::ostream&
operator << (std::ostream& stream, const CharX& cx) noexcept;
// [Abstract]
//   Write the given character to the output stream.
//
// [Args]
//   stream (std::ostream&): [IN] The output stream to which the characters will be written.
//   cx     (const CharX&) : [IN] The character to be written.
//
// [Returns]
//   (std::ostream&): The output stream.

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
