////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: preview.hxx                                                                 ///
///                                                                                              ///
/// This file defines the function `preview` that returns preview result as a vector of string.  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PREVIEW_HXX
#define PREVIEW_HXX

// Include the headers of STL.
#include <cstdint>

// Include the headers of custom modules.
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<StringX> preview(const String& path, uint16_t width, uint16_t height) noexcept;
// [Abstract]
//   Returns preview contents of the given file.
//
// [Args]
//   path   (const String&): [IN] Path to the target file.
//   width  (int16_t)      : [IN] Width of preview window.
//   height (int16_t)      : [IN] Height of preview window.
//
// [Returns]
//   (Vector<String>): Lines of preview contents.

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
