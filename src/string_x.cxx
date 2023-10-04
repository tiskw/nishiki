////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: string_x.hxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "string_x.hxx"

#include <iostream>
#include <sstream>
#include <set>

#include "utils.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// File local macros
////////////////////////////////////////////////////////////////////////////////////////////////////

// Chack the given element is contained in the given set.
#define contains(set, elem) (set.find(elem) != set.end())


////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX::CharX(uint64_t value, uint16_t size, uint16_t width) : value(value), size(size), width(width)
{ /* Do nothing */ }

CharX::CharX(const char* ptr, bool raw) : value(0), size(0), width(0)
{ CharX::construct_from_char_pointer(this, ptr, raw); }

CharX::CharX(std::istream& sin, bool raw) : value(0), size(0), width(0)
{ CharX::construct_from_string_stream(this, sin, raw); }


////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
CharX::operator<(const CharX& cx)
const noexcept
{   // {{{

    return (this->value < cx.value);

}   // }}}

StringX
CharX::operator*(uint16_t n_repeat)
const noexcept
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
CharX::ansi_color(void)
const noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Parse CharX value to a pair of color indices.
    //
    // Args:
    //   value (uint64_t): Value of the target CharX instance.
    //   index (uint8_t) : Starting byte index.
    //
    // Returns:
    //   (std::pair<int8_t, int8_t>): Parsed pair of integers.
    //
    // Notes:
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto parse_to_int = [](uint64_t value, uint8_t index) noexcept
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
    std::pair<int8_t, uint8_t> result_fg = parse_to_int(this->value, 2);
    std::pair<int8_t, uint8_t> result_bg = parse_to_int(this->value, result_fg.second);

    return std::make_pair(result_fg.first, result_bg.first);

}   // }}}

std::string
CharX::printable(void)
const noexcept
{   // {{{

    if (this->value <= 0x1F) return std::string("^") + (char) (this->value + '@');
    else                     return this->string();

}   // }}}

std::string
CharX::string(void)
const noexcept
{   // {{{

    std::string result;

    for (uint16_t n = 0; n < this->size; ++n)
        result += (char) ((this->value >> (8 * n)) & 0xFF);

    return result;

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t
CharX::get_utf8_byte_size(char ch)
noexcept
{   // {{{

    if      ((ch & 0x80) == 0x00) return (uint8_t) 1;
    else if ((ch & 0xE0) == 0xC0) return (uint8_t) 2;
    else if ((ch & 0xF0) == 0xE0) return (uint8_t) 3;
    else if ((ch & 0xF8) == 0xF0) return (uint8_t) 4;
    else                          return (uint8_t) 1;

}   // }}}

uint8_t
CharX::get_utf8_width(uint64_t val)
noexcept
{   // {{{

    if      (                      (val <= 0x0000FF)) return (uint8_t) 1;
    else if ((0xEFBDB1 <= val) and (val <= 0xEFBE9F)) return (uint8_t) 1;
    else                                              return (uint8_t) 2;

}   // }}}

void
CharX::construct_from_char_pointer(CharX* cx, const char* ptr, bool raw)
noexcept
{   // {{{

    // Convert char pointer to a string stream.
    std::stringstream ss(ptr);

    construct_from_string_stream(cx, ss, raw);

}   // }}}

void
CharX::construct_from_string_stream(CharX* cx, std::istream& sin, bool raw)
noexcept
{   // {{{

    // Read the first byte.
    uint8_t c_first = sin.get();

    if      (c_first == '\0'  ) /* Do nothing! */;
    else if (raw              ) CharX::construct_normal_char(cx, c_first, sin);
    else if (c_first == '\x1b') CharX::construct_ansi_escseq(cx, c_first, sin);
    else                        CharX::construct_normal_char(cx, c_first, sin);

}   // }}}

void
CharX::construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin)
noexcept
{   // {{{

    // Initialize the value using the first byte of the input stream.
    self->value = (uint64_t) c_first;

    // Maximum length of ANSI escape sequence is 8.
    for (self->size = 2; self->size <= 8; ++self->size)
    {
        // Get 1 byte.
        uint8_t c = sin.get();

        // Write the byte to the value.
        self->value |= (((uint64_t) (uint8_t) c) << (8 * (self->size - 1)));

        // Exit if an alphabet found.
        if (('a' <= c) and (c <= 'z'))
            return;
    }

}   // }}}

void
CharX::construct_normal_char(CharX* self, uint8_t c_first, std::istream& sin)
noexcept
{   // {{{

    // Ciompute the byte size of the character.
    self->size = CharX::get_utf8_byte_size(c_first);

    // Initialize the value using the first byte of the input stream.
    self->value = (uint64_t) c_first;

    // Read values and update the member variable.
    for (uint16_t i = 1; i < self->size; ++i)
        self->value |= (((uint64_t) (uint8_t) sin.get()) << (8 * i));

    // Compute the width of the character.
    self->width = CharX::get_utf8_width(self->value);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX::StringX(void) : std::deque<CharX>()
{ /* Do nothing */ }

StringX::StringX(const StringX& str) : std::deque<CharX>()
{ std::copy(str.cbegin(), str.cend(), std::back_inserter(*this)); }

StringX::StringX(const char* ptr) : std::deque<CharX>()
{ StringX::construct_from_char_pointer(this, ptr); }

StringX::StringX(const std::string& str) : StringX(str.c_str())
{ /* Do nothing */ }


////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
StringX::operator+(const CharX& cx)
const noexcept
{   // {{{

    StringX src = StringX(*this);

    src.push_back(cx);

    return src;

}   // }}}

StringX
StringX::operator+(const StringX& sx)
const noexcept
{   // {{{

    // Create a copy of string.
    StringX src = StringX(*this);

    // Append the given string.
    std::copy(sx.cbegin(), sx.cend(), std::back_inserter(src));

    return src;

}   // }}}

StringX&
StringX::operator=(const StringX& sx)
noexcept
{   // {{{

    // Clear current contents.
    this->clear();

    // Copy the given string.
    std::copy(sx.cbegin(), sx.cend(), std::back_inserter(*this));

    // Returns myself for convenience.
    return *this;

}   // }}}

StringX&
StringX::operator+=(const CharX& cx)
noexcept
{   // {{{

    // Append the given character.
    this->push_back(cx);

    // Returns myself for convenience.
    return *this;

}   // }}}

StringX&
StringX::operator+=(const StringX& str)
noexcept
{   // {{{

    // Append the given string at the end.
    std::copy(str.cbegin(), str.cend(), std::back_inserter(*this));

    // Returns myself for convenience.
    return *this;

}   // }}}

bool
StringX::operator==(const StringX& str)
const noexcept
{   // {{{

    return (consistent_comparison(*this, str) == 0);

}   // }}}

bool
StringX::operator!=(const StringX& str)
const noexcept
{   // {{{

    return (consistent_comparison(*this, str) != 0);

}   // }}}

bool
StringX::operator<(const StringX& str)
const noexcept
{   // {{{

    return (consistent_comparison(*this, str) < 0);

}   // }}}

bool
StringX::operator>(const StringX& str)
const noexcept
{   // {{{

    return (consistent_comparison(*this, str) > 0);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringX::append(const StringX& str)
noexcept
{   // {{{

    *this += str;

}   // }}}

StringX
StringX::clip(uint16_t length)
const noexcept
{   // {{{

    StringX result;

    uint16_t total = 0;

    for (const CharX& cx : *this)
    {
        // Update total width.
        total += cx.width;

        // Returns the result string before adding the current character
        // if the total width exceeds the given limit langth.
        if (total > length) return result;

        // Update byte size.
        result.push_back(cx);
    }

    return result;

}   // }}}

StringX
StringX::colorize(void)
const noexcept
{   // {{{

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // File local constants
    ////////////////////////////////////////////////////////////////////////////////////////////////

    static const std::set<StringX> set_command({
        StringX("cat") , StringX("cd")    , StringX("chmod"), StringX("chown"), StringX("cp") , StringX("echo"),
        StringX("env") , StringX("export"), StringX("grep") , StringX("let")  , StringX("ln") , StringX("ls")  ,
        StringX("make"), StringX("mkdir") , StringX("mv")   , StringX("rm")   , StringX("sed"), StringX("set") ,
        StringX("tar") , StringX("touch") , StringX("umask"), StringX("unset"),
    });

    static const std::set<StringX> set_keyword({
        StringX("case") , StringX("do")   , StringX("done")  , StringX("elif")    , StringX("else") , StringX("esac"),
        StringX("exit") , StringX("fi")   , StringX("for")   , StringX("function"), StringX("if")   , StringX("in")  ,
        StringX("local"), StringX("read") , StringX("return"), StringX("select")  , StringX("shift"), StringX("then"),
        StringX("time") , StringX("until"), StringX("while") ,
    });

    static const std::set<StringX> set_symbols({
        StringX("&"), StringX("|"), StringX(">"), StringX("<"), StringX("&&"), StringX("||"), StringX(">>"), StringX("<<"),
    });

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Define a function that returns true if the given token is a string token
    // which is enclosed by single/double quotes.
    //
    // [Args]
    //   token (const StringX&): [IN] Token.
    //
    // [Returns]
    //   (bool): True if the given token is a string token.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto is_string_token = [](const StringX& token)
    {
        if      (token.size()   == 0   ) return false;
        else if (token[0].value == '\"') return true;
        else if (token[0].value == '\'') return true;
        else                             return false;
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Define a function to colorize a given token.
    //
    // [Args]
    //   token (const StringX&): [IN] Token string.
    //
    // [Returns]
    //   (StringX): Colorized token.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto colorize_token = [=](const StringX& token)
    {
        if      (contains(set_command, token)) return StringX("\033[32m") + token + StringX("\033[m");  // Command color.
        else if (contains(set_keyword, token)) return StringX("\033[33m") + token + StringX("\033[m");  // Keyword color.
        else if (contains(set_symbols, token)) return StringX("\033[34m") + token + StringX("\033[m");  // Symbols color.
        else if (is_string_token(token))       return StringX("\033[31m") + token + StringX("\033[m");  // Strings color.
        else                                   return                       token                    ;  // Others.
    };

    // Initialize returned value.
    StringX colorized;

    // Split the string to tokens, colorize each tokens, and join.
    for (const StringX& token : this->tokenize())
        colorized += colorize_token(token);

    return colorized;

}   // }}}

bool
StringX::endswith(const char c)
const noexcept
{   // {{{

    return ((this->size() > 0) and (this->back().value == (uint64_t) c));

}   // }}}

StringX
StringX::join(const std::vector<StringX>& strs, const bool delim_end)
const noexcept
{   // {{{

    StringX result;

    for (uint32_t i = 0; i < strs.size(); ++i)
        result += (delim_end or (i != (strs.size() - 1))) ? (strs[i] + *this) : strs[i];

    return result;

}   // }}}

CharX
StringX::pop(const StringX::Pos pos)
noexcept
{   // {{{

    CharX cx;

    if      (this->size() == 0         ) { cx = CharX(0, 0, 0);                    }
    else if (pos == StringX::Pos::BEGIN) { cx = this->front() ; this->pop_front(); }
    else if (pos == StringX::Pos::END  ) { cx = this->back()  ; this->pop_back() ; }
    else                                 { cx = CharX(0, 0, 0);                    }

    return cx;

}   // }}}

bool
StringX::startswith(const StringX& str)
const noexcept
{   // {{{

    // The pattern string never match with the target string
    // if the length of pattern string is longer than the target string.
    if (this->size() < str.size()) return false;

    for (size_t i = 0; i < str.size(); ++i)
        if (str[i].value != (*this)[i].value)
            return false;

    return true;

}   // }}}

StringX
StringX::strip(bool left, bool right)
const noexcept
{   // {{{

    // Initialize the returned value as a copy of myself.
    StringX result = StringX(*this);

    // Strip white-spaces from front.
    while (left and result.size() > 0 and ((result.front().value == 0x09) or (result.front().value == 0x20)))
        result.pop_front();

    // Strip white-spaces from back.
    while (right and result.size() > 0 and ((result.back().value == 0x09) or (result.back().value == 0x20)))
        result.pop_back();

    return result;

}   // }}}

std::string
StringX::string(void)
const noexcept
{   // {{{

    std::string result;

    for (CharX cx : *this)
        for (uint16_t n = 0; n < cx.size; ++n)
            result += (char) ((cx.value >> (8 * n)) & 0xFF);

    return result;

}   // }}}

StringX
StringX::substr(uint32_t pos, uint32_t n)
const noexcept
{   // {{{

    // Starting position should be smaller than the string length.
    pos = MIN(pos, this->size());

    // Size of sub-string should be smaller than the size of remaining rest.
    // Note that the default value of UINT32_MAX, so please take care for overflow.
    uint32_t dist = pos + MIN(n, this->size() - pos);

    // Initialize returned value.
    StringX result;

    // Append all characters in the sub-string range.
    for (auto iter = this->begin() + pos; std::distance(this->begin(), iter) < dist; ++iter)
        result += *iter;

    return result;

}   // }}}

std::vector<StringX>
StringX::tokenize(void)
const noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Get string token that is quoted by single/double quote.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
    //   quote    (const uint64_t)       : [IN ] Quote type.
    //
    // [Returns]
    //   void
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto push_string_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end, const uint64_t quote)
    {
        token.push_back(*(iter++));

        while ((iter != iter_end) and (iter->value != quote))
            token.push_back(*(iter++));

        if (iter != iter_end)
            token.push_back(*(iter++));
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Get white-space token.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
    //
    // [Returns]
    //   void
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto push_whitespace_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end)
    {
        while ((iter != iter_end) and ((iter->value == ' ') or (iter->value == '\t')))
            token.push_back(*(iter++));
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Get normal token.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
    //
    // [Returns]
    //   void
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto push_normal_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end)
    {
        while ((iter != iter_end) and (iter->value != ' ') and (iter->value != '\t'))
            token.push_back(*(iter++));
    };

    // Initialize returned value.
    std::vector<StringX> result;

    // Initialize iterators.
    StringX::const_iterator iter     = this->cbegin();
    StringX::const_iterator iter_bgn = this->cbegin();
    StringX::const_iterator iter_end = this->cend();

    while (iter != iter_end)
    {
        StringX token;

        // Automatically add zero-width characters (e.g. ANSI escape sequence).
        while ((iter != iter_end) and (iter->width == 0))
            token.push_back(*(iter++));

        // Read a token.
        switch (iter->value)
        {
            case '\'': push_string_token(token, iter, iter_end, '\''); break;  // String token.
            case '\"': push_string_token(token, iter, iter_end, '\"'); break;  // String token.
            case ' ' : push_whitespace_token(token, iter, iter_end);   break;  // Whitespace token.
            default  : push_normal_token(token, iter, iter_end);       break;  // Others.
        }

        // Automatically push-back zero-width characters (e.g. ANSI escape sequence).
        while ((token.width() > 0) and (iter != iter_bgn) and (iter != iter_end) and (token.back().width == 0))
        {
            --iter;
            token.pop(StringX::Pos::END);
        }

        // Push the token to the returned vector.
        if (token.size() > 0)
            result.push_back(token);
    }

    return result;

}   // }}}

StringX
StringX::unquote(void)
const noexcept
{   // {{{

    // Do nothing if empty string.
    if (this->size() < 2) return StringX(*this);

    // Unquote if quoted.
    if (((this->front().value == '\'') and (this->back().value == '\''))
     or ((this->front().value == '\"') and (this->back().value == '\"')))
        return this->substr(1, this->size() - 2);

    return StringX(*this);

}   // }}}

uint16_t
StringX::width(void)
const noexcept
{   // {{{

    // Accumurate width of each character.
    uint16_t result = 0;
    for (CharX cx : *this)
        result += cx.width;

    return result;

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringX::construct_from_char_pointer(StringX* sx, const char* ptr)
noexcept
{   // {{{

    // Read one CharX from the pointer, and incremant the pointer with read bytes.
    for (const char* p = ptr; *p != '\0'; p += sx->back().size)
        sx->emplace_back(p);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream& stream, const CharX& cx)
noexcept
{   // {{{

    // Output the given character to the stream.
    for (uint16_t n = 0; n < cx.size; ++n)
        stream << (char) ((cx.value >> (8 * n)) & 0xFF);

    return stream;

}   // }}}

std::ostream&
operator<<(std::ostream& stream, const StringX& str)
noexcept
{   // {{{

    // Output the given string to the stream.
    for (CharX cx : str)
        stream << cx;

    return stream;

}   // }}}

int8_t
consistent_comparison(const StringX& s1, const StringX& s2)
noexcept
{   // {{{

    int8_t res  = -128;
    size_t idx1 = 0;
    size_t idx2 = 0;

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Get normal token.
    //
    // [Args]
    //   idx1 (const size_t&) : [IN] Index of the `s1`.
    //   s1   (const StringX&): [IN] Instance of `s1`.
    //   idx2 (const size_t&) : [IN] Index of the `s2`.
    //   s2   (const StringX&): [IN] Instance of `s2`.
    //
    // [Returns]
    //   (int8_t): Returns 0 if s1 == s2, +1 if s1 < s2, -1 if s1 > s2,
    //             and -128 if continue.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto check_terminate_condition = [](size_t idx1, const StringX& s1,
                                                  size_t idx2, const StringX& s2)
    {
        // End condition 1: both s1 and s2 finished at the same time.
        if ((idx1 == s1.size()) and (idx2 == s2.size()))
            return (int8_t) 0;

        // End condition 2: s1 finished earlier.
        if (idx1 == s1.size())
            return (int8_t) -1;

        // End condition 3: s2 finished earlier.
        if (idx2 == s2.size())
            return (int8_t) +1;

        // End condition 4: faced to inequal character and s1 < s2.
        if (s1[idx1].value < s2[idx2].value)
            return (int8_t) -1;

        // End condition 4: faced to inequal character and s2 > s1.
        if (s1[idx1].value > s2[idx2].value)
            return (int8_t) +1;

        // Otherwise, continue the loop.
        return (int8_t) -128;
    };

    while (true)
    {
        // Skip zero-width characters.
        while (idx1 < s1.size() and (s1[idx1].width == 0))
            ++idx1;
 
        // Skip zero-width characters.
        while (idx2 < s2.size() and (s2[idx2].width == 0))
            ++idx2;

        res = check_terminate_condition(idx1, s1, idx2, s2);

        if (res != -128) break;

        // Continue condition: still the same characters continuing.
        ++idx1; ++idx2;
    }

    return res;

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
