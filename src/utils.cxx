////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: utils.cxx
//
// A collection of utility functions used in NiShiKi.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "utils.hxx"

#include <filesystem>
#include <regex>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
clip(int32_t x, int32_t x_min, int32_t x_max)
{   // {{{

    if      (x < x_min) return x_min;
    else if (x_max < x) return x_max;
    else                return x;

}   // }}}

std::vector<StringX>
column(const std::vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin)
noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Returns maximum value of the given array.
    //
    // [Args]
    //   array   (const std::vector<uint16_t>&): Target vector.
    //   idx_bgn (uint32_t)                    : Start index.
    //   idx_end (uint32_t)                    : End index.
    //
    // [Returns]
    //   (std::vector<StringX>): List of available command names.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto maximum = [](const std::vector<uint16_t>& array, uint32_t idx_bgn, uint32_t idx_end) noexcept
    {
        uint16_t val_max = array[idx_bgn];
    
        for (size_t idx = idx_bgn + 1; idx < idx_end; ++idx)
            val_max = (val_max < array[idx]) ? array[idx] : val_max;
    
        return val_max;
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Compute shape of column style display.
    //
    // [Args]
    //   ws     (const std::vector<uint16_t>&): [IN] Length of each text.
    //   width  (uint16_t)                    : [IN] Maximum width of display.
    //   margin (uint16_t)                    : [IN] Minimum margin between each text.
    //   row    (uint16_t)                    : [IN] Number of rows.
    //
    // [Returns]
    //   (std::pair<uint16_t, bool>): A pair of (number of columns, true if all texts can be shown).
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto get_shape = [maximum](const std::vector<uint16_t>& ws, uint16_t width, uint16_t margin, uint16_t row) noexcept
    {
        uint16_t wid_total = 0;

        // Create columns and append them to each line.
        for (uint16_t col = 0; true; ++col)
        {
            // Compute start/end index of the texts used in the current column
            size_t idx_bgn = col * row;
            size_t idx_end = MIN(idx_bgn + row, ws.size());

            // Compute maximum width of texts used in the current column.
            uint16_t wid_max = maximum(ws, idx_bgn, idx_end);

            // Increment of width by this column.
            uint16_t wid_inc = ((col > 0) ? margin : 0) + wid_max;

            // Exit if the current width exceeds the maximum width.
            if ((wid_total + wid_inc) >= width)
                return std::make_pair(MAX((uint16_t) 1, col), false);

            // Exit if all text was used.
            if (idx_end == ws.size())
                return std::make_pair((uint16_t) (col + 1), true);

            // Update current width.
            wid_total += wid_inc;
        }
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Compute optimal shape (rows and columns) of column style display.
    //
    // [Args]
    //   texts  (const std::vector<StringX>&): [IN] Input texts.
    //   width  (uint16_t)                   : [IN] Maximum width of display.
    //   height (uint16_t)                   : [IN] Maximum height of display.
    //   margin (uint16_t)                   : [IN] Minimum margin between each text.
    //
    // [Returns]
    //   (std::pair<uint16_t, uint16_t>): A pair of (rows of the optimal shape, columns of the optimal shape).
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto get_optimal_height = [get_shape](const std::vector<uint16_t>& ws, uint16_t width, uint16_t height, uint16_t margin) noexcept
    {
        for (uint16_t row = 1; row < height; ++row)
        {
            // Compute shape for each row.
            auto [col, finished] = get_shape(ws, width, margin, row);

            // Immediately determine optimal shape if all texts can be shown.
            if (finished)
                return std::make_pair(row, col);
        }

        // Compute column if row is equal with the maximum height.
        const auto& [col, _] = get_shape(ws, width, margin, height);

        return std::make_pair(height, col);
    };

    // Prepare output lines.
    std::vector<StringX> lines;
    for (uint16_t row = 0; row < height; ++row)
        lines.emplace_back("");

    // Do nothing if no text is given.
    if (texts.size() == 0)
        return lines;

    // Get width of each text.
    std::vector<uint16_t> ws;
    for (const StringX& sx : texts)
        ws.push_back(sx.width());

    // Compute optimal shape (rows and columns).
    const auto [rows, cols] = get_optimal_height(ws, width, height, margin);

    // Initialize total width.
    uint16_t width_total = 0;

    // Create columns and append them to each line.
    for (uint16_t col = 0; col < cols; ++col)
    {
        // Compute start/end index of the texts used in the current column
        const size_t idx_bgn = col * rows;
        const size_t idx_end = MIN(idx_bgn + rows, ws.size());

        // Compute maximum width of texts used in the current column.
        const uint16_t wid_max = maximum(ws, idx_bgn, idx_end);

        // Append texts to each line.
        for (uint16_t idx = idx_bgn; idx < idx_end; ++idx)
            lines[idx % rows] += texts[idx] + CharX(" ") * (wid_max + margin - ws[idx]);

        // Update total width.
        width_total += wid_max + margin;

        // Exit function if the current width exceeds the given width.
        if (width_total > width) break;
    }

    return lines;

}   // }}}

void
drop_whitespace_tokens(std::vector<StringX>& tokens)
noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // [Args]
    //   token (const StringX&): [IN] Target token.
    //
    // [Returns]
    //   (bool): True if the given token is a white-space token.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto is_whitespace_token = [](const StringX& token) noexcept
    {
        return (token.size() == 0) or (token[0].value == ' ');
    };

    // Drop white-space tokens.
    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), is_whitespace_token), tokens.cend());

}   // }}}

StringX
get_common_substring(const std::vector<StringX>& texts)
noexcept
{   // {{{

    // Initialize output string.
    StringX result;

    // Returns empty string if the size of the given text list is zero.
    if (texts.size() == 0)
        return result;

    // Find minimul length of the given texts.
    uint32_t min_size = texts[0].size();
    for (uint32_t n = 1; n < texts.size(); ++n)
        min_size = MIN(min_size, texts[n].size());

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

}   // }}}

std::string
get_cwd(void)
noexcept
{   // {{{

    try
    {
        return std::filesystem::current_path();
    }
    catch (const std::exception& err)
    {
        printf("\033[33mNiShiKi: Error: %s L.%d: get_cwd(): %s\033[m", __FILE__, __LINE__, err.what());
        return "?";
    }

}   // }}}

std::string
get_date(void)
noexcept
{   // {{{

    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    std::stringstream ss;
    std::string       str;

    ss << std::setw(4) << std::setfill('0') << p.tm_year + 1900 << "-"
       << std::setw(2) << std::setfill('0') << p.tm_mon  + 1    << "-"
       << std::setw(2) << std::setfill('0') << p.tm_mday;
    ss >> str;

    return str;

}   // }}}

std::string
get_time(void)
noexcept
{   // {{{

    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    std::stringstream ss;
    std::string       str;

    ss << std::setw(2) << std::setfill('0') << p.tm_hour << ":"
       << std::setw(2) << std::setfill('0') << p.tm_min  << ":"
       << std::setw(2) << std::setfill('0') << p.tm_sec;
    ss >> str;

    return str;

}   // }}}

std::string
get_git_branch_info(void)
noexcept
{   // {{{

    // Get git branch and status information.
    const std::string branch = run_command("git rev-parse --abbrev-ref HEAD 2> /dev/null");
    const std::string status = run_command("git status --porcelain 2> /dev/null");

    if      ((branch.size() > 0) and (status.size() > 0)) return ("\033[33m" + branch + "!\033[m");
    else if ( branch.size() > 0                         ) return ("\033[32m" + branch +  "\033[m");
    else                                                  return branch;

}   // }}}

std::vector<StringX>
get_system_commands(void)
noexcept
{   // {{{

    std::vector<StringX> result;

    for (const std::string& path : split(std::string(std::getenv("PATH")), ":"))
        for (const auto& x : std::filesystem::directory_iterator(path))
            result.emplace_back(x.path().filename().c_str());

    // Sort the array of command names.
    std::sort(result.begin(), result.end());

    // Remove duplicated command names.
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;

}   // }}}

std::string
replace(const std::string& target, const std::string& oldstr, const std::string& newstr)
noexcept
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

std::string
run_command(const std::string& command, bool strip_output)
noexcept
{   // {{{

    // Initialize output variable.
    std::string result;

    // Open pipe and run the command.
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe == NULL)
        return std::string("");

    // Get the returned value of the command.
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    // Close the pipe.
    pclose(pipe);

    // Returns the output value.
    if (strip_output) { return strip(result); }
    else              { return result;        }

}   // }}}

std::vector<std::string>
split(const std::string& str)
noexcept
{   // {{{

    // Initialize the returned value.
    std::vector<std::string> result;

    // Pattern for splitting.
    const std::regex pattern("\\S+");

    // Split given string using the pattern.
    for (std::sregex_iterator it(std::begin(str), std::end(str), pattern), end; it != end; ++it)
        result.emplace_back(it->str());

    return result;

}   // }}}
 
std::vector<std::string>
split(const std::string& str, const std::string& delim)
noexcept
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

}   // }}}

bool
startswith(const std::string& s, const std::string& t)
noexcept
{   // {{{

    return ((s.size() >= t.size()) and std::equal(std::begin(t), std::end(t), std::begin(s)));

}   // }}}

std::string
strip(const std::string& str)
noexcept
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Strip white-spaces from the front.
    //
    // [Args]
    //   str (const std::string&): [IN] Target string to be stripped.
    //
    // [Returns]
    //   (std::string): Stripped string.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto
    lstrip = [](const std::string &src)
    {
        constexpr auto is_not_space = [](unsigned char ch)
        {
            return !std::isspace(ch);
        };

        std::string str = std::string(src);

        str.erase(str.begin(), std::find_if(str.begin(), str.end(), is_not_space));

        return str;
    };

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Strip white-spaces from the end.
    //
    // [Args]
    //   str (const std::string&): [IN] Target string to be stripped.
    //
    // [Returns]
    //   (std::string): Stripped string.
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto
    rstrip = [](const std::string &src)
    {
        constexpr auto is_not_space = [](unsigned char ch)
        {
            return !std::isspace(ch);
        };

        std::string str = std::string(src);

        str.erase(std::find_if(str.rbegin(), str.rend(), is_not_space).base(), str.end());

        return str;
    };

    return rstrip(lstrip(str));

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
