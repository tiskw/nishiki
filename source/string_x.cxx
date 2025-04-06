////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: string_x.cxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "string_x.hxx"

// Include the headers of STL.
#include <numeric>
#include <sstream>

// Include the headers of custom modules.
#include "config.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX::StringX(void) : Deque<CharX>()
{ /* Do nothing */ }

StringX::StringX(const StringX& sx) : Deque<CharX>(sx)
{ /* Do nothing */ }

StringX::StringX(const char* ptr) : Deque<CharX>()
{ StringX::construct_from_char_pointer(this, ptr); }

StringX::StringX(const String& str) : Deque<CharX>()
{ StringX::construct_from_char_pointer(this, str.c_str()); }

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX StringX::operator + (const CharX& cx) const noexcept
{   // {{{

    // Create a copy of string.
    StringX src = StringX(*this);

    // Append the given character.
    src.emplace_back(cx);

    return src;

}   // }}}

StringX StringX::operator + (const StringX& sx) const noexcept
{   // {{{

    // Create a copy of string.
    StringX src = StringX(*this);

    // Append the given string.
    std::copy(sx.cbegin(), sx.cend(), std::back_inserter(src));

    return src;

}   // }}}

StringX& StringX::operator = (const StringX& sx) noexcept
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

StringX& StringX::operator += (const CharX& cx) noexcept
{   // {{{

    // Append the given character.
    this->push_back(cx);

    // Returns myself for convenience.
    return *this;

}   // }}}

StringX& StringX::operator += (const StringX& str) noexcept
{   // {{{

    // Append the given string at the end.
    std::copy(str.cbegin(), str.cend(), std::back_inserter(*this));

    // Returns myself for convenience.
    return *this;

}   // }}}

std::strong_ordering StringX::operator <=> (const StringX& str) const noexcept
{   // {{{

    // Rename strings to be compared.
    const StringX& s1 = *this;
    const StringX& s2 = str;

    // Initialize local variables.
    uint32_t idx1 = 0;
    uint32_t idx2 = 0;

    while (true)
    {
        // Skip zero-width characters.
        while ((idx1 < s1.size()) and (s1[idx1].width == 0)) ++idx1;
        while ((idx2 < s2.size()) and (s2[idx2].width == 0)) ++idx2;

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
bool StringX::operator == (const StringX& str) const noexcept { return (*this <=> str) == 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX StringX::clip(uint16_t length) const noexcept
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

Vector<StringX> StringX::chunk(uint16_t chunk_size) const noexcept
{   // {{{

    // Initialize the output chunks.
    std::vector<StringX> chunks;

    // Initialize the starting position of the current chunk.
    uint32_t pos = 0;

    while (pos < this->size())
    {
        // Add the chunk to the output vector.
        chunks.emplace_back(this->substr(pos, this->size()).clip(chunk_size));

        // Compute the starting position of the next chunk.
        pos += chunks.back().size();
    }

    return chunks;

}   // }}}

StringX StringX::colorize(void) const noexcept
{   // {{{

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

    constexpr auto create_stringx_set = [](Set<StringX>& set, const String& targets) noexcept -> void
    // [Abstract]
    //   Create a set of tokens.
    //
    // [Args]
    //   set     (Set<StringX>&): [IN] Target set to create.
    //   targets (const String&): [IN] Comma-seperated keyword strings.
    {
        for (const String& s : split(targets, ","))
            set.emplace(StringX(s));
    };

    static bool is_initialized = false;
    static Set<StringX> set_commands;
    static Set<StringX> set_keywords;
    static Set<StringX> set_symbols;

    if (not is_initialized)
    {
        create_stringx_set(set_commands, config.colorize_commands);
        create_stringx_set(set_keywords, config.colorize_keywords);
        create_stringx_set(set_symbols,  config.colorize_symbols);
        is_initialized = true;
    }

    // Split the string to tokens, colorize each token, and join them.
    StringX result;
    for (const StringX& token : this->tokenize())
    {
        if      (set_commands.contains(token)) { result += StringX("\033[32m") + token + StringX("\033[m"); }  // Command color.
        else if (set_keywords.contains(token)) { result += StringX("\033[33m") + token + StringX("\033[m"); }  // Keyword color.
        else if (set_symbols.contains(token))  { result += StringX("\033[34m") + token + StringX("\033[m"); }  // Symbols color.
        else if (is_string_token(token))       { result += StringX("\033[31m") + token + StringX("\033[m"); }  // Strings color.
        else                                   { result +=                       token                    ; }  // Others.
    }

    return result;

}   // }}}

bool StringX::endswith(const char c) const noexcept
{   // {{{

    return ((this->size() > 0) and (this->back().value == (uint64_t) c));

}   // }}}

StringX StringX::join(const Vector<StringX>& strs, const bool delim_end) const noexcept
{   // {{{

    StringX result;

    for (uint32_t i = 0; i < strs.size(); ++i)
        result += (delim_end or (i != (strs.size() - 1))) ? (strs[i] + *this) : strs[i];

    return result;

}   // }}}

CharX StringX::pop(const StringX::Pos pos) noexcept
{   // {{{

    CharX cx;

    if      (this->size() == 0         ) { cx = CharX(0, 0, 0);                    }
    else if (pos == StringX::Pos::BEGIN) { cx = this->front() ; this->pop_front(); }
    else if (pos == StringX::Pos::END  ) { cx = this->back()  ; this->pop_back() ; }
    else                                 { cx = CharX(0, 0, 0);                    }

    return cx;

}   // }}}

bool StringX::startswith(const StringX& str) const noexcept
{   // {{{

    // The pattern string never match with the target string
    // if the length of pattern string is longer than the target string.
    if (this->size() < str.size())
        return false;

    for (size_t i = 0; i < str.size(); ++i)
        if (str[i].value != this->at(i).value)
            return false;

    return true;

}   // }}}

StringX StringX::strip(bool left, bool right) const noexcept
{   // {{{

    // Initialize the returned value as a copy of myself.
    StringX result = StringX(*this);

    // Strip white-spaces from front.
    while (left and (result.size() > 0) and ((result.front().value == 0x09) or (result.front().value == 0x20)))
        result.pop_front();

    // Strip white-spaces from back.
    while (right and (result.size() > 0) and ((result.back().value == 0x09) or (result.back().value == 0x20)))
        result.pop_back();

    return result;

}   // }}}

String StringX::string(void) const noexcept
{   // {{{

    String result;

    for (CharX cx : *this)
        result += cx.string();

    return result;

}   // }}}

StringX StringX::substr(uint32_t pos, uint32_t n) const noexcept
{   // {{{

    // Get the string length.
    const uint32_t length = static_cast<uint32_t>(this->size());

    // Starting position should be smaller than the string length.
    pos = std::min(pos, length);

    // Size of sub-string should be smaller than the size of remaining rest.
    // Note that the default value of UINT32_MAX, so please take care for overflow.
    const uint32_t dist = pos + std::min(n, length - pos);

    // Initialize returned value.
    StringX result;

    // Append all characters in the sub-string range.
    for (auto iter = this->begin() + pos; std::distance(this->begin(), iter) < dist; ++iter)
        result += *iter;

    return result;

}   // }}}

Vector<StringX> StringX::tokenize(void) const noexcept
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
    Vector<StringX> result;

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

StringX StringX::unquote(void) const noexcept
{   // {{{

    // Do nothing if empty string.
    if (this->size() < 2) return StringX(*this);

    // Unquote if quoted.
    if (((this->front().value == '\'') and (this->back().value == '\''))
     or ((this->front().value == '\"') and (this->back().value == '\"')))
        return this->substr(1, this->size() - 2);

    return StringX(*this);

}   // }}}

uint16_t StringX::width(void) const noexcept
{   // {{{

    // Define a function to add the width of the given character.
    const auto add_width = [](const uint16_t acc, const CharX& cx) { return acc + cx.width; };

    // Accumurate width of each character.
    return std::accumulate(this->cbegin(), this->cend(), (uint16_t) 0, add_width);
                           

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringX: Class static member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void StringX::construct_from_char_pointer(StringX* sx, const char* ptr) noexcept
{   // {{{

    // Convert char pointer to a string stream.
    std::stringstream ss(ptr);

    // Read one CharX from the pointer, and incremant the pointer with read bytes.
    while (not ss.eof())
        sx->emplace_back(ss);

    // Drop extra empty strings.
    while ((sx->size() > 0) and (sx->back().value == 0))
        sx->pop_back();

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& stream, const StringX& sx) noexcept
{   // {{{

    // Output the given string to the stream.
    for (const CharX& cx : sx)
        stream << cx;

    return stream;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
