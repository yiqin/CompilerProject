
#include "catch.hpp"

#include <sstream>

#include "parser.tab.hpp"
#include "scanner.hpp"


TEST_CASE ("scanner::Scanner recognizes lexemes") {
    typedef parser::Parser::semantic_type semantic_type;
    typedef parser::Parser::location_type location_type;
    typedef parser::Parser::token_type token_type;
    typedef parser::Parser::token token;

    std::istringstream input;
    std::ostringstream output;
    scanner::Scanner scanner(&input, &output);

    semantic_type yylval;
    location_type yylloc;

    REQUIRE (yylloc.begin.line == 1);
    REQUIRE (yylloc.begin.column == 1);
    REQUIRE (yylloc.end.line == 1);
    REQUIRE (yylloc.end.column == 1);

    SECTION ("recognize '+'") {
        input.str("+");
        auto tok = scanner.lex(&yylval, &yylloc);

        REQUIRE (tok == token::PLUS);
        REQUIRE (output.str() == "");

        REQUIRE (yylloc.begin.line == 1);
        REQUIRE (yylloc.begin.column == 1);
        REQUIRE (yylloc.end.line == 1);
        REQUIRE (yylloc.end.column == 2);
    }
}
