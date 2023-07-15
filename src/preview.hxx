////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: preview.cxx                                                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PREVIEW_HXX
#define PREVIEW_HXX

#include "string_x.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<StringX>
preview(const std::string& path, uint16_t width, uint16_t height)
noexcept;
// [Abstract]
//   Returns preview contents of the given file.
//
// [Args]
//   path   (const std::string&): [IN] Path to the target file.
//   width  (int16_t)           : [IN] Width of preview window.
//   height (int16_t)           : [IN] Height of preview window.
//
// [Returns]
//   (std::vector<std::string>): Lines of preview contents.


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
