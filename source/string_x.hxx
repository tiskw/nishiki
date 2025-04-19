////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: string_x.hxx                                                                ///
///                                                                                              ///
/// Extended string class and it's iterator class.                                               ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STRING_X_HXX
#define STRING_X_HXX

// Include the headers of STL.
#include <cstdint>
#include <string>
#include <vector>

// Include the headers of custom modules.
#include "char_x.hxx"
#include "dtypes.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class StringX : public Deque<CharX>
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
        // [Abstract]
        //   Default constructor of StringX.

        StringX(const StringX& sx);
        // [Abstract]
        //   Default constructor of StringX.
        //
        // [Args]
        //   sx (const StringX&): [IN] Source string to be copied.
 
        explicit StringX(const char* ptr);
        // [Abstract]
        //   Constructor of StringX.
        //
        // [Args]
        //   ptr (const char*): String to be copied.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Operators
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX operator + (const CharX& cx) const noexcept;
        // [Abstract]
        //   Addition operator with CharX.
        //
        // [Args]
        //   cx (CharX): [IN] A character to be added.
        //
        // [Returns]
        //   (StringX): Added string.

        StringX operator + (const StringX& str) const noexcept;
        // [Abstract]
        //   Addition operator with StringX.
        //
        // [Args]
        //   sx (StringX): [IN] A string to be added.
        //
        // [Returns]
        //   (StringX): Added string.

        StringX& operator = (const StringX& str) noexcept;
        // [Abstract]
        //   Assignment operation.
        //
        // [Args]
        //   sx (const StringX&): [IN] A string to be assigned.
        //
        // [Returns]
        //   (StringX): Myself.

        StringX& operator += (const CharX& cx) noexcept;
        // [Abstract]
        //   Addition assignment operator with CharX.
        //
        // [Args]
        //   cx (const CharX&): [IN] A character to be added.
        //
        // [Returns]
        //   (StringX&): Myself.

        StringX& operator += (const StringX& str) noexcept;
        // [Abstract]
        //   Addition assignment operator with StringX.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be added.
        //
        // [Returns]
        //   (StringX&): Myself.

        bool operator < (const StringX& str) const noexcept;
        // [Abstract]
        //   "less than" operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string.
        //
        // [Returns]
        //   (bool): True if (*this < str) holds.

        bool operator == (const StringX& str) const noexcept;
        // [Abstract]
        //   Equal operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string.
        //
        // [Returns]
        //   (bool): True if (*this == str) holds.

        std::strong_ordering operator <=> (const StringX& str) const noexcept;
        // [Abstract]
        //   Three way comparison operator.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (std::strong_order): Three way comparison value.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX clip(uint16_t length) const noexcept;
        // [Abstract]
        //   Clip the string to the specified width.
        //
        // [Args]
        //   length (uint16_t): [IN] The string will be clipped to this width.
        //
        // [Returns]
        //   (StringX): Clipped string.

        Vector<StringX> chunk(uint16_t chunk_size) const noexcept;
        // [Abstract]
        //   Split the string into fixed width chunks.
        //
        // [Args]
        //   chunk_size (uint16_t): [IN] Maximum width of each chunk.
        //
        // [Returns]
        //   (Vector<StringX>): Chunked strings.

        StringX colorize(void) const noexcept;
        // [Abstract]
        //   Colorize the string in Shell syntax.
        //
        // [Returns]
        //   (StringX): Colorized string.

        bool endswith(char c) const noexcept;
        // [Abstract]
        //   Returns if myself is ended with the given character.
        //
        // [Args]
        //   c (const char): [IN] A character to be compared.
        //
        // [Returns]
        //   (bool): True if myself is ended with `c`.

        StringX join(const Vector<StringX>& strs, bool delim_end = false) const noexcept;
        // [Abstract]
        //   Join the given strings where the delimiter of the joining is myself.
        //
        // [Args]
        //   strs (const std::vector<StringX>&): [IN] List of strings to be joined.
        //
        // [Returns]
        //   (StringX): Joined string.

        CharX pop(const StringX::Pos) noexcept;
        // [Abstract]
        //   Pop a character from the string and returns the popped character.
        //
        // [Args]
        //   pos (const StringX::Pos): [IN] Position to be popped.
        //
        // [Returns]
        //   (CharX): The popped character.

        bool startswith(const StringX& str) const noexcept;
        // [Abstract]
        //   Returns true if myself is started from the given string.
        //
        // [Args]
        //   str (const StringX&): [IN] A string to be compared.
        //
        // [Returns]
        //   (bool): True if `*this` starts with `str`.

        StringX strip(bool left = true, bool right = true) const noexcept;
        // [Abstract]
        //   Strip white-spaces.
        //
        // [Args]
        //   left  (bool): [IN] Strip from left if true.
        //   right (bool): [IN] Strip from right if true.
        //
        // [Returns]
        //   (StringX): Stripped string.

        String string(void) const noexcept;
        // [Abstract]
        //   Convert StringX to std::string.
        //
        // [Returns]
        //   (String): Converted string.

        StringX substr(uint32_t pos, uint32_t n = UINT32_MAX) const noexcept;
        // [Abstract]
        //   Returns sub-string.
        //
        // [Args]
        //   pos (uint32_t): [IN] Position of the first character to include.
        //   n   (uint32_t): [IN] Length of the sub-string
        //
        // [Returns]
        //   (StringX): Sub-string.

        Vector<StringX> tokenize(void) const noexcept;
        // [Abstract]
        //   Split myself to tokens. For example, a string
        //
        //     echo 'this is a pen' | grep 'pen'
        //
        //   will be tokenized to
        //
        //     echo / 'this is a pen' / | / grep / 'pen'
        //
        //   where / is the seperator of the tokens.
        //
        // [Returns]
        //   (Vector<StringX>): Array of tokens.

        StringX unquote(void) const noexcept;
        // [Abstract]
        //   Remove quote if quoted.
        //
        // [Returns]
        //   (StringX): A string where quote is stripped.

        uint16_t width(void) const noexcept;
        // [Abstract]
        //   Returns width of the string.
        //
        // [Returns]
        //   (uint16_t): Total width of the string.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        static const char* colorize_commands;
        static const char* colorize_keywords;
        static const char* colorize_symbols;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Static functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        static void construct_from_char_pointer(StringX* sx, const char* ptr) noexcept;
        // [Abstract]
        //   Construct StringX from char pointer.
        //
        // [Args]
        //   sx  (StringX*)   : [OUT] Construction target.
        //   ptr (const char*): [IN ] Source of a string.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

template<> struct std::hash<StringX>
{
    std::size_t operator()(const StringX& sx) const noexcept
    // [Abstract]
    //   Custom specialization of std::hash can be injected in namespace std.
    //
    // [Args]
    //   sx (const StringX&): The target string.
    //
    // [Returns]
    //   (std::size_t): Hash value of the given string.
    //
    // [Notes]
    //   This is FNV-1a algorithm, a simple non-cryptographic hash function.
    //   FNV-1a is fast and simple to implement, but has a higher collision rate than sha1/md5.
    //
    {   // {{{

        // Define the prime value.
        constexpr uint64_t prime = 0x100000001b3;

        // Initialize the hash value.
        uint64_t hash_value = 0xcbf29ce484222325;

        // Update the hash value.
        for (const CharX& cx : sx)
            hash_value = prime * (hash_value ^ cx.value);

        return static_cast<std::size_t>(hash_value);
    }   // }}}
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
