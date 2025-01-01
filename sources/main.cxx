////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: main.cxx                                                                    ///
///                                                                                              ///
/// Entry point of NiShiKi.                                                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the headers of custom modules.
#include "nishiki.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
main(const int32_t argc, const char* argv[])
// [Abstract]
//   Entry point of NiSHiKi. This function just call "nishiki_main" function (written in 
//   nishiki.cxx). This structure will not be changed because the test code rely on the structure.
//
// [Args]
//   argc (const int32_t): Number of command line arguments.
//   argv (const char*[]): Contents of command line arguments.
//
// [Returns]
//   (int32_t): EXIT_SUCCESS or EXIT_FAILURE.
//
{   // {{{

    return nishiki_main(argc, argv);

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
