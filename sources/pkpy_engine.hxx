////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: pkpy_engine.hxx                                                             ///
///                                                                                              ///
/// This file provides a wrapper class for PocketPy <https://github.com/pocketpy/pocketpy>.      ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PKPY_ENGINE_HXX
#define PKPY_ENGINE_HXX

// Include the headers of STL.
#include <cstdint>
#include <string>

// Include PockerPy header.
#include <pocketpy.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class PkPyEngine
{

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////

         PkPyEngine(void);
        ~PkPyEngine(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        bool
        exec(const char* s, py_CompileMode mode) const noexcept;
        // [Abstract]
        //
        // [Returns]

        std::string
        get_prompt(int16_t index) const noexcept;
        // [Abstract]
        //
        // [Returns]

        bool
        load_script(const char* path) const noexcept;
        // [Abstract]
        //
        // [Returns]
};

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker