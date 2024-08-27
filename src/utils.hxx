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

void
drop_whitespace_tokens(std::vector<StringX>& tokens) noexcept;
// [Abstract]
//   Drop white-space tokens.
//
// [Args]
//   tokens (std::vector<StringX>&): [IN/OUT] Target list of tokens.

StringX
get_common_substring(const std::vector<StringX>& texts) noexcept;
// [Abstract]
//   Get common substring of the given strings.
//
// [Args]
//   texts (const std::vector<StringX>&): [IN] List of strings.
//
// [Returns]
//   (StringX): Common string in the given list of strings.

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

void
print_message_and_exit(const char* message) noexcept;
// [Abstract]
//   Print message and exit.
//
// [Args]
//   message (const char*): [IN] Message to be shown.

std::string
replace(const std::string& target, const std::string& oldstr, const std::string& newstr) noexcept;
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

std::vector<std::string>
split(const std::string& str) noexcept;
// [Abstract]
//   Split the given string with whitespaces.
//
// [Args]
//   str (const std::string&): [IN] Target string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.

std::vector<std::string>
split(const std::string& str, const std::string& delim) noexcept;
// [Abstract]
//   Split the given string with the given delimiter.
//
// [Args]
//   str   (const std::string&): [IN] Target string.
//   delim (const std::string&): [IN] Delimiter string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.

std::string
strip(const std::string &str) noexcept;
// [Abstract]
//   Strip white-spaces from both front and end of the given string.
//
// [Args]
//   str (const std::string&): [IN] Target string to be stripped.
//
// [Returns]
//   (std::string): Stripped string.

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
