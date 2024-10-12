////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: string_x.cxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "string_x.hxx"

// Include the headers of STL.
#include <numeric>
#include <sstream>
#include <set>

// Include the headers of custom modules.
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CharX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

CharX::CharX(const CharX& cx) : value(cx.value), size(cx.size), width(cx.width)
{ /* Do nothing */ }

CharX::CharX(uint64_t value, uint16_t size, uint16_t width) : value(value), size(size), width(width)
{ /* Do nothing */ }

CharX::CharX(const char* ptr, bool raw) : value(0), size(0), width(0)
{ CharX::construct_from_char_pointer(this, ptr, raw); }

CharX::CharX(std::istream& sin, bool raw) : value(0), size(0), width(0)
{ CharX::construct_from_string_stream(this, sin, raw); }

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
CharX::printable(void) const noexcept
{   // {{{

    if (this->value <= 0x1F) return std::string("^") + (char) (this->value + '@');
    else                     return this->string();

}   // }}}

std::string
CharX::string(void) const noexcept
{   // {{{

    // 0x00 ~ 0x1F: caret expression (^@, ^A, ..., ^Z, ^[, ..., ^_).
    if (this->value <= 0x1F)
        return std::string("^") + ((char) (0x40 + this->value));

    // 0x20 ~ 0x7E: simple ANSI code.
    else if (this->value <= 0x7e)
        return std::string() +  ((char) this->value);

    // 0x7F: caret expression (^?).
    else if (this->value == 0x7F)
        return std::string("^?");

    // 0x80 ~ : number expression.
    else
        return std::string("^") + ((char) this->value);

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
CharX::construct_from_char_pointer(CharX* cx, const char* ptr, bool raw) noexcept
{   // {{{

    // Convert char pointer to a string stream.
    std::stringstream ss(ptr);

    construct_from_string_stream(cx, ss, raw);

}   // }}}

void
CharX::construct_from_string_stream(CharX* cx, std::istream& sin, bool raw) noexcept
{   // {{{

    // Read the first byte.
    const uint8_t c_first = sin.get();

    if      (c_first == '\0'  ) /* Do nothing! */;
    else if (raw              ) CharX::construct_normal_char(cx, c_first, sin);
    else if (c_first == '\x1b') CharX::construct_ansi_escseq(cx, c_first, sin);
    else                        CharX::construct_normal_char(cx, c_first, sin);

}   // }}}

void
CharX::construct_ansi_escseq(CharX* self, uint8_t c_first, std::istream& sin) noexcept
{   // {{{

    // Initialize the value using the first byte of the input stream.
    self->value = (uint64_t) c_first;

    // Maximum length of ANSI escape sequence is 8.
    for (self->size = 2; self->size <= 8; ++self->size)
    {
        // Get 1 byte.
        const uint8_t c = sin.get();

        // Write the byte to the value.
        self->value |= (((uint64_t) c) << (8 * (self->size - 1)));

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
        self->value |= (((uint64_t) (uint8_t) sin.get()) << (8 * i));

    // Compute the width of the character.
    self->width = CharX::get_utf8_width(self->value);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX::StringX(void) : std::deque<CharX>()
{ /* Do nothing */ }

StringX::StringX(const StringX& str) : std::deque<CharX>(str)
{ /* Do nothing */ }

StringX::StringX(const char* ptr) : std::deque<CharX>()
{ StringX::construct_from_char_pointer(this, ptr); }

StringX::StringX(const std::string& str) : std::deque<CharX>()
{ StringX::construct_from_char_pointer(this, str.c_str()); }

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
StringX::operator + (const CharX& cx) const noexcept
{   // {{{

    // Create a copy of string.
    StringX src = StringX(*this);

    // Append the given character.
    src.push_back(cx);

    return src;

}   // }}}

StringX
StringX::operator + (const StringX& sx) const noexcept
{   // {{{

    // Create a copy of string.
    StringX src = StringX(*this);

    // Append the given string.
    std::copy(sx.cbegin(), sx.cend(), std::back_inserter(src));

    return src;

}   // }}}

StringX&
StringX::operator = (const StringX& sx) noexcept
{   // {{{

    // Do nothing if the source and target is the same.
    // This avoidance is quite important because the following code
    // does not work if "this" and "sx" is the same instance.
    if (*this == sx) return *this;

    // Clear current contents.
    this->clear();

    // Copy the given string.
    std::copy(sx.cbegin(), sx.cend(), std::back_inserter(*this));

    // Returns myself for convenience.
    return *this;

}   // }}}

StringX&
StringX::operator += (const CharX& cx) noexcept
{   // {{{

    // Append the given character.
    this->push_back(cx);

    // Returns myself for convenience.
    return *this;

}   // }}}

StringX&
StringX::operator += (const StringX& str) noexcept
{   // {{{

    // Append the given string at the end.
    std::copy(str.cbegin(), str.cend(), std::back_inserter(*this));

    // Returns myself for convenience.
    return *this;

}   // }}}

std::strong_ordering
StringX::operator <=> (const StringX& str) const noexcept
{   // {{{

    // Rename strings to be compared.
    const StringX& s1 = *this;
    const StringX& s2 = str;

    // Initialize local variables.
    size_t idx1 = 0;
    size_t idx2 = 0;

    while (true)
    {
        // Skip zero-width characters.
        while ((idx1 < s1.size()) and (s1[idx1].width == 0))
            ++idx1;

        // Skip zero-width characters.
        while ((idx2 < s2.size()) and (s2[idx2].width == 0))
            ++idx2;

        // Compute end flag of each string.
        const bool is_end1 = (idx1 >= s1.size());
        const bool is_end2 = (idx2 >= s2.size());

        // End condition.
        if      (is_end1 and is_end2            ) return std::strong_ordering::equivalent; // Both s1 and s2 finished at the same time.
        else if (is_end1                        ) return std::strong_ordering::less;       // s1 finished earlier.
        else if (is_end2                        ) return std::strong_ordering::greater;    // s2 finished earlier.
        else if (s1[idx1].value < s2[idx2].value) return std::strong_ordering::less;       // Faced to inequal character and s1 < s2.
        else if (s1[idx1].value > s2[idx2].value) return std::strong_ordering::greater;    // Faced to inequal character and s2 > s1.

        // Continue condition: still the same characters continuing.
        ++idx1; ++idx2;
    }

}   // }}}

// Comparison operators derived from the spaceship operator.
bool StringX::operator <  (const StringX& str) const noexcept { return (*this <=> str) <  0; }
bool StringX::operator >  (const StringX& str) const noexcept { return (*this <=> str) >  0; }
bool StringX::operator == (const StringX& str) const noexcept { return (*this <=> str) == 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
StringX::clip(uint16_t length) const noexcept
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
StringX::colorize(void) const noexcept
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

    constexpr auto is_string_token = [](const StringX& token) noexcept -> bool
    // [Abstract]
    //   Define a function that returns true if the given token is a string token
    //   which is enclosed by single/double quotes.
    //
    // [Args]
    //   token (const StringX&): [IN] Token.
    //
    // [Returns]
    //   (bool): True if the given token is a string token.
    {
        if      (token.size()   == 0   ) return false;
        else if (token[0].value == '\"') return true;
        else if (token[0].value == '\'') return true;
        else                             return false;
    };

    constexpr auto colorize_token = [=](const StringX& token) noexcept -> StringX
    // [Abstract]
    //   Define a function to colorize a given token.
    //
    // [Args]
    //   token (const StringX&): [IN] Token string.
    //
    // [Returns]
    //   (StringX): Colorized token.
    {
        if      (set_command.contains(token)) return StringX("\033[32m") + token + StringX("\033[m");  // Command color.
        else if (set_keyword.contains(token)) return StringX("\033[33m") + token + StringX("\033[m");  // Keyword color.
        else if (set_symbols.contains(token)) return StringX("\033[34m") + token + StringX("\033[m");  // Symbols color.
        else if (is_string_token(token))      return StringX("\033[31m") + token + StringX("\033[m");  // Strings color.
        else                                  return                       token                    ;  // Others.
    };

    // Split the string to tokens, colorize each token, and join them.
    return StringX("").join(transform<StringX, StringX>(this->tokenize(), colorize_token));

}   // }}}

bool
StringX::endswith(const char c) const noexcept
{   // {{{

    return ((this->size() > 0) and (this->back().value == (uint64_t) c));

}   // }}}

StringX
StringX::join(const std::vector<StringX>& strs, const bool delim_end) const noexcept
{   // {{{

    StringX result;

    for (uint32_t i = 0; i < strs.size(); ++i)
        result += (delim_end or (i != (strs.size() - 1))) ? (strs[i] + *this) : strs[i];

    return result;

}   // }}}

CharX
StringX::pop(const StringX::Pos pos) noexcept
{   // {{{

    CharX cx;

    if      (this->size() == 0         ) { cx = CharX(0, 0, 0);                    }
    else if (pos == StringX::Pos::BEGIN) { cx = this->front() ; this->pop_front(); }
    else if (pos == StringX::Pos::END  ) { cx = this->back()  ; this->pop_back() ; }
    else                                 { cx = CharX(0, 0, 0);                    }

    return cx;

}   // }}}

bool
StringX::startswith(const StringX& str) const noexcept
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
StringX::strip(bool left, bool right) const noexcept
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
StringX::string(void) const noexcept
{   // {{{

    std::string result;

    for (CharX cx : *this)
        for (uint16_t n = 0; n < cx.size; ++n)
            result += (char) ((cx.value >> (8 * n)) & 0xFF);

    return result;

}   // }}}

StringX
StringX::substr(uint32_t pos, uint32_t n) const noexcept
{   // {{{

    // Starting position should be smaller than the string length.
    pos = MIN(pos, this->size());

    // Size of sub-string should be smaller than the size of remaining rest.
    // Note that the default value of UINT32_MAX, so please take care for overflow.
    const uint32_t dist = pos + MIN(n, this->size() - pos);

    // Initialize returned value.
    StringX result;

    // Append all characters in the sub-string range.
    for (auto iter = this->begin() + pos; std::distance(this->begin(), iter) < dist; ++iter)
        result += *iter;

    return result;

}   // }}}

std::vector<StringX>
StringX::tokenize(void) const noexcept
{   // {{{

    constexpr auto push_string_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end, const uint64_t quote) noexcept -> void
    // [Abstract]
    //   Get string token that is quoted by single/double quote.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
    //   quote    (const uint64_t)       : [IN ] Quote type.
    {
        token.push_back(*(iter++));

        while ((iter != iter_end) and (iter->value != quote))
            token.push_back(*(iter++));

        if (iter != iter_end)
            token.push_back(*(iter++));
    };

    constexpr auto push_whitespace_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end) noexcept -> void
    // [Abstract]
    //   Get white-space token.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
    {
        while ((iter != iter_end) and ((iter->value == ' ') or (iter->value == '\t')))
            token.push_back(*(iter++));
    };

    constexpr auto push_normal_token = [](StringX& token, const_iterator& iter, const const_iterator& iter_end) noexcept -> void
    // [Abstract]
    //   Get normal token.
    //
    // [Args]
    //   token    (StringX&)             : [OUT] The token will be added to this string instance.
    //   iter     (const_iterator&)      : [IN ] Start iterator.
    //   iter_end (const const_iterator&): [IN ] End iterator.
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
StringX::unquote(void) const noexcept
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
StringX::width(void) const noexcept
{   // {{{

    // Define a function to add the width of the given character.
    const auto add_width = [](const uint16_t acc, const CharX& cx) { return acc + cx.width; };

    // Accumurate width of each character.
    return std::accumulate(this->cbegin(), this->cend(), (uint16_t) 0, add_width);
                           

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringX::construct_from_char_pointer(StringX* sx, const char* ptr) noexcept
{   // {{{

    // Read one CharX from the pointer, and incremant the pointer with read bytes.
    for (const char* p = ptr; *p != '\0'; p += sx->back().size)
        sx->emplace_back(p);

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

std::ostream&
operator << (std::ostream& stream, const StringX& str) noexcept
{   // {{{

    // Output the given string to the stream.
    for (const CharX& cx : str)
        stream << cx;

    return stream;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
