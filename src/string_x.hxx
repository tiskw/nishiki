////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: string_x.hxx                                                                ///
///                                                                                              ///
/// Extended string class and it's iterator class.                                               ///
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
// Class definition
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
        // [Abstract]
        //   Comparison operator.
        //
        // [Args]
        //   cx (const CharX&): [IN] comparison target (right hand side).
        //
        // [Returns]
        //   (bool): True if the variable `cx` is greater than myself.

        StringX
        operator*(uint16_t n_repeat)
        const noexcept;
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
        ansi_color(void)
        const noexcept;
        // [Abstract]
        //   Returns SGR (Select Graphic Rendition) parameters of ANSI escape sequence.
        //   Note that this function assumed that the current character is ANSI escape sequence.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::pair<int16_t, int16_t>): A pair of foreground and background color index.

        std::string
        printable(void)
        const noexcept;
        // [Abstract]
        //   Convert to printable std::string.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::string): String expression of CharX.

        std::string
        string(void)
        const noexcept;
        // [Abstract]
        //   Convert to std::string.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::string): String expression of CharX.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        uint64_t value;
        // [Abstract]
        //   Value of the UTF8 character.

        uint16_t size;
        // [Abstract]
        //   Size of the UTF8 character.

        uint16_t width;
        // [Abstract]
        //   Width of the UTF8 character.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static uint8_t
        get_utf8_byte_size(char ch)
        noexcept;
        // [Abstract]
        //   Returns number of bytes of UTF8 character from the first byte of the character.
        //
        // [Args]
        //   ch (char): [IN] First byte of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Number of bytes of UTF8 character.

        static uint8_t
        get_utf8_width(uint64_t val)
        noexcept;
        // [Abstract]
        //   Returns width of the given UTF8 character.
        //
        // [Args]
        //   ch (uint64_t): [IN] Value of UTF8 character.
        //
        // [Returns]
        //   (uint8_t): Width of UTF8 character.

        static void
        construct_from_char_pointer(CharX* cx, const char* sin, bool raw = false)
        noexcept;
        // [Abstract]
        //   Constructor from char pointer.
        //
        // [Args]
        //   self (CharX*)     : [OUT] Myself (target of the constructor).
        //   ptr  (const char*): [IN ] Input stream of the source.
        //   raw  (bool)       : [IN ] Do not parse ANSI escape sequence if true.
        //
        // [Returns]
        //   void
        //

        static void
        construct_from_string_stream(CharX* cx, std::istream& sin, bool raw = false)
        noexcept;
        // [Abstract]
        //   Constructor from input stream.
        //
        // [Args]
        //   self (CharX*)       : [OUT] Myself (target of the constructor).
        //   sin  (std::istream&): [IN ] Input stream of the source.
        //   raw  (bool)         : [IN ] Do not parse ANSI escape sequence if true.
        //
        // [Returns]
        //   void

        // Constructor from input stream for ANSI escape sequences.
        static void
        construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin)
        noexcept;
        // [Abstract]
        //   Constructor from input stream for ANSI escape sequences.
        //
        // [Args]
        //   self    (CharX*)     : [OUT] Myself (target of the constructor).
        //   c_first (uint8_t)    : [IN ] First character of the input stream.
        //   sin     (const char*): [IN ] Input stream of the source (`c_first` is not contained).
        //
        // [Returns]
        //   void

        static void
        construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin)
        noexcept;
        // [Abstract]
        //   Constructor from input stream for normal characters.
        //
        // [Args]
        //   self    (CharX*)     : [OUT] Myself (target of the constructor).
        //   c_first (uint8_t)    : [IN ] First character of the input stream.
        //   sin     (const char*): [IN ] Input stream of the source (`c_first` is not contained).
        //
        // [Returns]
        //   void
};

class StringX : public std::deque<CharX>
{
    public:

        ////////////////////////////////////////////////////////////////////////
        // Data types
        ////////////////////////////////////////////////////////////////////////

        enum class Pos { BEGIN, END };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX(void);
        StringX(const StringX& str);
        explicit StringX(const char* ptr);
        explicit StringX(const std::string& str);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX
        operator+(const CharX& cx)
        const noexcept;
        // [Abstract]
        //   Addition operator with CharX.
        //
        // [Args]
        //   cx (CharX): [IN] A character to be added.
        //
        // [Returns]
        //   (StringX): Added string.

        StringX
        operator+(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Addition operator with StringX.
        //
        // [Args]
        //   sx (StringX): [IN] A string to be added.
        //
        // [Returns]
        //   (StringX): Added string.

        StringX&
        operator=(const StringX& str)
        noexcept;
        // [Abstract]
        //   Assignment operation.
        //
        // [Args]
        //   sx (const StringX&): [IN] A string to be assigned.
        //
        // [Returns]
        //   (StringX): Myself.

        StringX&
        operator+=(const CharX& cx)
        noexcept;
        // [Abstract]
        //   Addition assignment operator with CharX.
        //
        // [Args]
        //   cx (const CharX&): [IN] A character to be added.
        //
        // [Returns]
        //   (StringX&): Myself.

        StringX&
        operator+=(const StringX& str)
        noexcept;
        // [Abstract]
        //   Addition assignment operator with StringX.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be added.
        //
        // [Returns]
        //   (StringX&): Myself.

        bool
        operator==(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Equality operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` is equal to `str`.

        bool
        operator!=(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Inequality operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` is not equal to `str`.

        bool
        operator<(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Less-than operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` is less than `str`.

        bool
        operator>(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Greater-than operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` is greater than `str`.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        void
        append(const StringX& str)
        noexcept;
        // [Abstract]
        //   Append string at the end.
        //
        // [Args]
        //   str (const StringX& str): [IN] String to be appended.
        //
        // [Returns]
        //   void

        StringX
        clip(uint16_t length)
        const noexcept;
        // [Abstract]
        //   Clip the string to the specified width.
        //
        // [Args]
        //   length (uint16_t): [IN] The string will be clipped to this width.
        //
        // [Returns]
        //   (StringX): Clipped string.

        StringX
        colorize(void)
        const noexcept;
        // [Abstract]
        //   Colorize the string in Shell syntax.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (StringX): Colorized string.

        bool
        endswith(char c)
        const noexcept;
        // [Abstract]
        //   Returns if myself is ended with the given character.
        //
        // [Args]
        //   c (const char): [IN] A character to be compared.
        //
        // [Returns]
        //   (bool): True if myself is ended with `c`.

        StringX
        join(const std::vector<StringX>& strs, bool delim_end = false)
        const noexcept;
        // [Abstract]
        //   Join the given strings where the delimiter of the joining is myself.
        //
        // [Args]
        //   strs (const std::vector<StringX>&): [IN] List of strings to be joined.
        //
        // [Returns]
        //   (StringX): Joined string.

        CharX
        pop(const StringX::Pos)
        noexcept;
        // [Abstract]
        //   Pop a character from the string and returns the popped character.
        //
        // [Args]
        //   pos (const StringX::Pos): [IN] Position to be popped.
        //
        // [Returns]
        //   (CharX): The popped character.

        bool
        startswith(const StringX& str)
        const noexcept;
        // [Abstract]
        //   Returns true if myself is started from the given string.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` starts with `str`.

        StringX
        strip(void)
        const noexcept;
        // [Abstract]
        //   Strip white-spaces.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (StringX): Stripped string.

        std::string
        string(void)
        const noexcept;
        // [Abstract]
        //   Convert StringX to std::string.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::string): Converted string.

        StringX
        substr(uint32_t pos, uint32_t n = UINT32_MAX)
        const noexcept;
        // [Abstract]
        //   Returns sub-string.
        //
        // [Args]
        //   pos (uint32_t): Position of the first character to include.
        //   n   (uint32_t): Length of the sub-string
        //
        // [Returns]
        //   (StringX): Sub-string.

        std::vector<StringX>
        tokenize(void)
        const noexcept;
        // [Abstract]
        //   Split myself to tokens. For example, a string
        //
        //       echo 'this is a pen' | grep 'pen'
        //
        //   will be tokenized to
        //
        //       echo / 'this is a pen' / | / grep / 'pen'
        //
        //   where / is the seperator of the tokens.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (std::vector<StringX>): Array of tokens.
        //
        // [Examples]
        //   | // Define target string.
        //   | StringX str = StringX("echo 'this is a pen' | grep 'pen'")
        //   |
        //   | // Tokenize.
        //   | std::vector<StringX> tokens = str.tokenize()
        //   |
        //   | // Check tokenization result.
        //   | assert(tokens[0] == StringX("echo"));
        //   | assert(tokens[1] == StringX(" "));
        //   | assert(tokens[2] == StringX("'this is a pen'"));
        //   | assert(tokens[3] == StringX(" "));
        //   | assert(tokens[4] == StringX("|"));
        //   | assert(tokens[5] == StringX(" "));
        //   | assert(tokens[6] == StringX("grep"));
        //   | assert(tokens[7] == StringX(" "));
        //   | assert(tokens[8] == StringX("'pen'"));

        StringX
        unquote(void)
        const noexcept;
        // [Abstract]
        //   Remove quote if quoted.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (StringX): A string where quote is stripped.

        uint16_t
        width(void)
        const noexcept;
        // [Abstract]
        //   Returns width of the string.
        //
        // [Args]
        //   void
        //
        // [Returns]
        //   (uint16_t): Total width of the string.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        // Construct StringX from char pointer.
        static void
        construct_from_char_pointer(StringX* sx, const char* ptr)
        noexcept;
        // [Abstract]
        //   Construct StringX from char pointer.
        //
        // [Args]
        //   sx  (StringX*)   : Construction target.
        //   ptr (const char*): Source of a string.
        //
        // [Returns]
        //   void
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

// Write the given character to the stream.
std::ostream&
operator<<(std::ostream& stream, const CharX& cx)
noexcept;
// [Abstract]
//   Write the given character to the output stream.
//
// [Args]
//   stream (std::ostream&): [IN] The output stream to which the characters will be written.
//   cx     (const CharX&) : [IN] The character to be written.
//
// [Returns]
//   (std::ostream&): The output stream.

std::ostream&
operator<<(std::ostream& stream, const StringX& str)
noexcept;
// [Abstract]
//   Write the given string to the output stream.
//
// [Args]
//   stream (std::ostream&) : [IN] The output stream to which the characters will be written.
//   cx     (const StringX&): [IN] The character to be written.
//
// [Returns]
//   (std::ostream&): The output stream.

int8_t
consistent_comparison(const StringX& s1, const StringX& s2)
noexcept;
// [Abstract]
//   Consistent comparison operator (it likes <=> operator in C++20).
//
// [Args]
//   s1 (const StringX&): [IN] Comparison target 1.
//   s2 (const StringX&): [IN] Comparison target 2.
//
// [Returns]
//   (int8_t): Returns 0 if s1 == s2, +1 if s1 < s2, and -1 if s1 > s2.


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
