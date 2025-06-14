////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: char_x.hxx                                                                  ///
///                                                                                              ///
/// Extended character class and it's iterator class.                                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHAR_X_HXX
#define CHAR_X_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"

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

        explicit CharX(uint64_t value = 0, uint16_t size = 0, uint16_t width = 0);
        // [Abstract]
        //   Constructor of CharX.
        //
        // [Args]
        //   value (uint64_t): [IN] Value of the UTF8 character.
        //   size  (uint16_t): [IN] Size of the UTF8 character.
        //   width (uint16_t): [IN] Width of the UTF8 character.

        explicit CharX(char c);
        // [Abstract]
        //   Constructor of CharX.
        //
        // [Args]
        //   c (char): [IN] Source character.

        explicit CharX(const char* ptr);
        // [Abstract]
        //   Constructor of CharX.
        //
        // [Args]
        //   ptr (const char*&): [IN] Source string to be parsed as a character.

        explicit CharX(const char* ptr, uint16_t& read_bytes);
        // [Abstract]
        //
        // [Args]

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        CharX& operator = (const CharX& cx) noexcept;
        // [Abstract]
        //   Assignment operator.
        //
        // [Args]
        //   cx (const CharX&): [IN] A character to be assigned.
        //
        // [Returns]
        //   (StringX): Repeated string.

        StringX operator * (uint16_t n_repeat) const noexcept;
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

        StringX printable(void) const noexcept;
        // [Abstract]
        //   Convert the character to printable string.
        //
        // [Returns]
        //   (StringX): Printable expression of CharX.

        String string(void) const noexcept;
        // [Abstract]
        //   Convert the character to string.
        //
        // [Returns]
        //   (String): String expression of CharX.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        uint64_t value;
        // Value of the UTF8 character.

        uint32_t size;
        // Size of the UTF8 character.

        uint32_t width;
        // Width of the UTF8 character.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void append_byte(uint8_t c) noexcept;
        // [Abstract]
        //   Appand one byte to "this->value".
        //
        // [Args]
        //   c (uint8_t): [IN] Input byte.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static uint8_t get_utf8_byte_size(char ch) noexcept;
        // [Abstract]
        //   Returns number of bytes of UTF8 character from the first byte of the character.
        //
        // [Args]
        //   ch (char): [IN] First byte of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Number of bytes of UTF8 character.

        static uint8_t get_utf8_width(uint64_t val) noexcept;
        // [Abstract]
        //   Returns width of the given UTF8 character.
        //
        // [Args]
        //   ch (uint64_t): [IN] Value of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Width of UTF8 character.

        static const char* construct_from_char_pointer(CharX& cx, const char* str) noexcept;
        // [Abstract]
        //   Constructor from char pointer.
        //
        // [Args]
        //   cx  (CharX&)      : [OUT] Myself (target of the constructor).
        //   str (const char*&): [IN ] Input stream of the source.

        static const char* construct_ansi_escseq(CharX& cx, const char* str) noexcept;
        // [Abstract]
        //   Constructor from input stream for ANSI escape sequences.
        //
        // [Args]
        //   cx  (CharX&)      : [OUT] Myself (target of the constructor).
        //   str (const char*&): [IN ] Input stream of the source.

        static const char* construct_normal_char(CharX& cx, const char* str) noexcept;
        // [Abstract]
        //   Constructor from input stream for normal characters.
        //
        // [Args]
        //   cx  (CharX&)      : [OUT] Myself (target of the constructor).
        //   str (const char*&): [IN ] Input stream of the source.
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
