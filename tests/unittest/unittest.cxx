////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: command_runner.cxx                                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the headers of STL.
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

// Include the headers of custom modules.
#include "file_type.hxx"
#include "path_x.hxx"
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

static void
assert_body(bool expr_bool, const char* expr_str, int line_no)
{   // {{{

    // Print test result.
    if (expr_bool) std::cout << "\033[32mPASSED\033[0m";
    else           std::cout << "\033[31mFAILED\033[0m";

    // Print detailed information.
    std::cout << ": L." << line_no << ": " << expr_str << std::endl;

    // Update the passed/failed flag.
    passed &= expr_bool;

}   // }}}

static void
print_header(const char* message)
{   // {{{

    std::cout                                       << std::endl;
    std::cout << "------------------------------"   << std::endl;
    std::cout << "\033[33m" << message << "\033[0m" << std::endl;
    std::cout                                       << std::endl;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void
test_CharX()
{   // {{{

    // Print header.
    print_header("Unit test for CharX class");

    // Test 1: character binary size.
    assert(CharX("a").size == 1);
    assert(CharX("東").size == 3);
    assert(CharX("\033[31m").size == 5);

    // Test 2: character width.
    assert(CharX("a").width == 1);
    assert(CharX("東").width == 2);
    assert(CharX("\033[31m").width == 0);

    // Test 3: ANSI color code parsing.
    assert(CharX("\033[m").ansi_color().first == -1);
    assert(CharX("\033[31m").ansi_color().first == 31);
    assert(CharX("\033[1;31m").ansi_color().second == 31);

}   // }}}

static void
test_FileType()
{   // {{{

    // Print header.
    print_header("Unit test for FileType class");

    // Instanciate the FileType class.
    FileType filetype = FileType();

    // Test 1: mime type.
    assert(filetype.mime("./build.py") == "text/x-python");
    assert(filetype.mime("get_url.sh") == "application/x-shellscript");

}   // }}}

static void
test_PathX()
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

}   // }}}

static void
test_StringX()
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
    assert(StringX("echo") == StringX("e\033[31mc\033[27mho"));
    assert(StringX("echa") < StringX("e\033[31mc\033[27mho"));

    // Test 4: startswith.
    assert(StringX("東京都江東区辰巳").startswith(StringX("東京")) == true);
    assert(StringX("東京都江東区辰巳").startswith(StringX("東京都")) == true);

    // Test 5: to string.
    assert(StringX("東京都江東区辰巳").string() == "東京都江東区辰巳");

    // Test6: tokenize.
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

}   // }}}

static void
test_utils()
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

void
unittest()
{   // {{{

    // Call test functions.
    test_CharX();
    test_FileType();
    test_PathX();
    test_StringX();
    test_utils();

    // Print header of overall result.
    std::cout                                         << std::endl;
    std::cout << "=============================="     << std::endl;
    std::cout << "\033[33mOVERALL TEST RESULT\033[0m" << std::endl;
    std::cout                                         << std::endl;

    // Print test result.
    if (passed) { std::cout << "\033[32mPASSED\033[0m" << std::endl; }
    else        { std::cout << "\033[31mFAILED\033[0m" << std::endl; }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
