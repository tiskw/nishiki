////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: test_nishiki.cxx                                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the headers of STL.
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Include the headers of custom modules.
#include "file_type.hxx"
#include "path_x.hxx"
#include "preview.hxx"
#include "read_cmd.hxx"
#include "string_x.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////////////////

bool passed = true;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility macros and functions for testing
////////////////////////////////////////////////////////////////////////////////////////////////////

#define assert(expr) (assert_body((expr), #expr, __LINE__))

static void assert_body(bool expr_bool, const char* expr_str, int line_no)
{   // {{{

    // Print test result.
    if (expr_bool) std::cout << "\033[32mPASSED\033[0m";
    else           std::cout << "\033[31mFAILED\033[0m";

    // Print detailed information.
    std::cout << ": L." << line_no << ": " << expr_str << std::endl;

    // Update the passed/failed flag.
    passed &= expr_bool;

}   // }}}

static void print_header(const char* message)
{   // {{{

    std::cout                                       << std::endl;
    std::cout << "------------------------------"   << std::endl;
    std::cout << "\033[33m" << message << "\033[0m" << std::endl;
    std::cout                                       << std::endl;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Unittest functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_CharX()
{   // {{{

    // Print header.
    print_header("Unit test for CharX class");

    // Test 1: character binary size.
    assert(CharX("a").size == 1);
    assert(CharX("東").size == 3);

    // Test 2: character width.
    assert(CharX("a").width == 1);
    assert(CharX("東").width == 2);

    // Test 3: ANSI color code parsing.
    assert(CharX("\x1B[m").value == 0x6D5B1B);
    assert(CharX("\x1B[m").size == 3);
    assert(CharX("\x1B[m").width == 0);
    assert(CharX("\x1B[0m").value == 0x6D005B1B);
    assert(CharX("\x1B[0m").size == 4);
    assert(CharX("\x1B[0m").width == 0);
    assert(CharX("\x1B[31m").value == 0x6D1F5B1B);
    assert(CharX("\x1B[31m").size == 4);
    assert(CharX("\x1B[31m").width == 0);
    assert(CharX("\x1B[38;2;12;34;56m").value == 0x6D38220C02265B1B);
    assert(CharX("\x1B[38;2;12;34;56m").width == 0);

    // Test 4: Print escape sequence.
    assert(CharX("\x1B[38;2;197;200;198m").string() == "\x1B[38;2;197;200;198m");
    assert(CharX("\x1B[0m").string() == "\x1B[0m");
    assert(CharX("\x1B[m").string() == "\x1B[m");

    // Test 6: Escape sequence other than CSI.
    assert(CharX("\x1B").value == 0x1B);
    assert(CharX("\x1B").size == 1);
    assert(CharX("\x1B\x5B\x41").value == CHARX_VALUE_KEY_UP);
    assert(CharX("\x1B\x5B\x42").value == CHARX_VALUE_KEY_DOWN);
    assert(CharX("\x1B\x5B\x43").value == CHARX_VALUE_KEY_RIGHT);
    assert(CharX("\x1B\x5B\x44").value == CHARX_VALUE_KEY_LEFT);

}   // }}}

static void test_FileType()
{   // {{{

    // Print header.
    print_header("Unit test for FileType class");

    // Instanciate the FileType class.
    FileType filetype = FileType();

    // Test 1: mime type for files.
    assert(filetype.mime("./build.py") == "text/x-python");
    assert(filetype.mime("get_url.sh") == "application/x-shellscript");

    // Test 2: mime type for directories.
    assert(filetype.mime("/home") == "inode/directory");

}   // }}}

static void test_PathX()
{   // {{{

    // Print header.
    print_header("Unit test for PathX class");

    // Test 1: 
    assert(PathX("~/workspace/Makefile").parent_path() == PathX("~/workspace"));
    assert(PathX("~/workspace/Makefile").parent_path() != PathX("~/workspace/"));
    assert(PathX("").parent_path() == PathX(""));

    // Test 2: 
    std::vector<StringX> tokens1;
    tokens1.push_back(StringX("ls"));
    tokens1.push_back(StringX(" "));
    auto [path1, name1] = split_to_target_and_query(tokens1);
    assert(path1 == PathX(""));
    assert(name1 == "");

    // Test 3: 
    std::vector<StringX> tokens2;
    tokens2.push_back(StringX("ls"));
    tokens2.push_back(StringX(" "));
    tokens2.push_back(StringX("../develop/nishiki"));
    auto [path2, name2] = split_to_target_and_query(tokens2);
    assert(path2 == PathX("../develop"));
    assert(name2 == "nishiki");

    // Test 4: 
    std::vector<StringX> tokens3;
    tokens3.push_back(StringX("ls"));
    tokens3.push_back(StringX(" "));
    tokens3.push_back(StringX("../develop/nishiki/"));
    auto [path3, name3] = split_to_target_and_query(tokens3);
    assert(path3 == PathX("../develop/nishiki"));
    assert(name3 == "");

    // Test 5: listdir.
    assert(PathX("/not_exists").listdir().size() == 0);
    assert(PathX(".").listdir(1).size() == 1);

}   // }}}

static void test_preview()
{   // {{{

    // Print header.
    print_header("Unit test for preview function");

    // Test 1: preview unexisting file.
    assert(preview("/unexisting_file", 100, 6).size() == 0);

}   // }}}

static void test_StringX()
{   // {{{

    // Print header.
    print_header("Unit test for StringX class");

    // Test 1: character width.
    assert(StringX("test string").width() == 11);
    assert(StringX("東京都江東区辰巳").width() == 16);

    // Test 2: clipping.
    assert(StringX("東京都").clip(5).width() == 4);

    // Test 3: comparison operators.
    assert(StringX("this is a pen") == StringX("this is a pen"));
    assert(StringX("this is a pen") < StringX("this is the pen"));
    assert(StringX("echo") == StringX("e\x1B[31mc\x1B[27mho"));
    assert(StringX("echa") < StringX("e\x1B[31mc\x1B[27mho"));

    // Test 4: startswith.
    assert(StringX("東京都江東区辰巳").startswith(StringX("東京")) == true);
    assert(StringX("東京都江東区辰巳").startswith(StringX("東京都")) == true);

    // Test 5: to string.
    assert(StringX("東京都江東区辰巳").string() == "東京都江東区辰巳");

    // Test 6: tokenize.
    std::vector<StringX> tokens = StringX("echo 'this is a pen' | grep 'pen'").tokenize();
    assert(tokens[0] == StringX("echo"));
    assert(tokens[1] == StringX(" "));
    assert(tokens[2] == StringX("'this is a pen'"));
    assert(tokens[3] == StringX(" "));
    assert(tokens[4] == StringX("|"));
    assert(tokens[5] == StringX(" "));
    assert(tokens[6] == StringX("grep"));
    assert(tokens[7] == StringX(" "));
    assert(tokens[8] == StringX("'pen'"));

    // Test 7: chunking.
    std::vector<StringX> chunks = StringX("東京都江東区辰巳").chunk(6);
    assert(chunks[0] == StringX("東京都"));
    assert(chunks[1] == StringX("江東区"));
    assert(chunks[2] == StringX("辰巳"));

    // Test 8: consert ANSI escape sequence to string.
    assert(StringX("\x1B[7m \x1B[0m").string().size() == 9);
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[0] == '\x1B');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[1] == '[');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[2] == '7');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[3] == 'm');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[4] == ' ');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[5] == '\x1B');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[6] == '[');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[7] == '0');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[8] == 'm');
    assert(StringX("\x1B[7m \x1B[0m").string().c_str()[9] == '\0');

}   // }}}

static void test_utils()
{   // {{{

    // Print header.
    print_header("Unit test for utils.cxx");

    // Test 1: column function.
    std::vector<StringX> values1;
    values1.push_back(StringX("Tokyo"));
    values1.push_back(StringX("東京都"));
    values1.push_back(StringX("Tatsumi"));
    values1.push_back(StringX("江東区"));
    values1.push_back(StringX("Koto-ku"));
    values1.push_back(StringX("辰巳"));
    assert(column(values1, 20, 3, 2)[0] == StringX("Tokyo    江東区   "));

    // Test 2: get_common_substring function.
    std::vector<StringX> values2;
    std::vector<StringX> values3;
    std::vector<StringX> values4;
    values2.push_back(StringX("apple"));
    values2.push_back(StringX("april"));
    values2.push_back(StringX("api"));
    values2.push_back(StringX("apparel"));
    values4.push_back(StringX(""));
    values4.push_back(StringX(""));
    assert(get_common_substring(values2) == StringX("ap"));
    assert(get_common_substring(values3) == StringX(""));
    assert(get_common_substring(values4) == StringX(""));

    // Test 3: split function.
    assert(split("this,is,csv", ",").size() == 3);
    assert(split("this is csv", "").size() == 1);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Integration test functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_readcmd()
{   // {{{

    constexpr auto run_test_readcmd = [](const char* input_str, const char* output_lhs, const char* output_rhs) noexcept
    // [Abstract]
    //   Run readcmd for testing purpose.
    //
    // [Args]
    //   input      (const char*): Input command string.
    //   output_lhs (const char*): Corrent lhs string after the command.
    //   output_rhs (const char*): Corrent rhs string after the commahd.
    //
    // [Returns]
    {
        // Config values of readcmd.
        const StringX        lhs_ini       = StringX("");
        const StringX        rhs_ini       = StringX("");
        const Deque<StringX> hists         = {StringX("previous input")};
        const int            area_height   = 8;
        const String         ps1i          = ">> ";
        const String         ps1n          = "<< ";
        const String         ps2           = ".. ";
        const char*          histhint_pre  = "";
        const char*          histhint_post = "";

        // Prepare input string.
        StringX input = StringX(input_str);

        const auto [lhs, rhs] = readcmd(lhs_ini, rhs_ini, hists, area_height, ps1i,
                                        ps1n, ps2, histhint_pre, histhint_post, input);

        const bool flag_lhs = (lhs.string() == String(output_lhs));
        const bool flag_rhs = (rhs.string() == String(output_rhs));
        return (flag_lhs and flag_rhs);
    };

    // Print header.
    print_header("Unit test for readcmd function");

    // Command completions.
    assert(run_test_readcmd("ls -l\n", "ls -l", ""));
    assert(run_test_readcmd("ls ~/\n", "ls ~/", ""));
    assert(run_test_readcmd("ls Makefile \n", "ls Makefile ", ""));
    assert(run_test_readcmd("git bra\t\n", "git branch ", ""));
    assert(run_test_readcmd("ls ./objects\t\n", "ls ./objects/", ""));
    assert(run_test_readcmd("ls ../source/file_\t\n", "ls ../source/file_type.", ""));

    // Edit characters in edit mode.
    assert(run_test_readcmd("ls \x1B\x1A 0a\x08\n", "", "s "));
    assert(run_test_readcmd("ls \x1B\x1A 0A\x7f\n", "ls", ""));
    assert(run_test_readcmd("ls \x1B\x1A i\x7f\n", "ls", ""));
    assert(run_test_readcmd("ls \x1B\x1A I\x7f\n", "", "ls "));
    assert(run_test_readcmd("ls \x1B\x1A hhhxx\n", "", " "));
    assert(run_test_readcmd("ls \x1B\x1A hhhllx\n", "ls", ""));
    assert(run_test_readcmd("ls \x1B\x1A hhD\n", "l", ""));
    assert(run_test_readcmd("ls \x1B\x1A Sa\n", "a", ""));

    // Move cursor.
    assert(run_test_readcmd("ls \x1B\x5B\x44\x1B\x5B\x43\n", "ls ", ""));
    assert(run_test_readcmd("ls \x1B\x1A \x1B\x5B\x44\x1B\x5B\x43\n", "ls ", ""));
    assert(run_test_readcmd("ls \x1B\x1A 0$\n", "ls ", ""));

    // History callback.
    assert(run_test_readcmd("ls \x1B\x1A kj\n", "ls ", ""));
    assert(run_test_readcmd("ls \x1B\x1A k\n", "previous input", ""));
    assert(run_test_readcmd("ls \x1B\x1A \x1b\x5b\x41\x1b\x5b\x42\n", "ls ", ""));

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t main(void)
{   // {{{

    // Run all unittest functions.
    test_CharX();
    test_FileType();
    test_PathX();
    test_preview();
    test_StringX();
    test_utils();

    // Run all integration test functions.
    test_readcmd();

    // Print header of overall result.
    std::cout                                         << std::endl;
    std::cout << "=============================="     << std::endl;
    std::cout << "\033[33mOVERALL TEST RESULT\033[0m" << std::endl;
    std::cout                                         << std::endl;

    // Print test result.
    if (passed) { std::cout << "\033[32mPASSED!!\033[0m" << std::endl; }
    else        { std::cout << "\033[31mFAILED!!\033[0m" << std::endl; }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
