////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: config.hxx                                                                  ///
///                                                                                              ///
/// This file provides:                                                                          ///
///   - the global variable `config` which stores configuration values for NiShiKi.              ///
///   - the function `load_config` which reads config file and update `config` variable.         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HXX
#define CONFIG_HXX

// Include the headers of STL.
#include <cstdint>
#include <map>
#include <string>
#include <tuple>
#include <vector>

// Include the headers of custom modules.
#include "edit_helper.hxx"
#include "string_x.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type alias
////////////////////////////////////////////////////////////////////////////////////////////////////

// A map from string to string.
using StringMap = std::map<std::string, std::string>;

// A vector of strings.
using StringVector = std::vector<std::string>;

// Completion item.
using CompletionItem = std::tuple<StringVector, EditHelper::CompType, std::string>;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    // General settings.
    uint16_t    max_hist_size = 5000;
    uint16_t    area_height   = 8;
    uint16_t    column_margin = 3;
    std::string datetime_pre;
    std::string datetime_post;
    std::string histhint_pre;
    std::string histhint_post;

    // Alias settings.
    StringMap aliases;

    // Colorization settings.
    std::string colorize_commands = "cat,cd,chmod,chown,cp,echo,env,export,grep,let,ln,ls,make,mkdir,mv,rm,sed,set,tar,touch,umask,unset";
    std::string colorize_keywords = "case,do,done,elif,else,esac,exit,fi,for,function,if,in,local,read,return,select,shift,then,time,until,while";
    std::string colorize_symbols  = "&,|,>,<,&&,||,>>,<<";

    // Keybind settings.
    StringMap keybind;

    // Completion settings.
    std::vector<CompletionItem> completions;

    // Preview settings.
    StringMap   preview;
    float       preview_ratio = 0.45;
    std::string preview_delim = "|";
}
NishikiConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototype declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

extern NishikiConfig config;

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
