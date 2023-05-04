// unittest.cxx

#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>

#include "file_type.hxx"
#include "path_x.hxx"
#include "string_x.hxx"
#include "utils.hxx"

#define assert(expr) (assert_body((expr), #expr, __LINE__))

static void
assert_body(bool expr_bool, const char* expr_str, int line_no)
{
    if (expr_bool) std::cout << "\033[32mPASSED\033[0m";
    else           std::cout << "\033[31mFAILED\033[0m";

    std::cout << ": L." << line_no << ": " << expr_str << std::endl;
}

static void
print_header(const char* message)
{
    std::cout                                       << std::endl;
    std::cout << "------------------------------"   << std::endl;
    std::cout << "\033[33m" << message << "\033[0m" << std::endl;
    std::cout                                       << std::endl;
}

static void
test_CharX()
{
    print_header("Unit test for CharX class");

    assert(CharX("a").size == 1);
    assert(CharX("東").size == 3);
    assert(CharX("\033[31m").width == 0);

    assert(CharX("\033[m").ansi_color().first == -1);
    assert(CharX("\033[31m").ansi_color().first == 31);
    assert(CharX("\033[1;31m").ansi_color().second == 31);
}

static void
test_FileType()
{
    print_header("Unit test for FileType class");

    FileType filetype = FileType();

    assert(filetype.mime("./build.py") == "text/x-python");
}

static void
test_PathX()
{
    print_header("Unit test for PathX class");

    assert(PathX("~/workspace/Makefile").parent_path() == PathX("~/workspace"));
    assert(PathX("~/workspace/Makefile").parent_path() != PathX("~/workspace/"));
    assert(PathX("").parent_path() == PathX(""));

    std::vector<StringX> tokens1;
    tokens1.push_back(StringX("ls"));
    tokens1.push_back(StringX(" "));
    auto [path1, name1] = split_to_target_and_query(tokens1);
    assert(path1 == PathX(""));
    assert(name1 == "");

    std::vector<StringX> tokens2;
    tokens2.push_back(StringX("ls"));
    tokens2.push_back(StringX(" "));
    tokens2.push_back(StringX("../develop/nishiki"));
    auto [path2, name2] = split_to_target_and_query(tokens2);
    assert(path2 == PathX("../develop"));
    assert(name2 == "nishiki");

    std::vector<StringX> tokens3;
    tokens3.push_back(StringX("ls"));
    tokens3.push_back(StringX(" "));
    tokens3.push_back(StringX("../develop/nishiki/"));
    auto [path3, name3] = split_to_target_and_query(tokens3);
    assert(path3 == PathX("../develop/nishiki"));
    assert(name3 == "");
}

static void
test_StringX()
{
    print_header("Unit test for StringX class");

    assert(StringX("test string").width() == 11);
    assert(StringX("東京都江東区辰巳").width() == 16);

    assert(StringX("東京都").clip(5).width() == 4);

    assert(StringX("this is a pen") == StringX("this is a pen"));
    assert(StringX("echo") == StringX("e\033[31mc\033[27mho"));
    assert(StringX("echz") > StringX("e\033[31mc\033[27mho"));

    assert(StringX("東京都江東区辰巳").startswith(StringX("東京")) == true);
    assert(StringX("東京都江東区辰巳").startswith(StringX("東京都")) == true);

    assert(StringX("東京都江東区辰巳").string() == "東京都江東区辰巳");
}

static void
test_utils()
{
    print_header("Unit test for utils.cxx");

    std::vector<StringX> values1;
    values1.push_back(StringX("Tokyo"));
    values1.push_back(StringX("東京都"));
    values1.push_back(StringX("Tatsumi"));
    values1.push_back(StringX("江東区"));
    values1.push_back(StringX("Koto-ku"));
    values1.push_back(StringX("辰巳"));
    assert(column(values1, 20, 3, 2)[0] == StringX("Tokyo    江東区   "));

    std::vector<StringX> values2;
    values2.push_back(StringX("apple"));
    values2.push_back(StringX("april"));
    values2.push_back(StringX("api"));
    values2.push_back(StringX("apparel"));
    assert(get_common_substring(values2) == StringX("ap"));
}

int main()
{
    test_CharX();
    test_FileType();
    test_StringX();
    test_PathX();
    test_utils();
}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
