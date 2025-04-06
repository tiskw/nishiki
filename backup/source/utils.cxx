////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ source file: utils.cxx                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "utils.hxx"

// Include the headers of STL.
#include <cstdio>
#include <filesystem>
#include <unistd.h>
#include <sys/ioctl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
column(const std::vector<StringX>& texts, uint16_t width, uint16_t height, uint16_t margin) noexcept
{   // {{{

    constexpr auto maximum = [](const std::vector<uint16_t>& array, uint32_t idx_bgn, uint32_t idx_end) noexcept -> uint16_t
    // [Abstract]
    //   Returns maximum value of the given array.
    //
    // [Args]
    //   array   (const std::vector<uint16_t>&): Target vector.
    //   idx_bgn (uint32_t)                    : Start index.
    //   idx_end (uint32_t)                    : End index.
    //
    // [Returns]
    //   (uint16_t): Maximum value of the given array.
    {
        uint16_t val_max = array[idx_bgn];

        for (size_t idx = idx_bgn + 1; idx < idx_end; ++idx)
            val_max = (val_max < array[idx]) ? array[idx] : val_max;

        return val_max;
    };

    constexpr auto get_shape = [maximum](const std::vector<uint16_t>& ws, uint16_t width, uint16_t margin, uint16_t row) noexcept -> std::pair<uint16_t, bool>
    // [Abstract]
    //   Compute shape of column style display.
    //
    // [Args]
    //   ws     (const std::vector<uint16_t>&): [IN] Length of each text.
    //   width  (uint16_t)                    : [IN] Maximum width of display.
    //   margin (uint16_t)                    : [IN] Minimum margin between each text.
    //   row    (uint16_t)                    : [IN] Number of rows.
    //
    // [Returns]
    //   (std::pair<uint16_t, bool>): A pair of (number of columns, true if all texts can be shown).
    {
        uint16_t wid_total = 0;

        // Create columns and append them to each line.
        for (uint16_t col = 0; true; ++col)
        {
            // Compute start/end index of the texts used in the current column
            const size_t idx_bgn = col * row;
            const size_t idx_end = std::min(idx_bgn + row, ws.size());

            // Compute maximum width of texts used in the current column.
            const uint16_t wid_max = maximum(ws, idx_bgn, idx_end);

            // Increment of width by this column.
            const uint16_t wid_inc = ((col > 0) ? margin : 0) + wid_max;

            // Exit if the current width exceeds the maximum width.
            if ((wid_total + wid_inc) >= width)
                return std::make_pair(std::max(static_cast<uint16_t>(1), col), false);

            // Exit if all text was used.
            if (idx_end == ws.size())
                return std::make_pair((uint16_t) (col + 1), true);

            // Update current width.
            wid_total += wid_inc;
        }
    };

    constexpr auto get_optimal_height = [get_shape](const std::vector<uint16_t>& ws, uint16_t width, uint16_t height, uint16_t margin) noexcept -> std::pair<uint16_t, int16_t>
    // [Abstract]
    //   Compute optimal shape (rows and columns) of column style display.
    //
    // [Args]
    //   texts  (const std::vector<StringX>&): [IN] Input texts.
    //   width  (uint16_t)                   : [IN] Maximum width of display.
    //   height (uint16_t)                   : [IN] Maximum height of display.
    //   margin (uint16_t)                   : [IN] Minimum margin between each text.
    //
    // [Returns]
    //   (std::pair<uint16_t, uint16_t>): A pair of (rows of the optimal shape, columns of the optimal shape).
    {
        for (uint16_t row = 1; row < height; ++row)
        {
            // Compute shape for each row.
            const auto& [col, finished] = get_shape(ws, width, margin, row);

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
    std::vector<uint16_t> ws = transform<StringX, uint16_t>(texts, [](const StringX& sx) { return sx.width(); });

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

std::string
get_date(void) noexcept
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

void
get_terminal_size(uint16_t* width, uint16_t* height) noexcept
{   // {{{

    // Get terminal size.
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Set values.
    if (width  != nullptr) { *width  = w.ws_col; }
    if (height != nullptr) { *height = w.ws_row; }

}   // }}}

std::string
get_time(void) noexcept
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

std::vector<StringX>
get_system_commands(void) noexcept
{   // {{{

    std::vector<StringX> result;

    // Split PATH by ':'.
    std::vector<std::string> target_paths = split(std::string(std::getenv("PATH")), ":");

    // Remove duplicated taregt path.
    target_paths.erase(std::unique(target_paths.begin(), target_paths.end()), target_paths.end());

    // Search all directories in PATH and get all executable files.
    for (const std::string& path : target_paths)
    {
        // Skip if the path is not a directory.
        if (not std::filesystem::is_directory(path))
            continue;

        // Appand all files in the directory.
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            // Skip if the path is not a regular file or symbolic link.
            if (not (entry.is_regular_file() or entry.is_symlink()))
                continue;

            // Get command name.
            StringX cmd = StringX(entry.path().filename().c_str());

            // Add the command name.
            result.emplace_back(cmd);
        }
    }

    // Sort the array of command names.
    std::sort(result.begin(), result.end());

    // Remove duplicated command names.
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;

}   // }}}

void
print_message_and_exit(const char* message) noexcept
{   // {{{

    // Print the given messasge.
    puts(message);

    // Exit with failure code.
    exit(EXIT_SUCCESS);

}   // }}}

std::string
run_command(const std::string& command, bool strip_output) noexcept
{   // {{{

    // Initialize output variable.
    std::string result;

    // Make a command string that will be called actually.
    const std::string command_called = command + " 2> /dev/null";

    // Open pipe and run the command.
    FILE *pipe = popen(command_called.c_str(), "r");

    // Return an empty string if failed to open the pipe.
    if (pipe == NULL) return std::string("");

    // Get the returned value of the command.
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    // Close the pipe.
    pclose(pipe);

    // Returns the output value.
    if (strip_output) { return strip(result); }
    else              { return result;        }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
