////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: edit_helper.cxx                                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "edit_helper.hxx"

// Include the headers of STL.
#include <regex>

// Include the headers of custom modules.
#include "config.hxx"
#include "path_x.hxx"
#include "preview.hxx"
#include "string_x.hxx"
#include "utils.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EditHelper: Constructors and destructors
////////////////////////////////////////////////////////////////////////////////////////////////////

EditHelper::EditHelper(const TermSize area)
{   // {{{

    // Get terminal size.
    this->area = area;

    // Create the cache of available command names.
    this->cache_commands = get_system_commands();

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EditHelper: Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<StringX> EditHelper::candidate(const StringX& lhs) noexcept
{   // {{{

    constexpr auto match = [](const Vector<String>& patterns, const Vector<String>& tokens) noexcept -> bool
    // [Abstract]
    //   Return True if the given tokens matched with the given patterns.
    //   The arguments `tokens` is a list of strings, and the argument `patterns`
    //   are list of regular expression strings with the following extra special commands:
    //
    //     * "FILE": existing file path
    //     * ">>"  : skip tokens (available only 1 time in one patterns)
    //
    // [Args]
    //   patterns (const Vector<String>&): [IN] List of regular expression strings.
    //   tokens   (const Vector<String>&): [IN] List of strings to be matched.
    //
    // [Returns]
    //   (bool): True is the given tokens matched with the given patterns.
    {
        // Initialize token index.
        size_t index_token = 0;

        // Run the for-loop based on the pattern index.
        for (size_t index_pattern = 0; index_pattern < patterns.size(); ++index_pattern)
        {
            // If the number of patterns is longer than the number of tokens
            // (i.e. token finished but pattern is exists yet), then returns false.
            if (index_token >= tokens.size())
                return false;

            // Select target pattern and token.
            const String pattern = patterns[index_pattern];
            const String token   = tokens[index_token];

            // Case 1: pattern is ">>".
            if (pattern == ">>")
                index_token = tokens.size() - patterns.size() + index_pattern;

            // Case 2: pattern is "FILE" but the file not exists.
            else if (pattern == "FILE" and (not std::filesystem::exists(token)))
                return false;

            // Case 3: pattern is "FILE" and the file exists.
            else if (pattern == "FILE") /* pass */;

            // Case 4: others.
            else if (not regex_match(token, std::regex(pattern)))
                return false;

            ++index_token;
        };

        // No unprocessed tokens remains if the token matches with the pattern.
        return (index_token == tokens.size());
    };

    constexpr auto get_target = [match](const Vector<String>& tokens) noexcept -> auto
    // [Abstract]
    //   Returns a pair of target completion type and it's optional string.
    //
    // [Args]
    //   tokens (const Vector<String>&): [IN] List of strings to be matched.
    //
    // [Returns]
    //   comp_type (EditHelper::CompType): Target completion type.
    //   option    (String)         : Optional string of the target completion.
    {
        for (const auto& [patterns, comp_type, option] : config.completions)
            if (match(patterns, tokens))
                return std::make_tuple(comp_type, option);

        return std::make_tuple(EditHelper::CompType::NONE, String(""));
    };

    // Clear completion candidates.
    this->cands.clear();

    // Split the given text (left hand side of the cursor) to tokens.
    Vector<StringX> tokens = lhs.tokenize();

    // Drop white-space tokens and convert StringX to String.
    Vector<String> tokens_str;
    for (const StringX& token : tokens)
        if (token.size() > 0 and token[0].value != ' ')
            tokens_str.push_back(token.string());

    // Add empty token if the editing line ends with white-space.
    if (lhs.size() > 0 and lhs.back().value == ' ')
        tokens_str.push_back("");

    // Get completion type and it's optional string.
    const auto& [comp_type, option] = get_target(tokens_str);

    // Compute lines of completion candidates that will be displayed to users.
    switch (comp_type)
    {
        case EditHelper::CompType::COMMAND: this->cands_command (tokens, option); break;
        case EditHelper::CompType::OPTION : this->cands_option  (tokens);         break;
        case EditHelper::CompType::PATH   : this->cands_filepath(tokens);         break;
        case EditHelper::CompType::PREVIEW: this->cands_filepath(tokens);         break;
        case EditHelper::CompType::SHELL  : this->cands_shell   (tokens, option); break;
        case EditHelper::CompType::SUBCMD : this->cands_subcmd  (tokens, option); break;
        case EditHelper::CompType::NONE   : this->cands_filepath(tokens);         break;
    }

    // Returns candidates.
    this->lines_from_cands(this->cands);

    // Update completion lines using preview info.
    if (comp_type == EditHelper::CompType::PREVIEW)
        this->cands_preview(tokens);

    return this->lines;

}   // }}}

StringX EditHelper::complete(const StringX& lhs) const noexcept
{   // {{{

    // Split the given text (left hand side of the cursor) to tokens.
    Vector<StringX> tokens = lhs.tokenize();

    // Do nothing if token is empty.
    if (tokens.size() == 0) return lhs;

    // Get number of completion candidates.
    const size_t num_cands = this->cands.size();

    // Do nothing if no candidate given.
    if (num_cands == 0) return lhs;

    // Concatenate tokens except the last token.
    StringX lhs_without_last_token;
    for (size_t idx = 0; idx < (tokens.size() - 1); ++idx)
        lhs_without_last_token += tokens[idx];

    // Compute completion string.
    if (num_cands == 1 and this->cands[0].first.endswith('/'))
    {
        // Extra slash will be added when directory path is completed.
        // However, slash is already added to the completion token,
        // therefore just adding the completion token is enough.
        return lhs_without_last_token + this->cands[0].first;
    }
    else if (num_cands == 1)
    {
        // Add extra white-space at the end if number of completion candidate is one.
        return lhs_without_last_token + this->cands[0].first + CharX(" ");
    }
    else
    {
        // Create an array of completion strings.
        constexpr auto get_first = [](const std::pair<StringX, StringX>& pair) noexcept -> StringX { return pair.first; };
        Vector<StringX> keys = transform<std::pair<StringX, StringX>, StringX>(this->cands, get_first);

        return lhs_without_last_token + get_common_substring(keys);
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EditHelper: Private functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void EditHelper::cands_command(const Vector<StringX>& tokens, const String& option) noexcept
{   // {{{

    // The `option` should be empty string.
    if (option.size() > 0) return;

    // Get query token.
    const StringX token = tokens[0];

    // Filter matched command names.
    for (const StringX& cmd : this->cache_commands)
        if (cmd.startswith(token))
            this->cands.emplace_back(cmd, cmd);

}   // }}}

void EditHelper::cands_filepath(const Vector<StringX>& tokens) noexcept
{   // {{{

    constexpr auto colorize_token = [](const String& token) noexcept -> String
    // [Abstract]
    //   Colorize the given token.
    //
    // [Args]
    //   token (const String&): [IN] Target token to be colorized.
    //
    // [Returns]
    //   (String): Colorized token.
    {
        return (token.size() > 0 and token.back() == '/') ? "\033[94m" + token + "\033[m" : token;
    };

    // Split user input token to a tuple of:
    //   * directory path to be searched,
    //   * query string for filtering seach result.
    auto [query_dir, query_key] = split_to_target_and_query(tokens);

    // Show dot file if current file name started with dot.
    const bool show_dot = (query_key.size() > 0) and (query_key[0] == '.');

    // Search the directory and filter out unnecessary search results.
    for (const String& name : query_dir.listdir())
    {
        // Skip dot files if the query key is not a dot file.
        if ((not show_dot) and (name[0] == '.'))
            continue;

        //
        if (name.starts_with(query_key))
        {
            this->cands.emplace_back(StringX((query_dir / name).c_str()),  // Query string.
                                     StringX(colorize_token(name)));       // Display string.
        }
    }

}   // }}}

void EditHelper::cands_option(const Vector<StringX>& tokens) noexcept
{   // {{{

    // Cache of the command options.
    static std::map<StringX, std::map<StringX, StringX>> opt_cache;

    // Get the target command.
    StringX command = (tokens.size() > 0) ? tokens[0].strip() : StringX("");

    // Run command with "--help" option if not registered in the cache.
    if (not opt_cache.contains(command))
    {
        // Create new map instance.
        opt_cache[command] = std::map<StringX, StringX>();

        // Run the given command with "--help" option.
        const String output = run_command(command.string() + " --help");

        // parse the output.
        for (String line : split(output, "\n"))
        {
            // Strip whitespaces from the line.
            line = strip(line);

            for (String elem : split(strip(line), " "))
            {
                // Strip whitespaces from the token.
                elem = strip(elem);

                // Remove after comma if exists.
                std::string::size_type pos_comma = elem.find(',');
                if (pos_comma != String::npos)
                    elem.erase(pos_comma);

                // Remove after equal sign if exists.
                std::string::size_type pos_equal = elem.find('=');
                if (pos_equal != String::npos)
                    elem.erase(pos_equal);

                // Both the token and line starts with '-', then register the option to the cache.
                if ((elem.size() > 0 and elem[0] == '-') and (line.size() > 0 and line[0] == '-'))
                    opt_cache[command][StringX(elem)] = StringX(line);
            }
        }
    }

    // Get query token.
    const StringX token = (tokens.size() > 0) ? tokens.back().strip() : StringX("");

    // Add matched options.
    for (const auto& [opt, desc] : opt_cache[command])
        if (opt.startswith(token))
            this->cands.emplace_back(opt, desc);

}   // }}}

void EditHelper::cands_preview(const Vector<StringX>& tokens) noexcept
{   // {{{

    constexpr auto get_last_nonwhitespace_token = [](const Vector<StringX>& tokens) noexcept -> StringX
    // [Abstract]
    //   Returns last non-whitespace token.
    //
    // [Args]
    //   tokens (Vector<StringX>&): [IN] Target tokens.
    //
    // [Returns]
    //   (StringX): Non-whitespace token.
    {
        // Search tokens from the tail.
        for (size_t idx = tokens.size() - 1; idx > 0; --idx)
            if (tokens[idx].size() > 0 and tokens[idx][0].value != ' ')
                return tokens[idx];

        // Returns brank string if not found.
        return StringX("");
    };

    // Delimiter of the preview area.
    const StringX preview_delim = StringX(config.preview_delim);

    // Get the target file path that is a last non-white-space token.
    const StringX path = get_last_nonwhitespace_token(tokens);

    // Compute width of the preview window.
    const uint16_t w = this->area.cols - int(this->area.cols * config.preview_ratio) - preview_delim.size();

    // Get preview result.
    Vector<StringX> preview_lines = preview(path.string(), w, this->area.rows);

    // Append preview lines to the current completion lines.
    for (size_t idx = 0; (idx < preview_lines.size()) and (idx < (size_t) this->area.rows); ++idx)
    {
        // Clip the line to the half-width `w`.
        this->lines[idx] = this->lines[idx].clip(w);

        // Add extra white-space if the line is shorter than the half-width `w`.
        if (this->lines[idx].width() < w)
            this->lines[idx] += CharX(" ") * (w - this->lines[idx].width());

        // Append preview delimiter and line to the target line.
        this->lines[idx] += StringX("\033[m") + preview_delim + preview_lines[idx];

        // Clip the target line again.
        this->lines[idx] = this->lines[idx].clip(this->area.cols - 1);
    }

}   // }}}

void EditHelper::cands_shell(const Vector<StringX>& tokens, const String& option) noexcept
{   // {{{

    // Get the target token.
    const StringX token = (tokens.size() > 0) ? tokens.back().strip() : StringX("");

    // Run specified command.
    const String output = run_command(option);

    for (String line : split(output, "\n"))
    {
        // Strip line.
        line = strip(line);

        // Get 1st token of each line.
        const StringX line_1st_token = StringX(split(line, " ")[0]);

        // Register matched output lines.
        if (line_1st_token.startswith(token))
            this->cands.emplace_back(line_1st_token, line);
    }

}   // }}}

void EditHelper::cands_subcmd(const Vector<StringX>& tokens, const String& option) noexcept
{   // {{{

    // Get the target token.
    const StringX token = (tokens.size() > 0) ? tokens.back().strip() : StringX("");

    // Cache of the command options.
    static std::map<String, Vector<StringX>> subcmd_cache;

    // Run the given command and add the result to the cache
    // if the given command is not registered in the cache.
    if (not subcmd_cache.contains(option))
    {
        // Initialize the result vector.
        subcmd_cache[option] = Vector<StringX>();

        // Run command and register each line.
        for (const String& line : split(run_command(option), "\n"))
            subcmd_cache[option].push_back(StringX(line).strip());
    }

    for (const StringX& line : subcmd_cache[option])
    {
        if (line.startswith(token))
        {
            // Tokenize a line of the commend result.
            const Vector<StringX> elems = line.tokenize();

            // Do nothing if no element found.
            if (elems.size() == 0) continue;

            // Get completion candidate.
            StringX token1 = StringX("\033[32m") + elems[0] + StringX("\033[m");

            // Get description.
            StringX token2;
            for (size_t idx = 2; idx < elems.size(); ++idx)
                token2 += elems[idx];

            // Get width of separator.
            const uint16_t width_seperator = (elems.size() > 1) ? elems[1].size() : 0;

            // Create separator between completion candidate and description.
            const StringX separator = StringX(" ") + CharX(".") * width_seperator + StringX(" ");

            this->cands.emplace_back(elems[0], token1 + separator + token2);
        }
    }

}   // }}}

void EditHelper::lines_from_cands(const Vector<std::pair<StringX, StringX>>& cands) noexcept
{   // {{{

    // Get descriptions of the completion candidates.
    constexpr auto get_second = [](const std::pair<StringX, StringX>& pair) noexcept -> StringX { return pair.second; };
    Vector<StringX> texts = transform<std::pair<StringX, StringX>, StringX>(cands, get_second);

    // Format descriptions in a column style.
    this->lines.clear();
    for (const StringX& line : column(texts, this->area.cols, this->area.rows))
        this->lines.push_back(line);

    // Add decoration at the left line and clip line width.
    for (size_t i = 0; i < this->lines.size(); ++i)
        if (this->lines[i].size() > 0)
            this->lines[i] = this->lines[i].clip(this->area.cols - 3);

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
