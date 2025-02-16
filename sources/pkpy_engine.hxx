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

// Include the headers of custom modules.
#include "config.hxx"

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
        // [Abstract]
        //   Default constructor of PkPyEngine.

        ~PkPyEngine(void);
        // [Abstract]
        //   Default destructor of PkPyEngine.

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::string
        get_prompt(const int16_t index) const noexcept;
        // [Abstract]
        //   Returns the prompt string.
        //
        // [Args]
        //   index (const int16_t): [IN] Index of the prompt to be returned.
        //
        // [Returns]
        //   (std::string): Prompt string.

        void
        setup_config(NishikiConfig* config) const noexcept;
        // [Abstract]
        //   Read the config file and setup the config data.
        //
        // [Args]
        //   config (NiShiKiConfig*): [OUT] Config values.

    private:

        bool
        exec(const char* str, py_CompileMode mode) const noexcept;
        // [Abstract]
        //   Runs the given string as a Python script.
        //
        // [Args]
        //   str  (const char*)   : [IN] Python command to run.
        //   mode (py_CompileMode): [IN] Compile mode (exec or eval).
        //
        // [Returns]
        //   (bool): True if success.

        bool
        load_script(const char* path) const noexcept;
        // [Abstract]
        //   Read the specified file and run as a Python script.
        //
        // [Args]
        //   path (const char*): [IN] File path to be read.
        //
        // [Returns]
        //   (bool): True if success.
};

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
