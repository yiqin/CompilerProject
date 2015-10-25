%option c++


%{

#include "preprocessor.hpp"

#include <cassert>

#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "macro.hpp"


const char whitespace[] = " \n\r\t\v";

// Strip leading and trailing whitespace.
void strip (std::string& s) {
    s.erase(0, s.find_first_not_of(whitespace));
    s.erase(s.find_last_not_of(whitespace) + 1);
}

%}


%x block_comment def_macro def_macro_args

identifier  [A-Za-z_][A-Za-z0-9_]*


%%


%{
    typedef std::map<std::string, preprocessor::Macro::Ptr> Macro_Map;
    Macro_Map macros;
    preprocessor::Macro::Ptr curr_macro;
%}

    /* Macro definitions. */

#define\ +{identifier}\ +               {
    // Strip off "#define ".
    std::string name(&yytext[8]);
    strip(name);

    curr_macro.reset(new preprocessor::Macro(std::move(name)));
    BEGIN(def_macro);
}

#define\ +{identifier}\(\ *             {
    // Strip off "#define ".
    std::string name(&yytext[8]);
    strip(name);

    // Strip off trailing '('.
    name.erase(name.size() - 1);

    curr_macro.reset(new preprocessor::Macro_Function(std::move(name)));
    BEGIN(def_macro_args);
}

<def_macro_args>{identifier}\ *[,\)]\ * {
    std::string name(yytext);
    auto terminator_position = name.find_first_of(",)");
    bool last_arg = name[terminator_position] == ')';

    // Strip off "\ *,\ *".
    name.erase(terminator_position);
    strip(name);

    // Append argument name.
    std::static_pointer_cast<preprocessor::Macro_Function>(curr_macro)
        ->argument_names().push_back(name);

    if (last_arg) {
        BEGIN(def_macro);
    }
}

<def_macro>.*[^\\]\n                    {
    std::string text(yytext);
    text.erase(text.size() - 1);
    curr_macro->body(std::move(text));
    macros[curr_macro->name()] = curr_macro;
    BEGIN(INITIAL);
}

    /* Macro use. */

^.*$  {
    std::string line(yytext);
    bool found = false;
    for (const auto& pair : macros) {
        auto position = line.find(pair.first);
        if (position != std::string::npos) {
            found = true;

            const auto& name = pair.first;
            auto& macro = pair.second;
            auto function_macro =
                std::dynamic_pointer_cast<preprocessor::Macro_Function>(macro);

            if (function_macro) {
                auto l_parens_pos = line.find_first_not_of(
                    whitespace, position + name.size());
                assert(line[l_parens_pos] == '(');

                // Extract argument list.
                preprocessor::Macro_Function::Argument_List arg_list;
                auto token_start = l_parens_pos + 1;
                auto token_end = line.find_first_of(",)", l_parens_pos);
                while (line[token_end] == ',') {
                    auto arg_name = line.substr(
                        token_start, token_end - token_start);
                    strip(arg_name);
                    arg_list.push_back(arg_name);
                    token_start = token_end + 1;
                    token_end = line.find_first_of(",)", token_start);
                }
                auto arg_name = line.substr(
                    token_start, token_end - token_start);
                strip(arg_name);
                arg_list.push_back(arg_name);

                line.replace(
                    position,
                    token_end - position + 1,
                    function_macro->resolve(arg_list)
                );
            } else {
                line.replace(position, name.size(), macro->body());
            }
            break;
        }
    }

    if (found) {
        // A macro found. Return resultant line(s) for re-processing. This
        // allows us to process the macro result and any comments or other
        // macros used in it.
        for (auto iter = line.crbegin(); iter != line.crend(); ++iter) {
            unput(*iter);
        }
    } else {
        // No macro found. Process the line normally.
        REJECT;
    }
}

    /* Comments. */

"/*"                    {
    BEGIN(block_comment);
}

<block_comment>.*"*/"   {
    BEGIN(INITIAL);
}

<block_comment>.*       {
    std::string line(yytext);
    if (line.find("*/") != std::string::npos) {
        REJECT;
    }
}

"//".*


%%


/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int PreprocessorFlexLexer::yywrap () {
    return 1;
}


int main () {
    PreprocessorFlexLexer pp;
    pp.yylex();
    return 0;
}
