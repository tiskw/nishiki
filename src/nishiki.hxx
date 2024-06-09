////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: nishiki.hxx                                                                ///
///                                                                                              ///
/// Entry point of NiShiKi.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NISHIKI_HXX
#define NISHIKI_HXX

// Include the headers of STL.
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////////////
// NiShiKi main function.
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
nishiki_main(const int32_t argc, const char* argv[]);
// [Abstract]
//   Entry point of NiShiKi.
//
// [Args]
//   argc (const int32_t): The number of strings pointed to by argv.
//   argv (const char*)  : The strings of command line arguments.
//
// [Returns]
//   (int32_t): Return code (EXIT_SUCCESS or EXIT_FAILURE).

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
