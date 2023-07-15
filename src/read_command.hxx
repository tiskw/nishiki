////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: read_command.hxx                                                            ///
///                                                                                              ///
/// This function defines the class `ReadCommand` that mamanges the user input session.          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef READ_COMMAND_HXX
#define READ_COMMAND_HXX

#include "history_manager.hxx"
#include "string_x.hxx"
#include "text_buffer.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadCommand: A class to read user input with rich interface
////////////////////////////////////////////////////////////////////////////////////////////////////
class ReadCommand
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and destructors
        ////////////////////////////////////////////////////////////////////////////////////////////

         ReadCommand(void);
        ~ReadCommand(void);


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX
        read(const StringX& lhs_ini, const StringX& rhs_ini)
        noexcept;
        // [Abstract]
        //   Read user input with rich interface.
        //
        // [Args]
        //   lhs (const StringX&): [IN] Default left hand side of user input.
        //   rhs (const StringX&): [IN] Default right hand side of user input.
        //
        // [Returns]
        //   (std::map<std::string, std::string>): Parsed command line arguments.


    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        TextBuffer buffer;
        // [Abstract]
        //   Text buffer instance.

        HistoryManager hist;
        // [Abstract]
        //   History manager instance.


        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        StringX
        create_nishiki_command(const std::string& command_keybind)
        const noexcept;
        // [Abstract]
        //   Returns encoded NiShiKi-internal command.
        //   The returned values are delimiter-separated value of the followings:
        //
        //     1. command type ("int" or "ext")
        //     2. left-hand-side of the editing buffer
        //     3. right-hand-side of the editing buffer
        //     4. command contents
        //
        // [Args]
        //   command (const std::string&): [IN] Input command.
        //
        // [Returns]
        //   (StringX): Encoded NiShiKi-internal command.
};


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
