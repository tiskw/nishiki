////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: utils.hxx                                                                   ///
///                                                                                              ///
/// A collection of utility functions used in NiShiKi.                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_HXX
#define UTILS_HXX

#include "string_x.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))


////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
clip(int32_t x, int32_t x_min, int32_t x_max);
// [Abstract]
//   Clip value.
//
// [Args]
//   x     (int32_t): [IN] Value to be clipped.
//   x_min (int32_t): [IN] Minimum value.
//   x_max (int32_t): [IN] Maximum value.
//
// [Returns]
//   (int32_t): Clipped value.

std::vector<StringX>
column(const std::vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin)
noexcept;
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
drop_whitespace_tokens(std::vector<StringX>& tokens)
noexcept;
// [Abstract]
//   Drop white-space tokens.
//
// [Args]
//   tokens (std::vector<StringX>&): [IN/OUT] Target list of tokens.
//
// [Returns]
//   void

StringX
get_common_substring(const std::vector<StringX>& texts)
noexcept;
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
get_cwd(void)
noexcept;
// [Abstract]
//   Get current working directory.
//
// [Args]
//   void
//
// [Returns]
//   (std::string): Current working directory.

std::string
get_date(void)
noexcept;
// [Abstract]
//   Get current date as a string.
//
// [Args]
//   void
//
// [Returns]
//   (std::string): Date string.

std::string
get_time(void)
noexcept;
// [Abstract]
//   Get current time as a string.
//
// [Args]
//   void
//
// [Returns]
//   (std::string): Time string.

std::string
get_git_branch_info(void)
noexcept;
// [Abstract]
//   Get git status info as a string.
//
// [Args]
//   void
//
// [Returns]
//   (std::string): Git branch information.

std::vector<StringX>
get_system_commands(void)
noexcept;
// [Abstract]
//   Get a list of all available commands in the system.
//
// [Args]
//   void
//
// [Returns]
//   (std::vector<StringX>): List of available command names.

std::string
replace(const std::string& target, const std::string& oldstr, const std::string& newstr)
noexcept;
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
run_command(const std::string& command, bool strip_output = true)
noexcept;
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
split(const std::string& str)
noexcept;
// [Abstract]
//   Split the given string with the given delimiter.
//
// [Args]
//   str (const std::string&): [IN] Target string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.

std::vector<std::string>
split(const std::string& str, const std::string& delim)
noexcept;
// [Abstract]
//   Split the given string with the given delimiter.
//
// [Args]
//   str   (const std::string&): [IN] Target string.
//   delim (const std::string&): [IN] Delimiter string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.

bool
startswith(const std::string& s, const std::string& t)
noexcept;
// [Abstract]
//   Returns true if the given string `s` is started from `t`.
//
// [Args]
//   s (const std::string&): [IN] Target string.
//   t (const std::string&): [IN] Match string.
//
// [Returns]
//   (bool): True if `s` is started with `t`.

std::string
strip(const std::string &str)
noexcept;
// [Abstract]
//   Strip white-spaces from both front and end of the given string.
//
// [Args]
//   str (const std::string&): [IN] Target string to be stripped.
//
// [Returns]
//   (std::string): Stripped string.


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
