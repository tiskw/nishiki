////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ header file: utils.hxx
//
// A collection of utility functions used in NiShiKi.
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

// Clip value.
int32_t
clip(int32_t x, int32_t x_min, int32_t x_max);

// Format strings in the column style.
std::vector<StringX>
column(const std::vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin)
noexcept;

// Drop whitespace tokens.
void
drop_whitespace_tokens(std::vector<StringX>& tokens)
noexcept;

// Get common substring of the given strings.
StringX
get_common_substring(const std::vector<StringX>& texts)
noexcept;

// Get current directory.
std::string
get_cwd(void)
noexcept;

// Get current date as a string.
std::string
get_date(void)
noexcept;

// Get current time as a string.
std::string
get_time(void)
noexcept;

// Get git status info as a string.
std::string
get_git_branch_info(void)
noexcept;

// Get a list of all availabe commands in the system.
std::vector<StringX>
get_system_commands(void)
noexcept;

// Print error information.
template <class X> X
print_error(const char* message, const char* err_what, const char* filename, int32_t line_no, const X ret_val)
noexcept;

// Replace string.
std::string
replace(const std::string& target, const std::string& oldstr, const std::string& newstr)
noexcept;

// Run a external command and get the returned value as a string.
std::string
run_command(const std::string& command, bool strip_output = true)
noexcept;

// Split the given string with the given delimiter.
std::vector<std::string>
split(const std::string& str)
noexcept;

// Split the given string with the given delimiter.
std::vector<std::string>
split(const std::string& str, const std::string& delim)
noexcept;

// Returns true if the given string `s` is started from `t`.
bool
startswith(const std::string& s, const std::string& t)
noexcept;

// Strip whitespaces from both front and end of the given string.
std::string
strip(const std::string &str)
noexcept;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
