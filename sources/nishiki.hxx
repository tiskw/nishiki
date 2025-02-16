////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: nishiki.hxx                                                                ///
///                                                                                              ///
/// Entry point of NiShiKi.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NISHIKI_HXX
#define NISHIKI_HXX

// Define version information.
#define VERSION "2025.02.16"

// Include the headers of STL.
#include <cstdint>
#include <map>
#include <string>

// Include the headers of custom modules.
#include "command_runner.hxx"
#include "history_manager.hxx"
#include "pkpy_engine.hxx"
#include "string_x.hxx"
#include "text_buffer.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// NiShiKi main function.
////////////////////////////////////////////////////////////////////////////////////////////////////

StringX
readline(const HistoryManager& hist, const CommandRunner& runner, const PkPyEngine& py_engine, const char* user_inputs = nullptr) noexcept;
// [Abstract]
//   Read user input with rich interface.
//
// [Args]
//   hist       (const HistoryManager&): History manager.
//   runner     (const CommandRunner&) : Command runner.
//   py_engine  (const PkPyEngine&)    : PocketPy engine.
//   user_input (const char*)          : User input (mainly used for uinttest).
//
// [Returns]
//   (std::map<std::string, std::string>): Parsed command line arguments.

int32_t
nishiki_main(const std::map<std::string, std::string>& args) noexcept;
// [Abstract]
//   Main function of NiSHiKi. This function will be called in the "main" function (written in
//   main.cxx). This structure will not be changed because the test code rely on the structure.
//
// [Args]
//   argc (const int32_t): The number of strings pointed to by argv.
//   argv (const char*[]): Contents of command line arguments.
//   envp (const char*[]): Pointer to environment variables.
//
// [Returns]
//   (int32_t): Return code (EXIT_SUCCESS or EXIT_FAILURE).

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
