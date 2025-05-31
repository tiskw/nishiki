////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: parse_args.hxx                                                              ///
///                                                                                              ///
/// Parse command line arguments.                                                                ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_TYPE_HXX
#define FILE_TYPE_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Map<String, String> parse_args(int32_t argc, char* const argv[], const char* version);
// [Abstract]
//   Parse command line arguments.
//
// [Args]
//   argc    (int32_t)    : Number of strings pointed to by argv.
//   argv    (const char*): Strings of command line arguments.
//   version (const char*): Version string.
//
// [Returns]
//   (Map<String, String>): Parsed command line arguments.

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
