////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: pkpy_engine.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "pkpy_engine.hxx"

// Include the headers of STL.
#include <filesystem>
#include <fstream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
pkpy_os_check_output(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "os.check_output" in PocketPy. The "os.check_output" is
//   similar to "os.system", but returns the contents of STDOUT as a string.
//
// [Args]
//   argc (const int32_t): 
//   argv (py_Ref)       : 
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check arguments and get the target command string.
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
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
pkpy_os_getenv(const int32_t argc, py_Ref argv)
// [Abstract]
//   C-implementation of the function "os.getenv" in PocketPy.
//
// [Args]
//   argc (const int32_t): 
//   argv (py_Ref)       : 
//
// [Returns]
//   (bool): True if success.
//
{   // {{{

    // Check arguments and get the key of environment variable.
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* env_key = py_tostr(py_arg(0));

    // Returns the value of environment variable.
    py_newstr(py_retval(), getenv(env_key));
    return true;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PkPyEngine: Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////

PkPyEngine::PkPyEngine(void)
{   // {{{

    constexpr auto get_config_path = []() -> std::filesystem::path
    // [Abstract]
    //
    // [Args]
    //
    // [Returns]
    //   (std::string): 
    {
        // Compute candidates of config file path.
        std::filesystem::path home  = std::filesystem::path(getenv("HOME"));
        std::filesystem::path path1 = home / ".config" / "nishiki" / "config.py";
        std::filesystem::path path2 = std::filesystem::path(".") / "config.py";

        // Returns path1 if exists, otherwise returns path2.
        return std::filesystem::exists(path1) ? path1 : path2;
    };

    //
    py_initialize();

    // Append os.check_output function to PocketPy.
    py_GlobalRef mod = py_getmodule("os");
    py_bind(mod, "check_output(command)", pkpy_os_check_output);
    py_bind(mod, "getenv(key)", pkpy_os_getenv);

    //
    std::string path_config = std::filesystem::canonical(get_config_path()).string();

    //
    this->load_script(path_config.c_str());

}   // }}}

PkPyEngine::~PkPyEngine(void)
{   // {{{

    py_finalize();

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PkPyEngine: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PkPyEngine::exec(const char* s, py_CompileMode mode) const noexcept
{   // {{{

    //
    bool is_success = py_exec(s, "<string>", mode, nullptr);

    //
    if (not is_success)
        py_printexc();

    return is_success;

}   // }}}

bool
PkPyEngine::load_script(const char* path) const noexcept
{   // {{{

    std::ifstream file(path);
    std::string text, line; 
    while (std::getline(file, line))
        text += line + '\n';

    return this->exec(text.c_str(), EXEC_MODE);

}   // }}}

std::string
PkPyEngine::get_prompt(int16_t index) const noexcept
{   // {{{

    //
    std::stringstream func_name;
    func_name << "get_ps" << index << "()";

    //
    bool is_success = this->exec(func_name.str().c_str(), EVAL_MODE);

    //
    return is_success ? py_tostr(py_retval()) : "";

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
