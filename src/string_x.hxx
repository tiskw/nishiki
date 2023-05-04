////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ header file: string_x.hxx
//
// Extended string class and it's iterator class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STRING_X_HXX
#define STRING_X_HXX

#include <deque>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototype declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

class CharX;
class StringX;

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: A class for UTF8 character
////////////////////////////////////////////////////////////////////////////////////////////////////
class CharX
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        explicit CharX(uint64_t value = 0, uint16_t size = 0, uint16_t width = 0);
        explicit CharX(const char* ptr, bool raw = false);
        explicit CharX(std::istream& sin, bool raw = false);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        bool
        operator<(const CharX& cx)
        const noexcept;

        StringX
        operator*(uint16_t n_repeat)
        const noexcept;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Returns SGR (Select Graphic Rendition) parameters of ANSI escape sequence.
        std::pair<int16_t, int16_t>
        ansi_color(void)
        const noexcept;

        // Convert to printable std::string.
        std::string
        printable(void)
        const noexcept;

        // Convert to std::string.
        std::string
        string(void)
        const noexcept;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        uint64_t value;  // Value of the UTF8 character.
        uint16_t size;   // Size of the UTF8 character.
        uint16_t width;  // Width of the UTF8 character.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Returns number of bytes of UTF8 character from the first byte of the character.
        static uint8_t
        get_utf8_byte_size(char ch)
        noexcept;

        // Returns width of the given UTF8 character.
        static uint8_t
        get_utf8_width(uint64_t val)
        noexcept;

        // Constructor from char pointer.
        static void
        construct_from_char_pointer(CharX* cx, const char* sin, bool raw = false)
        noexcept;

        // Constructor from input stream.
        static void
        construct_from_string_stream(CharX* cx, std::istream& sin, bool raw = false)
        noexcept;

        // Constructor from input stream for ANSI escape sequences.
        static void
        construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin)
        noexcept;

        // Constructor from input stream for normal characters.
        static void
        construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin)
        noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: A class for UTF8 string
////////////////////////////////////////////////////////////////////////////////////////////////////
class StringX : public std::deque<CharX>
{
    public:

        ////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////

        enum class Pos { BEGIN, END };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX(void);
        StringX(const StringX& str);
        explicit StringX(const char* ptr);
        explicit StringX(const std::string& str);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Addition operator.
        StringX
        operator+(const CharX& cx)
        const noexcept;

        // Addition operator.
        StringX
        operator+(const StringX& str)
        const noexcept;

        // Assignment operator.
        StringX&
        operator=(const StringX& str)
        noexcept;

        // Addition assignment operator.
        StringX&
        operator+=(const CharX& cx)
        noexcept;

        // Addition assignment operator.
        StringX&
        operator+=(const StringX& str)
        noexcept;

        // Equality operator.
        bool
        operator==(const StringX& str)
        const noexcept;

        // Inequality operator.
        bool
        operator!=(const StringX& str)
        const noexcept;

        // Less-than operator.
        bool
        operator<(const StringX& str)
        const noexcept;

        // Greater-than operator.
        bool
        operator>(const StringX& str)
        const noexcept;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Append string at the end.
        void
        append(const StringX& str)
        noexcept;

        // Returns clipped string with the given length.
        StringX
        clip(uint16_t length)
        const noexcept;

        // Returns colorized string.
        StringX
        colorize(void)
        const noexcept;

        // Returns if myself is ended with the given string.
        bool
        endswith(char c)
        const noexcept;

        // Join strings.
        StringX
        join(const std::vector<StringX>& strs, bool delim_end = false)
        const noexcept;

        // Pop out character.
        CharX
        pop(const StringX::Pos)
        noexcept;

        // Returns if myself is started from the given string.
        bool
        startswith(const StringX& str)
        const noexcept;

        // Strip white-spaces.
        StringX
        strip(void)
        const noexcept;

        // Convert myself to std::string.
        std::string
        string(void)
        const noexcept;

        // Returns sub-string.
        StringX
        substr(uint32_t pos, uint32_t n = UINT32_MAX)
        const noexcept;

        // Splits the string to tokens and returns them.
        std::vector<StringX>
        tokenize(void)
        const noexcept;

        // Remove quote if quoted.
        StringX
        unquote(void)
        const noexcept;

        // Returns width of the string.
        uint16_t
        width(void)
        const noexcept;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Construct StringX from char pointer.
        static void
        construct_from_char_pointer(StringX* sx, const char* ptr)
        noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Write the given character to the stream.
std::ostream&
operator<<(std::ostream& stream, const CharX& cx)
noexcept;

// Write the given string to the stream.
std::ostream&
operator<<(std::ostream& stream, const StringX& str)
noexcept;

// Consistent comparison operator (it likes <=> operator in C++20).
int8_t
consistent_comparison(const StringX& s1, const StringX& s2)
noexcept;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
