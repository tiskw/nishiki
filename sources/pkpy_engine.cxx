////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: pkpy_engine.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "pkpy_engine.hxx"

// Include the headers of STL.
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <sys/ioctl.h>

// Include the headers of custom modules.
#include "text_buffer.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
pkpy_os_getenv(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "os.getenv" in PocketPy.
//
// [Args]
//   argc (const int32_t): The number of arguments.
//   argv (py_Ref)       : The argument values.
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check the arguments passed from PocketPy.
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    // Get the arguments.
    const char* env_key = py_tostr(py_arg(0));

    // Returns the value of environment variable.
    py_newstr(py_retval(), getenv(env_key));
    return true;

}   // }}}

static bool
pkpy_os_putenv(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "os.putenv" in PocketPy.
//
// [Args]
//   argc (const int32_t): The number of arguments.
//   argv (py_Ref)       : The argument values.
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check the arguments passed from PocketPy.
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_str);

    // Get the arguments.
    const char* key = py_tostr(py_arg(0));
    const char* val = py_tostr(py_arg(1));

    // Returns the value of environment variable.
    py_newint(py_retval(), setenv(key, val, 1));
    return true;

}   // }}}

static bool
pkpy_re_sub(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "re.sub" in PocketPy.
//
// [Args]
//   argc (const int32_t): The number of arguments.
//   argv (py_Ref)       : The argument values.
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check the arguments passed from PocketPy.
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_str);

    // Get the arguments.
    const char* pattern = py_tostr(py_arg(0));
    const char* repl    = py_tostr(py_arg(1));
    const char* target  = py_tostr(py_arg(2));

    // Replace strings using regular expression.
    std::string ret = std::regex_replace(target, std::regex(pattern), repl);

    // Returns the value of environment variable.
    py_newstr(py_retval(), ret.c_str());
    return true;

}   // }}}

static bool
pkpy_nishiki_check_output(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "os.check_output" in PocketPy. The "os.check_output" is
//   similar to "os.system", but returns the contents of STDOUT as a string.
//
// [Args]
//   argc (const int32_t): The number of arguments.
//   argv (py_Ref)       : The argument values.
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check the arguments passed from PocketPy.
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    // Get the arguments.
    const char* command = py_tostr(py_arg(0));

    // Initialize output variable.
    std::string result;

    // Make a command string that will be called actually.
    const std::string command_called = std::string(command) + " 2> /dev/null";

    // Open pipe and run the command.
    FILE *pipe = popen(command_called.c_str(), "r");

    // Return an empty string if failed to open the pipe.
    if (pipe == NULL) return false;

    // Get the returned value of the command.
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    // Close the pipe.
    pclose(pipe);

    // Returns the output value.
    py_newstr(py_retval(), result.c_str());
    return true;

}   // }}}

static bool
pkpy_nishiki_get(const int32_t argc, py_Ref argv)
// [Abstract]
//
// [Args]
//   argc (const int32_t): The number of arguments.
//   argv (py_Ref)       : The argument values.
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check the arguments passed from PocketPy.
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    // Create window size instance to get the terminal size.
    struct winsize ws;

    // Get the current editing mode.
    TextBuffer::Mode mode = (TextBuffer::instance != nullptr) ? TextBuffer::instance->get_mode() : TextBuffer::Mode::NORMAL;

    // Set returned value.
    switch (hash(py_tostr(py_arg(0))))
    {
        case hash("height"): 
            ioctl(0, TIOCGWINSZ, &ws);
            py_newint(py_retval(), ws.ws_row);
            break;

        case hash("width"): 
            ioctl(0, TIOCGWINSZ, &ws);
            py_newint(py_retval(), ws.ws_col);
            break;

        case hash("mode"): 
            py_newstr(py_retval(), (mode == TextBuffer::Mode::NORMAL) ? "N" : "I");
            break;

        default:
            py_newnone(py_retval());
    }

    // Returns the output value.
    return true;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PkPyEngine: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

PkPyEngine::PkPyEngine(void)
{   // {{{

    // Initialize PocketPy.
    py_initialize();

    // Append several functions to the "os" module in PocketPy.
    py_GlobalRef mod_os = py_getmodule("os");
    py_bind(mod_os, "getenv(key)", pkpy_os_getenv);
    py_bind(mod_os, "putenv(key, value)", pkpy_os_putenv);

    // Newly create "re" module in PocketPy.
    py_GlobalRef mod_re = py_newmodule("re");
    py_bind(mod_re, "sub(pattern, repl, string)", pkpy_re_sub);

    // Newly create "nishiki" module in PocketPy.
    py_GlobalRef mod_nishiki = py_newmodule("nishiki");
    py_bind(mod_nishiki, "check_output(command)", pkpy_nishiki_check_output);
    py_bind(mod_nishiki, "get(key)", pkpy_nishiki_get);

    // Compute candidates of config file path.
    std::filesystem::path home  = std::filesystem::path(getenv("HOME"));
    std::filesystem::path path1 = home / ".config" / "nishiki" / "config.py";
    std::filesystem::path path2 = std::filesystem::path(".") / "config.py";

    // Returns path1 if exists, otherwise returns path2.
    std::filesystem::path path_config = std::filesystem::exists(path1) ? path1 : path2;

    // Load the config file.
    this->load_script(std::filesystem::canonical(path_config).string().c_str());

}   // }}}

PkPyEngine::~PkPyEngine(void)
{   // {{{

    // Finalize PocketPy.
    py_finalize();

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PkPyEngine: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
PkPyEngine::get_prompt(int16_t index) const noexcept
{   // {{{

    // Evaluate the prompt function and capture the returned value.
    std::stringstream ss;
    ss << "get_ps" << index << "()";
    bool is_success = this->exec(ss.str().c_str(), EVAL_MODE);

    // Returns default value if failed.
    if (not is_success)
    {
        if (index == 0) return "";
        if (index == 1) return ">> ";
        if (index == 2) return ".. ";
        else            return "";
    }

    // Returns the captured value.
    return py_tostr(py_retval());

}   // }}}

void
PkPyEngine::setup_config(NishikiConfig* config) const noexcept
{   // {{{

    // Get local variable names defainde in the config file.
    std::vector<std::string> py_local_variables;
    if (this->exec("[key for key,_ in locals().items() if not key.startswith('_')]", EVAL_MODE))
        for (int32_t i = 0; i < py_list_len(py_retval()); ++i)
            py_local_variables.push_back(py_tostr(py_list_getitem(py_retval(), i)));

    for (const std::string& target : py_local_variables)
    {
        // Capture the value of the target variable.
        if (not this->exec(target.c_str(), EVAL_MODE))
            continue;

        // Do nothing if the captured value is None.
        if (py_isnone(py_retval()) or py_isnil(py_retval()))
            continue;

        switch (hash(target.c_str()))
        {
            // General settings.
            case hash("area_height")  : config->area_height   = py_toint(py_retval()); break;
            case hash("column_margin"): config->column_margin = py_toint(py_retval()); break;
            case hash("datetime_pre") : config->datetime_pre  = py_tostr(py_retval()); break;
            case hash("datetime_post"): config->datetime_post = py_tostr(py_retval()); break;
            case hash("histhint_pre") : config->histhint_pre  = py_tostr(py_retval()); break;
            case hash("histhint_post"): config->histhint_post = py_tostr(py_retval()); break;

            // Colorization settings.
            case hash("colorize_commands"): config->colorize_commands = py_tostr(py_retval()); break;
            case hash("colorize_keywords"): config->colorize_keywords = py_tostr(py_retval()); break;
            case hash("colorize_symbols") : config->colorize_symbols  = py_tostr(py_retval()); break;

            // Preview settings.
            case hash("preview_delim"): config->preview_delim = py_tostr(py_retval());   break;
            case hash("preview_ratio"): config->preview_ratio = py_tofloat(py_retval()); break;

            // Alias / Keybind / Preview settings.
            case hash("aliases"):
            case hash("keybinds"):
            case hash("previews"):

                for (int32_t i = 0; i < py_list_len(py_retval()); ++i)
                {
                    // Get list instance.
                    py_Ref item = py_list_getitem(py_retval(), i);

                    // Get items in the list.
                    const std::string key = py_tostr(py_list_getitem(item, 0));
                    const std::string val = py_tostr(py_list_getitem(item, 1));

                    // Append to the map.
                    switch (hash(target.c_str()))
                    {
                        case hash("aliases") : config->aliases[key] = val; break;
                        case hash("keybinds"): config->keybind[key] = val; break;
                        case hash("previews"): config->preview[key] = val; break;
                    }
                }
                break;

            // Compresion settings.
            case hash("completions"):

                for (int32_t i = 0; i < py_list_len(py_retval()); ++i)
                {
                    // Get list instance.
                    py_Ref ref_comp = py_list_getitem(py_retval(), i);

                    // Get the pattern (= list of strings).
                    py_Ref ref_pattern = py_list_getitem(ref_comp, 0);

                    // Get the contents of the pattern list.
                    std::vector<std::string> pattern;
                    for (int32_t j = 0; j < py_list_len(ref_pattern); ++j)
                        pattern.push_back(py_tostr(py_list_getitem(ref_pattern, j)));

                    // Get the completion type.
                    EditHelper::CompType ctype;
                    switch (hash(py_tostr(py_list_getitem(ref_comp, 1))))
                    {
                        case hash("command"): ctype = EditHelper::CompType::COMMAND; break;
                        case hash("option") : ctype = EditHelper::CompType::OPTION;  break;
                        case hash("preview"): ctype = EditHelper::CompType::PREVIEW; break;
                        case hash("shell")  : ctype = EditHelper::CompType::SHELL;   break;
                        case hash("subcmd") : ctype = EditHelper::CompType::SUBCMD;  break;
                        default             : ctype = EditHelper::CompType::PATH;    break;
                    }

                    // Get the optional string.
                    const std::string opt_str = py_tostr(py_list_getitem(ref_comp, 2));

                    // Append to the completion information.
                    config->completions.emplace_back(pattern, ctype, opt_str);
                }
                break;
        }
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PkPyEngine: Private member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PkPyEngine::exec(const char* s, py_CompileMode mode) const noexcept
{   // {{{

    // Run the specified string as a Python script using PocketPy.
    bool is_success = py_exec(s, "<string>", mode, nullptr);

    // Print exception if raised in the PocketPy.
    if (not is_success)
        py_printexc();

    return is_success;

}   // }}}

bool
PkPyEngine::load_script(const char* path) const noexcept
{   // {{{

    // Open the file.
    std::ifstream ifs(path);

    // Read the contents of the file.
    std::string text; 
    for (std::string line; getline(ifs, line); line.clear())
        text += line + '\n';

    // Execute the file contents as a Python script using PocketPy.
    return this->exec(text.c_str(), EXEC_MODE);

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
