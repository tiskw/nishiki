////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: config.cxx                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "config.hxx"

#include <filesystem>
#include <iostream>

#include "edit_helper.hxx"

// Import toml++ library.
#define TOML_EXCEPTIONS 0
#include <toml.hpp>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable
////////////////////////////////////////////////////////////////////////////////////////////////////

NishikiConfig config;


////////////////////////////////////////////////////////////////////////////////////////////////////
// File-local functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
set_config(const toml::table& table, const toml::key& section, const toml::key& value)
noexcept
// [Abstract]
//   Read one config item to the global variable `config`.
//
// [Args]
//   table   (const toml::table&): [IN] Top node of the config file.
//   section (const toml::key&)  : [IN] Section name.
//   value   (const toml::key&)  : [IN] Value name.
//
// [Returns]
//   void
{   // {{{

    ///// FUNCTION-LOCAL FUNCTION /////
    //
    // Show error message and quit this software.
    //
    // [Args]
    //   section (const toml::key&): [IN] Section name.
    //   value   (const toml::key&): [IN] Value name.
    //
    // [Returns]
    //   void
    //
    // [Notes]
    //   This is a function-local function (defined inside a functin and only effective inside the
    //   function). This function-local function is realized using lambda expression and constexpr
    //   specifier, therefore this function will be evaluated on compile-time and cause no runtime
    //   load. This function-local function is sometimes used in the source code of NiShiKi.
    //
    constexpr auto show_error_message = [](const toml::key& section, const toml::key& value)
    {
        std::cout << "\033[33m";
        std::cout << "NiShiKi: Error occured while parsing config file\n";
        std::cout << "NiShiKi: Undefined entry name: " << section << "." << value;
        std::cout << "\033[m" << std::endl;
    };

    // Get target config item.
    toml::node_view node = table[section][value];

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [GENERAL] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    if      ((section == "GENERAL") and (value == "area_hgt"     )) config.area_hgt      = node.value_or(config.area_hgt);
    else if ((section == "GENERAL") and (value == "column_margin")) config.column_margin = node.value_or(config.column_margin);
    else if ((section == "GENERAL") and (value == "histhint_pre" )) config.histhint_pre  = node.value_or(config.histhint_pre);
    else if ((section == "GENERAL") and (value == "histhint_post")) config.histhint_post = node.value_or(config.histhint_post);
    else if ((section == "GENERAL")                               ) show_error_message(section, value);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [PROMPT] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if ((section == "PROMPT") and (value == "prompt1"    )) config.prompt1     = node.value_or(config.prompt1);
    else if ((section == "PROMPT") and (value == "prompt2"    )) config.prompt2     = node.value_or(config.prompt2);
    else if ((section == "PROMPT") and (value == "prompt3_ins")) config.prompt3_ins = node.value_or(config.prompt3_ins);
    else if ((section == "PROMPT") and (value == "prompt3_nor")) config.prompt3_nor = node.value_or(config.prompt3_nor);
    else if ((section == "PROMPT") and (value == "prompt_comp")) config.prompt_comp = node.value_or(config.prompt_comp);
    else if ((section == "PROMPT")                             ) show_error_message(section, value);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [ALIAS] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if (section == "ALIAS")
    {
        // Get key and value, and registers them to the member variable.
        StringX key = StringX(std::string(value.str()));
        StringX val = StringX(node.value_or(""));

        config.aliases[key] = val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [KEYBIND] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if (section == "KEYBIND")
    {
        // Get key and value, and registers them to the member variable.
        std::string key = std::string(value.str());
        std::string val = std::string(node.value_or(""));

        config.keybind[key] = val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [COMPLETION] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if ((section == "COMPLETION") and (value == "completions"))
    {
        for (const auto& entry : *node.as_array())
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // Read completion pattern
            ////////////////////////////////////////////////////////////////////////////////////

            // Get the node of the completion pattern.
            const toml::array* pattern_node = entry.as_array()->at(0).as_array();

            // Read pattern strings.
            std::vector<std::string> pattern;
            for (const auto& ptrn_token : *pattern_node)
                pattern.push_back(ptrn_token.value_or(""));

            ////////////////////////////////////////////////////////////////////////////////////
            // Read completion type
            ////////////////////////////////////////////////////////////////////////////////////

            // Get the node of the completion type.
            const char* ctype_strptr = entry.as_array()->at(1).value_or("");

            // Compute completion type.
            EditHelper::CompType ctype;
            if      (strcmp(ctype_strptr, "command") == 0) ctype = EditHelper::CompType::COMMAND;
            else if (strcmp(ctype_strptr, "option")  == 0) ctype = EditHelper::CompType::OPTION;
            else if (strcmp(ctype_strptr, "path")    == 0) ctype = EditHelper::CompType::PATH;
            else if (strcmp(ctype_strptr, "preview") == 0) ctype = EditHelper::CompType::PREVIEW;
            else if (strcmp(ctype_strptr, "shell")   == 0) ctype = EditHelper::CompType::SHELL;
            else if (strcmp(ctype_strptr, "subcmd")  == 0) ctype = EditHelper::CompType::SUBCMD;
            else                                           ctype = EditHelper::CompType::PATH;

            ////////////////////////////////////////////////////////////////////////////////////
            // Read optional string
            ////////////////////////////////////////////////////////////////////////////////////

            // Get the node of the optional string.
            const char* opt_strptr = entry.as_array()->at(2).value_or("");

            // Register the triplet of completion pattern, completion type, and optional string.
            config.completions.emplace_back(pattern, ctype, std::string(opt_strptr));
        }
    
    }
    else if (section == "COMPLETION") show_error_message(section, value);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [PREVIEW] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if ((section == "PREVIEW") and (value == "previews"))
    {
        // Read all preview items.
        for (const auto& entry : *node.as_array())
        {
            const toml::array* entry_items    = entry.as_array();
            const char*        pattern_strptr = entry_items->at(0).value_or("");
            const char*        command_strptr = entry_items->at(1).value_or("");

            config.previews.emplace_back(pattern_strptr, command_strptr);
        }
    }
    else if ((section == "PREVIEW") and (value == "preview_delim")) config.preview_delim = node.value_or(config.preview_delim);
    else if ((section == "PREVIEW") and (value == "preview_ratio")) config.preview_ratio = node.value_or(config.preview_ratio);
    else if ((section == "PREVIEW")                               ) show_error_message(section, value);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Otherwise, show error message and exit.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    else show_error_message(section, value);

}   // }}}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
load_config(std::string filepath)
noexcept
{   // {{{

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Set default values
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // The [GENERAL] settings.
    config.area_hgt      = 6u;
    config.column_margin = 3u;
    config.histhint_pre  = "";
    config.histhint_post = "";

    // The [PROMPT] settings.
    config.prompt1     = "[{user}@{host}]-[{cwd}]";
    config.prompt2     = "{git}";
    config.prompt3_ins = ">> ";
    config.prompt3_nor = "|| ";
    config.prompt_comp = "| ";

    // The [ALIAS] settings.
    config.aliases.clear();

    // The [KEYBIND] settings.
    config.keybind.clear();

    // The [COMPLETION] settings.
    config.completions.clear();

    // The [PREVIEW] settings.
    config.previews.clear();
    config.preview_delim = " | ";
    config.preview_ratio = 0.45;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read confg file
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Compute the filepath to the config file if "auto" is specified.
    //
    // Priority rule of the loading target is:
    //   1. ./config.toml
    //   2. ~/.config/nishiki/config.toml
    //   3. ~/.local/nishiki/config.toml
    //
    if (filepath == "auto")
    {
        std::filesystem::path home  = std::filesystem::path(getenv("HOME"));
        std::filesystem::path path1 = std::filesystem::path(".") / "config.toml";
        std::filesystem::path path2 = home / ".config" / "nishiki" / "config.toml";

        if      (std::filesystem::exists(path1)) { filepath = std::filesystem::canonical(path1).string(); }
        else if (std::filesystem::exists(path2)) { filepath = std::filesystem::canonical(path2).string(); }
        else                                     { filepath = "config.toml";                              }
    }

    // Read specified TOML file.
    toml::parse_result result = toml::parse_file(filepath);
    if (not result)
    {
        std::cout << "\033[33mNiShiKi: Error occured while parsing config file: " << filepath << "\033[m\n";
        std::cout << "\033[33mNiShiKi: " << result.error() << "\033[m" << std::endl;
        return;
    }

    // Steal the table from the result.
    toml::table table = std::move(result).table();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse config contents
    ////////////////////////////////////////////////////////////////////////////////////////////////

    for (const auto& node_section : table)
    {
        // If the node is a table then read the values contained in the table.
        if (node_section.second.is_table())
            for (auto node_value : *node_section.second.as_table())
                set_config(table, node_section.first, node_value.first);
    }

}   // }}}


// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
