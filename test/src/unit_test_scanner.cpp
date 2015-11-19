
#include "catch.hpp"

#include <sstream>

#include "location.hh"
#include "parser.tab.hpp"
#include "position.hh"
#include "scanner.hpp"


TEST_CASE ("scanner::Scanner recognizes lexemes") {
    typedef parser::Parser::semantic_type semantic_type;
    typedef parser::Parser::token_type    token_type;
    typedef parser::Parser::token         token;
    typedef parser::location              location_type;
    typedef parser::position              position_type;

    std::istringstream input;
    std::ostringstream output;
    scanner::Scanner scanner(&input, &output);

    semantic_type value;
    location_type location;
    position_type initial_position(nullptr, 1, 1);

    REQUIRE (location.begin == initial_position);
    REQUIRE (location.end   == initial_position);


    // ARITHMETIC OPERATORS

    SECTION ("recognize '+'") {
        input.str("+");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::PLUS);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '-'") {
        input.str("-");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::MINUS);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '*'") {
        input.str("*");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::MULTIPLY);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '/'") {
        input.str("/");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::DIVIDE);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '%'") {
        input.str("%");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::MODULO);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }


    // BITSHIFT OPERATORS

    SECTION ("recognize '>>'") {
        input.str(">>");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::SHIFTRIGHT);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize '<<'") {
        input.str("<<");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::SHIFTLEFT);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }


    // MISCELLANEOUS LEXEMES

    SECTION ("recognize ','") {
        input.str(",");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>(','));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '='") {
        input.str("=");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>('='));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize ';'") {
        input.str(";");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>(';'));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }


    // COMPARISON OPERATORS

    SECTION ("recognize '=='") {
        input.str("==");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::EQUAL);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize '!='") {
        input.str("!=");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::NEQUAL);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize '<'") {
        input.str("<");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::LESS);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '>'") {
        input.str(">");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::GREATER);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '<='") {
        input.str("<=");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::LESSEQUAL);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize '>='") {
        input.str(">=");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::GREATEREQUAL);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }


    // CONTROL STRUCTURE KEYWORDS

    SECTION ("recognize 'if'") {
        input.str("if");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::IF);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize 'else'") {
        input.str("else");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::ELSE);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 4);
    }

    SECTION ("recognize 'while'") {
        input.str("while");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::WHILE);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 5);
    }

    SECTION ("recognize 'do'") {
        input.str("do");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::DO);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 2);
    }

    SECTION ("recognize 'for'") {
        input.str("for");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::FOR);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 3);
    }

    SECTION ("recognize 'return'") {
        input.str("return");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::RETURN);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 6);
    }


    // TYPE KEYWORDS

    SECTION ("recognize 'int'") {
        input.str("int");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::INT);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 3);
    }

    SECTION ("recognize 'string'") {
        input.str("string");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::STRING);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 6);
    }

    SECTION ("recognize 'extern'") {
        input.str("extern");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::EXTERN);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 6);
    }


    // PARENTHESES AND BRACES

    SECTION ("recognize '('") {
        input.str("(");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>('('));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize ')'") {
        input.str(")");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>(')'));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '{'") {
        input.str("{");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>('{'));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }

    SECTION ("recognize '}'") {
        input.str("}");
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == static_cast<token_type>('}'));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + 1);
    }


    // ARBITRARY SEQUENCES

    SECTION ("recognize an identifier \"asDf_97pQ__\"") {
        std::string input_value = "asDf_97pQ__";
        input.str(input_value);
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::IDENT);
        REQUIRE (value.as<std::string>() == input_value);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + input_value.size());
    }

    SECTION ("recognize an integer literal \"012345671001\"") {
        std::string input_value = "012345671001";
        input.str(input_value);
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::CONST_INT);
        REQUIRE (value.as<int>() == atoi(input_value.c_str()));
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + input_value.size());
    }

    SECTION ("recognize a string literal \" - *&#( asDf _97 pQ__\"") {
        std::string input_value = "\" - *&#( asDf _97 pQ__\"";
        input.str(input_value);
        auto tok = scanner.lex(&value, &location);

        REQUIRE (tok == token::CONST_STRING);
        REQUIRE (value.as<std::string>() == input_value);
        REQUIRE (output.str() == "");

        REQUIRE (location.begin == initial_position);
        REQUIRE (location.end   == initial_position + input_value.size());
    }
}
