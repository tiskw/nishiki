////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: utils.cxx                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "utils.hxx"

// Include the headers of STL.
#include <fstream>
#include <random>
#include <unistd.h>
#include <sys/ioctl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<StringX> column(const Vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin) noexcept
{   // {{{

    constexpr auto get_shape = [](const Vector<uint16_t>& ws, uint16_t width, uint16_t margin, uint16_t row) noexcept -> Pair<uint16_t, bool>
    // [Abstract]
    //   Compute shape of column style display.
    //
    // [Args]
    //   ws     (const Vector<uint16_t>&): [IN] Length of each text.
    //   width  (uint16_t)               : [IN] Maximum width of display.
    //   margin (uint16_t)               : [IN] Minimum margin between each text.
    //   row    (uint16_t)               : [IN] Number of rows.
    //
    // [Returns]
    //   (Pair<uint16_t, bool>): A pair of (number of columns, true if all texts can be shown).
    {
        uint16_t wid_total = 0;

        // Create columns and append them to each line.
        for (uint16_t col = 0; true; ++col)
        {
            // Compute start/end index of the texts used in the current column
            const size_t idx_bgn = col * row;
            const size_t idx_end = std::min(idx_bgn + row, ws.size());

            // Compute maximum width of texts used in the current column.
            const uint16_t wid_max = *std::max_element(ws.begin() + idx_bgn, ws.begin() + idx_end);

            // Increment of width by this column.
            const uint16_t wid_inc = ((col > 0) ? margin : 0) + wid_max;

            // Exit if the current width exceeds the maximum width.
            if ((wid_total + wid_inc) >= width)
                return Pair(std::max((uint16_t) 1, col), false);

            // Exit if all text was used.
            if (idx_end == ws.size())
                return Pair((uint16_t) (col + 1), true);

            // Update current width.
            wid_total += wid_inc;
        }
    };

    constexpr auto get_optimal_height = [get_shape](const Vector<uint16_t>& ws, uint16_t width, uint16_t height, uint16_t margin) noexcept -> Pair<uint16_t, int16_t>
    // [Abstract]
    //   Compute optimal shape (rows and columns) of column style display.
    //
    // [Args]
    //   texts  (const Vector<StringX>&): [IN] Input texts.
    //   width  (uint16_t)              : [IN] Maximum width of display.
    //   height (uint16_t)              : [IN] Maximum height of display.
    //   margin (uint16_t)              : [IN] Minimum margin between each text.
    //
    // [Returns]
    //   (Pair<uint16_t, uint16_t>): A pair of (rows of the optimal shape, columns of the optimal shape).
    {
        for (uint16_t row = 1; row < height; ++row)
        {
            // Compute shape for each row.
            const auto& [col, finished] = get_shape(ws, width, margin, row);

            // Immediately determine optimal shape if all texts can be shown.
            if (finished)
                return Pair(row, col);
        }

        // Compute column if row is equal with the maximum height.
        const auto& [col, _] = get_shape(ws, width, margin, height);

        return Pair(height, col);
    };

    // Prepare output lines.
    Vector<StringX> lines;
    for (uint16_t row = 0; row < height; ++row)
        lines.emplace_back();

    // Do nothing if no text is given.
    if (texts.size() == 0)
        return lines;

    // Get width of each text.
    Vector<uint16_t> ws = transform<StringX, uint16_t>(texts, [](const StringX& sx) { return sx.width(); });

    // Compute optimal shape (rows and columns).
    const auto& [rows, cols] = get_optimal_height(ws, width, height, margin);

    // Initialize total width.
    uint16_t width_total = 0;

    // Create columns and append them to each line.
    for (uint16_t col = 0; col < cols; ++col)
    {
        // Compute start/end index of the texts used in the current column
        const size_t idx_bgn = col * rows;
        const size_t idx_end = std::min(idx_bgn + rows, ws.size());

        // Compute maximum width of texts used in the current column.
        const uint16_t wid_max = *std::max_element(ws.begin() + idx_bgn, ws.begin() + idx_end);

        // Append texts to each line.
        for (uint16_t idx = idx_bgn; idx < idx_end; ++idx)
            lines[idx % rows] += texts[idx] + CharX(' ') * (wid_max + margin - ws[idx]);

        // Update total width.
        width_total += wid_max + margin;

        // Exit function if the current width exceeds the given width.
        if (width_total > width) break;
    }

    return lines;

}   // }}}

void drop_whitespace_tokens(Vector<StringX>& tokens) noexcept
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

String get_date(void) noexcept
{   // {{{

    // Get current date.
    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    // Format the date as a string.
    char buffer[64] = {'\0'};
    sprintf(buffer, "%04d-%02d-%02d", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday);

    return String(buffer);

}   // }}}

String get_random_string(int16_t length) noexcept
{   // {{{

    constexpr char target_chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    // Initialize the random generator.
    std::random_device seed_gen;
    std::mt19937       engine(seed_gen());

    // Define a wrapper of the random number generator.
    std::uniform_int_distribution<int16_t> dist(0, sizeof(target_chars) - 1);

    // Generate the random string.
    String rand_str;
    for (int16_t idx = 0; idx < length; ++idx)
        rand_str += target_chars[dist(engine)];

    return rand_str;

}   // }}}

TermSize get_terminal_size(void) noexcept
{   // {{{

    // Get terminal size.
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Set values.
    TermSize term_size = {w.ws_row, w.ws_col};

    return term_size;

}   // }}}

String get_time(void) noexcept
{   // {{{

    // Get the current time.
    time_t now = time(NULL);
    struct tm p;
    localtime_r(&now, &p);

    // Format the time as a string.
    char buffer[64] = {'\0'};
    sprintf(buffer, "%02d:%02d:%02d", p.tm_hour, p.tm_min, p.tm_sec);

    return String(buffer);

}   // }}}

Vector<StringX> get_system_commands(void) noexcept
{   // {{{

    Vector<StringX> result;

    // Split PATH by ':'.
    Vector<String> target_paths = split(String(std::getenv("PATH")), ":");

    // Remove duplicated taregt path.
    target_paths.erase(std::unique(target_paths.begin(), target_paths.end()), target_paths.end());

    // Search all directories in PATH and get all executable files.
    for (const String& path : target_paths)
    {
        // Skip if the path is not a directory.
        if (std::filesystem::is_directory(path))
        {
            // Appand all files in the directory.
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
            {
                // Skip if the path is not a regular file or symbolic link.
                if (entry.is_regular_file() or entry.is_symlink())
                {
                    // Get command name.
                    StringX cmd = StringX(entry.path().filename().c_str());

                    // Add the command name.
                    result.emplace_back(cmd);
                }
            }
        }
    }

    // Sort the array of command names.
    std::sort(result.begin(), result.end());

    // Remove duplicated command names.
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;

}   // }}}

Vector<String> read_lines(const String& path) noexcept
{   // {{{

    // Initialize the returned value.
    Vector<String> lines;

    // Open input stream.
    std::ifstream ifs(path);

    // Read the file contents.
    for (String line; getline(ifs, line);)
        lines.push_back(line);

    return lines;

}   // }}}

String replace(const String& target, const String& oldstr, const String& newstr) noexcept
{   // {{{

    // Create copy of the input string.
    String replaced = target;

    // Find the old string in the target string.
    String::size_type pos = replaced.find(oldstr);
 
    while (pos != String::npos)
    {
        // Replace the old string to the new string.
        replaced.replace(pos, oldstr.length(), newstr);

        // Find the old string again.
        pos = replaced.find(oldstr, pos + newstr.length());
    }

    // Returns the replaced string.
    return replaced;

}   // }}}

String run_command(const String& command, bool strip_output) noexcept
{   // {{{

    // Initialize output variable.
    String result;

    // Make a command string that will be called actually.
    const String command_called = command + " 2> /dev/null";

    // Open pipe and run the command.
    FILE *pipe = popen(command_called.c_str(), "r");

    // Return an empty string if failed to open the pipe.
    if (pipe == NULL) return String("");

    // Get the returned value of the command.
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    // Close the pipe.
    pclose(pipe);

    // Returns the output value.
    return strip_output ? strip(result) : result;

}   // }}}

Vector<String> split(const String& str, const String& delim) noexcept
{   // {{{

    // Initialize the output array.
    Vector<String> result;

    // Do nothing if the delimiter is an empty string.
    if (delim.size() == 0)
        return {str};

    // Initialize offset which indicates current position.
    String::size_type offset = 0;

    while (true)
    {
        // Find next target.
        const String::size_type pos = str.find(delim, offset);

        // Returns if no next target found.
        if (pos == String::npos)
        {
            result.push_back(str.substr(offset));
            return result;
        }

        // Otherwise, memorize the found target and update the offset value.
        result.push_back(str.substr(offset, pos - offset));
        offset = pos + delim.size();
    }

}   // }}}

String strip(const String& str, bool left, bool right) noexcept
{   // {{{

    // Define a function that returns true if the given character is not whitespace.
    constexpr auto is_not_space = [](unsigned char ch) noexcept { return !std::isspace(ch); };

    // Create a copy of input string.
    String sx = String(str);

    // Strip from the left side.
    if (left == true)
        sx.erase(sx.begin(), std::find_if(sx.begin(), sx.end(), is_not_space));

    // Strip from the right side.
    if (right == true)
        sx.erase(std::find_if(sx.rbegin(), sx.rend(), is_not_space).base(), sx.end());

    return sx;

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
