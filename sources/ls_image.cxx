////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: ls_image.cxx                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "ls_image.hxx"

// Include the headers of STL.
#include <filesystem>
#include <format>
#include <generator>
#include <iostream>

// Include the headers of custom modules.
#include "config.hxx"
#include "path_x.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::generator<std::string>
get_all_image_files(const PathX& path)
// [Abstract]
//
// [Args]
//
// [Returns]
//
{   // {{{

    // Run the "identify" command to find image files.
    std::string output = run_command("identify " + (path / "*").string());

    // Store the output of "identify" to the vector.
    for (const std::string& line : split(output, "\n"))
        co_yield split(strip(line))[0];

}   // }}}

static int32_t
print_images(const std::vector<std::string>& list_path_img, uint16_t w_thumb_px, uint16_t h_thumb_px, uint16_t p_thumb_px) noexcept
// [Abstract]
//
// [Args]
//
// [Returns]
//
{   // {{{

    //
    const uint16_t w_cell_px = w_thumb_px + 2 * p_thumb_px;
    const uint16_t h_cell_px = h_thumb_px + 2 * p_thumb_px;

    //
    std::vector<StringX> tmp_files;

    //
    for (const std::string& path_img : list_path_img)
    {
        //
        tmp_files.push_back(StringX(std::format("tmp_{}.png", tmp_files.size())));

        // Generate an image snippet.
        run_command(std::format(
            "convert -resize {}x{} -extent {}x{} -background black -gravity center {} {}",
            w_thumb_px, h_thumb_px, w_cell_px, h_cell_px, path_img, tmp_files.back().string()
        ));
    }

    //
    run_command(std::format(
        "convert +append {} tmp_all.png",
        StringX(" ").join(tmp_files).string()
    ));

    //
    return std::system(std::format(
        "img2sixel -w {}px tmp_all.png", list_path_img.size() * (w_thumb_px + 2 * p_thumb_px)
    ).c_str());

}   // }}}

static int32_t
print_labels(const std::vector<std::string>& list_path_img, uint16_t col_thumb, uint16_t row_iname) noexcept
// [Abstract]
//
// [Args]
//
// [Returns]
//
{   // {{{

    //
    constexpr uint16_t margin = 2;

    //
    std::vector<StringX> lines;
    for (int16_t idx = 0; idx < row_iname; ++idx)
        lines.push_back(StringX(""));

    for (const std::string& path_img : list_path_img)
    {
        //
        const StringX name = StringX(split(path_img, "/").back());

        //
        std::vector<StringX> strs = StringX(name).chunk(col_thumb - margin);

        for (int16_t row = 0; row < row_iname; ++row)
        {
            //
            StringX sx = (strs.size() > static_cast<size_t>(row)) ? strs[row] : StringX("");

            //
            lines[row] += sx + StringX(std::string(col_thumb - sx.width(), ' '));
        }

        //
        if (name.width() > static_cast<size_t>(row_iname * (col_thumb - margin)))
            lines.back() = lines.back().substr(0, lines.back().width() - 3 - margin) + StringX("..." + std::string(margin, ' '));
    }

    //
    std::cout << StringX("\n").join(lines) << std::endl;

    return 0;

}   // }}}

static int32_t
ls_image_main_dir(const PathX& dirpath) noexcept
// [Abstract]
//
// [Args]
//
// [Returns]
//   (int32_t): Return code (0 means success, the others mean errors).
//
{   // {{{

    // Get terminal size.
    uint16_t cols, rows, w_px, h_px;
    get_terminal_size(&cols, &rows, &w_px, &h_px);

    //
    const uint16_t col_px = w_px / cols;
    const uint16_t row_px = h_px / rows;

    //
    const uint16_t p_thumb_px = 12;
    const uint16_t w_thumb_px = config.ls_image_col_thumb * col_px - 2 * config.ls_image_pad_thumb;
    const uint16_t h_thumb_px = config.ls_image_row_thumb * row_px - 2 * config.ls_image_pad_thumb;

    // Compute the number of images per row.
    const uint16_t n_image_on_a_row = w_px / (w_thumb_px + 2 * p_thumb_px) - 1;

    std::vector<std::string> list_path_img;

    //
    for (const std::string& path_img : get_all_image_files(dirpath))
    {
        list_path_img.push_back(path_img);

        if (list_path_img.size() == n_image_on_a_row)
        {
            print_images(list_path_img, w_thumb_px, h_thumb_px, config.ls_image_pad_thumb);
            print_labels(list_path_img, config.ls_image_col_thumb, config.ls_image_row_iname);

            list_path_img.clear();
        }
    }

    if (list_path_img.size() > 0)
    {
        print_images(list_path_img, w_thumb_px, h_thumb_px, config.ls_image_pad_thumb);
        print_labels(list_path_img, config.ls_image_col_thumb, config.ls_image_row_iname);
    }

    return 0;

}   // }}}

static int32_t
ls_image_main_file(const PathX& path_img) noexcept
// [Abstract]
//
// [Args]
//
// [Returns]
//   (int32_t): Return code (0 means success, the others mean errors).
//
{   // {{{

    //
    const std::string command = "img2sixel " + path_img.string();

    //
    return std::system(command.c_str());;

}   // }}}

static int32_t
ls_image_main(const char* path_str) noexcept
// [Abstract]
//   Main function of ls_image.
//
// [Args]
//   path_str (const char*): Target path.
//
// [Returns]
//   (int32_t): Return code (0 means success, the others mean errors).
//
{   // {{{

    // Convert the path string to PathX instance.
    PathX path = PathX(path_str);

    // Call "ls_image_main_dir" if the target path is a directory.
    if (std::filesystem::is_directory(path))
        return ls_image_main_dir(path);

    // Call "ls_image_main_file" if the target path is a regular file.
    if (std::filesystem::is_regular_file(path))
        return ls_image_main_file(path);

    // Otherwise, print error message and returns error code.
    std::cout << "NiShiKi: Error on ls_image: Unknown file type: " << path.string() << std::endl;
    return -1;

}   // }}}
    
////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
ls_image(const char* command) noexcept
{   // {{{

    // Split to tokens with dropping whitespace tokens.
    std::vector<StringX> tokens = StringX(command).tokenize();
    drop_whitespace_tokens(tokens);

    // If no argument is given, append the current directory "." as an argument.
    if (tokens.size() == 1)
        tokens.emplace_back(StringX("."));

    // Process all tokens.
    for (size_t n = 1; n < tokens.size(); ++n)
        if (ls_image_main(tokens[n].string().c_str()) != 0)
            return -1;

    return 0;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
