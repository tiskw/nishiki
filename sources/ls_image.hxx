////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: ls_image.hxx                                                                ///
///                                                                                              ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LS_IMAGE_HXX
#define LS_IMAGE_HXX

// Include the headers of STL.
#include <cstdint>

int32_t
ls_image(const char* path) noexcept;
// [Abstract]
//   List image files to STDOUT.
//
// [Args]
//   path (const char*): Target path.
//
// [Returns]
//   (int32_t): Returns zero on success.

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
