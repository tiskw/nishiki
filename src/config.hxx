////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: config.hxx                                                                  ///
///                                                                                              ///
/// This file provides:                                                                          ///
///   - the global variable `config` which stores configuration values for NiShiKi.              ///
///   - the function `load_config` which reads config file and update `config` variable.         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HXX
#define CONFIG_HXX

#include <map>

#include "edit_helper.hxx"
#include "string_x.hxx"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    // [GENERAL] settings.
    uint16_t    area_hgt;
    uint16_t    column_margin;
    std::string datetime_pre;
    std::string datetime_post;
    std::string histhint_pre;
    std::string histhint_post;

    // [PROMPT] settings.
    std::string prompt1;
    std::string prompt2;
    std::string prompt3_ins;
    std::string prompt3_nor;
    std::string prompt_comp;

    // [ALIAS] settings.
    std::map<StringX, StringX> aliases;

    // [KEYBIND] settings.
    std::map<std::string, std::string> keybind;

    // [COMPLETION] settings.
    std::vector<std::tuple<std::vector<std::string>, EditHelper::CompType, std::string>> completions;

    // [PREVIEW] settings.
    std::vector<std::pair<std::string, std::string>> previews;
    std::string                                      preview_delim;
    float                                            preview_ratio;
}
NishikiConfig;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototype declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

extern NishikiConfig config;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
load_config(std::string filepath)
noexcept;
// [Abstract]
//   Load config file written in TOML format.
//   The result will be stored in the global variable `config` that is declared in `config.cxx`.
//
// [Args]
//   filepath (const char*): [IN] Path to TOML file.
//
// [Returns]
//   void


#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
