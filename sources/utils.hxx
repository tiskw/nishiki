////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: utils.hxx                                                                   ///
///                                                                                              ///
/// A collection of utility functions used in NiShiKi.                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_HXX
#define UTILS_HXX

// Include the headers of STL.
#include <algorithm>
#include <cstdint>
#include <regex>
#include <set>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
column(const std::vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin) noexcept;
// [Abstract]
//   Format strings in the column style.
//
// [Args]
//   texts  (const std::vector<StringX>&): [IN] Input texts.
//   width  (uint16_t)                   : [IN] Maximum width of display.
//   height (uint16_t)                   : [IN] Maximum height of display.
//   margin (uint16_t)                   : [IN] Minimum margin between each text.
//
// [Returns]
//   (std::vector<StringX>): Formated strings in column style.

constexpr inline void
drop_whitespace_tokens(std::vector<StringX>& tokens) noexcept
// [Abstract]
//   Drop white-space tokens.
//
// [Args]
//   tokens (std::vector<StringX>&): [IN/OUT] Target list of tokens.
//
{   // {{{

    constexpr auto is_whitespace_token = [](const StringX& token) noexcept -> bool
    // [Abstract]
    //   Returns true if the given token is whitespace token.
    //
    // [Args]
    //   token (const StringX&): [IN] Target token.
    //
    // [Returns]
    //   (bool): True if the given token is a whitespace token.
    {
        return (token.size() == 0) or (token[0].value == ' ');
    };

    // Drop white-space tokens.
    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), is_whitespace_token), tokens.cend());

};  // }}}

constexpr inline StringX
get_common_substring(const std::vector<StringX>& texts) noexcept
// [Abstract]
//   Get common substring of the given strings.
//
// [Args]
//   texts (const std::vector<StringX>&): [IN] List of strings.
//
// [Returns]
//   (StringX): Common string in the given list of strings.
//
{   // {{{

    // Initialize output string.
    StringX result;

    // Returns empty string if the size of the given text list is zero.
    if (texts.size() == 0)
        return result;

    // Find minimul length of the given texts.
    uint32_t min_size = texts[0].size();
    for (uint32_t n = 1; n < texts.size(); ++n)
        min_size = std::min(min_size, static_cast<uint32_t>(texts[n].size()));

    // Check consistency for each character and append to the result string.
    for (uint32_t m = 0; m < min_size; ++m)
    {
        // Check the character consistency.
        for (uint32_t n = 1; n < texts.size(); ++n)
            if (texts[n][m].value != texts[0][m].value)
                return result;

        // Append to the result string.
        result += texts[0][m];
    }

    return result;

};  // }}}

// Get current directory.
std::string
get_cwd(void) noexcept;
// [Abstract]
//   Get current working directory.
//
// [Returns]
//   (std::string): Current working directory.

std::string
get_date(void) noexcept;
// [Abstract]
//   Get current date as a string.
//
// [Returns]
//   (std::string): Date string.

void
get_terminal_size(uint16_t* width, uint16_t* height) noexcept;
// [Abstract]
//   Get terminal size.
//
// [Args]
//   width  (int*): Integer pointer, or nullptr.
//   height (int*): Integer pointer, or nullptr.

std::string
get_time(void) noexcept;
// [Abstract]
//   Get current time as a string.
//
// [Returns]
//   (std::string): Time string.

std::string
get_git_branch_info(void) noexcept;
// [Abstract]
//   Get git status info as a string.
//
// [Returns]
//   (std::string): Git branch information.

std::vector<StringX>
get_system_commands(void) noexcept;
// [Abstract]
//   Get a list of all available commands in the system.
//
// [Returns]
//   (std::vector<StringX>): List of available command names.

constexpr inline uint64_t
hash(const char* str) noexcept
// [Abstract]
//   Compute a hash value of the given string.
//
// [Returns]
//   (uint64_t): Hash value of the string.
//
{   // {{{

    uint64_t value = 14695981039346656037UL;

    while (*str != '\0')
        value = (1099511628211UL * value) ^ *(str++);

    return value;

};  // }}}

void
print_message_and_exit(const char* message) noexcept;
// [Abstract]
//   Print message and exit.
//
// [Args]
//   message (const char*): [IN] Message to be shown.

constexpr inline std::string
replace(const std::string& target, const std::string& oldstr, const std::string& newstr) noexcept
// [Abstract]
//   Replace string.
//
// [Args]
//   target (const std::string&): [IN] Target string to be replaced.
//   oldstr (const std::string&): [IN] This string will be replaced to `newstr`.
//   newstr (const std::string&): [IN] The `oldstr` will be replaced to this string.
//
// [Returns]
//   (std::string): Replaced string.
//
{   // {{{

    // Create copy of the input string.
    std::string replaced = target;

    // Find the old string in the target string.
    std::string::size_type pos = replaced.find(oldstr);
 
    while (pos != std::string::npos)
    {
        // Replace the old string to the new string.
        replaced.replace(pos, oldstr.length(), newstr);

        // Find the old string again.
        pos = replaced.find(oldstr, pos + newstr.length());
    }

    // Returns the replaced string.
    return replaced;

};  // }}}

std::string
run_command(const std::string& command, bool strip_output = true) noexcept;
// [Abstract]
//   Run a external command and get the returned value as a string.
//
// [Args]
//   command      (const char*): [IN] External command to be run.
//   strip_output (bool)       : [IN] If true, the return value will be stripped.
//
// [Returns]
//   (std::string): Returns value of the external command.

constexpr inline std::vector<std::string>
split(const std::string& str) noexcept
// [Abstract]
//   Split the given string with whitespaces.
//
// [Args]
//   str (const std::string&): [IN] Target string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.
//
{   // {{{

    // Initialize the returned value.
    std::vector<std::string> result;

    // Pattern for splitting.
    const std::regex pattern("\\S+");

    // Split given string using the pattern.
    for (std::sregex_iterator it(std::begin(str), std::end(str), pattern), end; it != end; ++it)
        result.emplace_back(it->str());

    return result;

};  // }}}

constexpr inline std::vector<std::string>
split(const std::string& str, const std::string& delim) noexcept
// [Abstract]
//   Split the given string with the given delimiter.
//
// [Args]
//   str   (const std::string&): [IN] Target string.
//   delim (const std::string&): [IN] Delimiter string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.
//
{   // {{{

    // Initialize the output array.
    std::vector<std::string> result;

    // Do nothing if the delimiter is an empty string.
    if (delim.size() == 0)
    {
        result.push_back(str);
        return result;
    }

    // Initialize offset which indicates current position.
    std::string::size_type offset = 0;

    while (true)
    {
        // Find next target.
        const std::string::size_type pos = str.find(delim, offset);

        // Returns if no next target found.
        if (pos == std::string::npos)
        {
            result.push_back(str.substr(offset));
            return result;
        }

        // Otherwise, memorize the found target and update the offset value.
        result.push_back(str.substr(offset, pos - offset));
        offset = pos + delim.size();
    }

};  // }}}

constexpr inline std::string
strip(const std::string &str) noexcept
// [Abstract]
//   Strip white-spaces from both front and end of the given string.
//
// [Args]
//   str (const std::string&): [IN] Target string to be stripped.
//
// [Returns]
//   (std::string): Stripped string.
//
{   // {{{

    constexpr auto is_not_space = [](unsigned char ch) noexcept -> bool
    // [Abstract]
    //   Returns true if the given character is not whitespace.
    //
    // [Args]
    //   ch (unsigned char): [IN] Target character.
    //
    // [Returns]
    //   (bool): True if not a space.
    {
        return !std::isspace(ch);
    };

    constexpr auto lstrip = [is_not_space](const std::string &src) noexcept -> std::string
    // [Abstract]
    //   Strip white-spaces from the front.
    //
    // [Args]
    //   str (const std::string&): [IN] Target string to be stripped.
    //
    // [Returns]
    //   (std::string): Stripped string.
    {
        std::string str = std::string(src);

        str.erase(str.begin(), std::find_if(str.begin(), str.end(), is_not_space));

        return str;
    };

    constexpr auto rstrip = [is_not_space](const std::string &src) noexcept -> std::string
    // [Abstract]
    //   Strip white-spaces from the end.
    //
    // [Args]
    //   str (const std::string&): [IN] Target string to be stripped.
    //
    // [Returns]
    //   (std::string): Stripped string.
    {
        std::string str = std::string(src);

        str.erase(std::find_if(str.rbegin(), str.rend(), is_not_space).base(), str.end());

        return str;
    };

    return rstrip(lstrip(str));

};  // }}}

template <typename T>
constexpr inline std::set<T>
to_set(const std::vector<T>& vec) noexcept
// [Abstract]
//
// [Args]
//
// [Returns]
//
{   // {{{

    return std::set<T>(vec.begin(), vec.end());

}   // }}}

template <typename T_in, typename T_out, typename F>
inline std::vector<T_out>
transform(const std::vector<T_in>& xs, F&& func) noexcept
// [Abstract]
//   Similar to std::transform, but returns array instance.
//
// [Args]
//   xs   (const std::vector<T_in>&): Input vector.
//   func (F&&)                     : Transform function.
//
// [Returns]
//   (std::vector<T_out>): Output vector.
//
{   // {{{

    // Create array instance and reserve array size.
    std::vector<T_out> ys(xs.size());

    // Apply transform.
    std::transform(xs.cbegin(), xs.cend(), ys.begin(), func);

    // Returns the result array.
    return ys;

}   // }}}

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
