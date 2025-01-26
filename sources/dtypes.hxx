////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: dtypes.hxx                                                                  ///
///                                                                                              ///
/// Define type aliases.                                                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DTYPES_HXX
#define DTYPES_HXX

// Include the headers of STL.
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

// Map type.
template <class T, class U>
using Map = std::map<T, U>;

// Pair type.
template <class T, class U>
using Pair = std::pair<T, U>;

// String type.
using String = std::string;

// Tuple type.
template <class T, class U, class V>
using Tuple = std::tuple<T, U, V>;

// Vector type.
template <class T>
using Vector = std::vector<T>;

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
