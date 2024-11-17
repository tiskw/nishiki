////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: command_runner.hxx                                                          ///
///                                                                                              ///
/// A command runner for NiShiKi to run user input in a backend. The command runner can manage   ///
/// both normal shell command and NiShiKi-special command. See the comments of                   ///
/// `CommandRunner::command_nishiki` function for more details about the NiShiKi-special command.///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef UNITTEST_HXX
#define UNITTEST_HXX

void
unittest();
// Callback function of the NiShiKi-special command.
// This funtion parse NiShiKi-special command and run it.
//
// [Args]
//   void
//
// [Returns]
//   void

#endif

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
