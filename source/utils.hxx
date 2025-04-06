////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: utils.hxx                                                                   ///
///                                                                                              ///
/// A collection of utility functions used in NiShiKi2.                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_HXX
#define UTILS_HXX

// Include the headers of STL.
#include <algorithm>
#include <cstdint>

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<StringX> column(const Vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin = 3) noexcept;
// [Abstract]
//   Format strings in the column style.
//
// [Args]
//   texts  (const Vector<StringX>&): [IN] Input texts.
//   width  (uint16_t)              : [IN] Maximum width of display.
//   height (uint16_t)              : [IN] Maximum height of display.
//   margin (uint16_t)              : [IN] Minimum margin between each text.
//
// [Returns]
//   (Vector<StringX>): Formated strings in column style.

void drop_whitespace_tokens(Vector<StringX>& tokens) noexcept;
// [Abstract]
//   Drop white-space tokens.
//
// [Args]
//   tokens (Vector<StringX>&): [IN/OUT] Target list of tokens.

StringX get_common_substring(const std::vector<StringX>& texts) noexcept;
// [Abstract]
//   Get common substring of the given strings.
//
// [Args]
//   texts (const std::vector<StringX>&): [IN] List of strings.
//
// [Returns]
//   (StringX): Common string in the given list of strings.

String get_cwd(void) noexcept;
// [Abstract]
//   Get current working directory.
//
// [Returns]
//   (String): Current working directory.

String get_date(void) noexcept;
// [Abstract]
//   Get current date as a string.
//
// [Returns]
//   (String): Date string.

String get_git_info(void) noexcept;
// [Abstract]
//   Get git status info as a string.
//
// [Returns]
//   (String): Git branch information.

String get_random_string(int16_t length) noexcept;
// [Abstract]
//   Returns random string.
//
// [Args]
//   length (int16_t): Length of the random string.
//
// [Returns]
//   (String): Random string with the specified length.

TermSize get_terminal_size() noexcept;
// [Abstract]
//   Get terminal size.
//
// [Returns]
//   (TermSize): Terminal size.

String get_time(void) noexcept;
// [Abstract]
//   Get current time as a string.
//
// [Returns]
//   (String): Time string.

std::vector<StringX> get_system_commands(void) noexcept;
// [Abstract]
//   Get a list of all available commands in the system.
//
// [Returns]
//   (std::vector<StringX>): List of available command names.

static constexpr uint64_t hash(const char* str)
// [Abstract]
//   Compute hash value of the given string.
//
// [Args]
//   str (const char*): The target string.
//
// [Returns]
//   (uint64_t): Hash value of the given string.
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
    for (size_t idx = 0; str[idx] != '\0'; ++idx)
        hash_value = prime * (hash_value ^ str[idx]);

    return hash_value;

}   // }}}

Vector<String> read_lines(const String& path) noexcept;
// [Abstract]
//   Read all lines from a text file.
//
// [Args]
//   path (const String&): [IN] Target filepath.
//
// [Returns]
//   (Vector<String>): Contents of the target file.

String replace(const String& target, const String& oldstr, const String& newstr) noexcept;
// [Abstract]
//   Replace string.
//
// [Args]
//   target (const String&): [IN] Target string to be replaced.
//   oldstr (const String&): [IN] This string will be replaced to `newstr`.
//   newstr (const String&): [IN] The `oldstr` will be replaced to this string.
//
// [Returns]
//   (String): Replaced string.

String run_command(const String& command, bool strip_output = true) noexcept;
// [Abstract]
//   Run a external command and get the returned value as a string.
//
// [Args]
//   command      (const char*): [IN] External command to be run.
//   strip_output (bool)       : [IN] If true, the return value will be stripped.
//
// [Returns]
//   (std::string): Returns value of the external command.

Vector<String> split(const String& str, const String& delim) noexcept;
// [Abstract]
//   Split the given string with the given delimiter.
//
// [Args]
//   str   (const String&): [IN] Target string.
//   delim (const String&): [IN] Delimiter string.
//
// [Returns]
//   (std::vector<std::string>): Split strings.

String strip(const String &str) noexcept;
// [Abstract]
//   Strip white-spaces from both front and end of the given string.
//
// [Args]
//   str (const String&): [IN] Target string to be stripped.
//
// [Returns]
//   (std::string): Stripped string.

template <typename T_in, typename T_out, typename F> std::vector<T_out>
transform(const Vector<T_in>& xs, F&& func) noexcept
// [Abstract]
//   Similar to std::transform, but returns array instance.
//
// [Args]
//   xs   (const Vector<T_in>&): Input vector.
//   func (F&&)                : Transform function.
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

void update_set(Set<StringX>& set, const char* str);
// [Abstract]
//   Update the given set by the comma-seperated values.
//
// [Args]
//   set (Set<StringX>&): [OUT] The set to be updated.
//   str (const char*)  : [IN ] Comma-seperated values.

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
