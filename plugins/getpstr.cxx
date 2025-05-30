////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: getpstr.cxx                                                                   ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the headers of STL.
#include <cstdlib>
#include <filesystem>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Color definition
////////////////////////////////////////////////////////////////////////////////////////////////////

const char* RED    = "\x1B[38;2;204;102;102m";
const char* GREEN  = "\x1B[38;2;181;189;104m";
const char* YELLOW = "\x1B[38;2;240;198;116m";
const char* BLUE   = "\x1B[38;2;129;162;190m";
const char* PURPLE = "\x1B[38;2;178;148;187m";
const char* CYAN   = "\x1B[38;2;138;190;183m";
const char* GRAY   = "\x1B[38;2;197;200;198m";
const char* NONE   = "\x1B[0m";

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string replace(const std::string& target, const std::string& oldstr, const std::string& newstr) noexcept
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

}   // }}}

std::string get_cwd(void) noexcept
{   // {{{

    try
    {
        // Get the current directory.
        std::string cwd = std::filesystem::absolute(std::filesystem::current_path());

        // Replace home directory to "~".
        std::string home = getenv("HOME");
        if (cwd.starts_with(home))
            cwd = replace(cwd, home, "~");

        return cwd;
    }
    catch (...)
    {
        return "???";
    }

}   // }}}

std::string get_date(void) noexcept
{   // {{{

    // Get current date.
    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    // Format the date as a string.
    char buffer[64] = {'\0'};
    sprintf(buffer, "%04d-%02d-%02d", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday);

    return std::string(buffer);

}   // }}}

std::string get_time(void) noexcept
{   // {{{

    // Get the current time.
    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    // Format the time as a string.
    char buffer[64] = {'\0'};
    sprintf(buffer, "%02d:%02d:%02d", p.tm_hour, p.tm_min, p.tm_sec);

    return std::string(buffer);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t main(void)
{   // {{{

    // Get necessary variables.
    std::string user = std::getenv("LOGNAME");
    std::string host = std::getenv("NAME");
    std::string date = get_date();
    std::string time = get_time();
    std::string cdir = get_cwd();

    // Compute PS0.
    std::printf("%s%s %s: ", RED,    user.c_str(), NONE);
    std::printf("%s%s %s: ", GREEN,  host.c_str(), NONE);
    std::printf("%s%s %s: ", YELLOW, date.c_str(), NONE);
    std::printf("%s%s %s: ", BLUE,   time.c_str(), NONE);
    std::printf("%s%s %s\n", PURPLE, cdir.c_str(), NONE);

    // Print PS1 for insert and normal mode.
    std::printf("=>> \n");
    std::printf("<<= \n");

    // Print PS2.
    std::printf("... \n");

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
