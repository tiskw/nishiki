////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: readline.hxx                                                                ///
///                                                                                              ///
/// Read user input with rich completion functions.                                              ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef READCMD_HXX
#define READCMD_HXX

// Include the headers of custom modules.
#include "dtypes.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Published functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Tuple<StringX, StringX>
readcmd(const StringX& lhs_ini, const StringX& rhs_ini, const Deque<StringX>& hists,
        uint8_t area_height, const String& ps1i, const String& ps1n, const String& ps2,
        const char* histhint_pre, const char* histhint_post, StringX& input_str);
// [Abstract]
//   Read user input with rich interface.
//
// [Args]
//   lhs_ini       (const StringX&)       : Initial value of the left hand side string.
//   rhs_ini       (const StringX&)       : Initial value of the right hand side string.
//   hists         (const Deque<StringX>&): History strings.
//   area_height   (uint8_t)              : 
//   ps1i          (const char*)          : 
//   ps1n          (const char*)          : 
//   ps2           (const char*)          : 
//   histhint_pre  (const char*)          : 
//   histhint_post (const char*)          : 
//   input_str     (const char*)          : 
//
// [Returns]
//   (Tuple<StringX, StringX>): Returned pair of left and right hand side.

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
