 // Enable line number tracking by Flex.
%option yylineno

 // Generate c++ class instead of bare c functions.
%option c++

 // Scanner does not support handling multiple files.
%option yywrap nounput

%{

#include "scanner.hpp"

#include <iostream>
#include <string>
#include <vector>

typedef enum {FUNCTION, INT} symbol_type;
typedef enum {EXTERN, FUNCTION_PARAMETER, GLOBAL, BLOCK_LOCAL, FOR_LOOP_STATEMENT} symbol_scope;

struct Symbol
{
    std::string name;
    std::string type;
    std::string scope;
};

std::vector<Symbol> symbol_table;

%}


%%


("+"|"-"|"++"|"--"|"+="|"-=") {
    std::cout << yytext << ": additive operation" << std::endl;
}

"," {
    std::cout << ",: list delimiter" << std::endl;
}

= {
    std::cout << "=: assignment" << std::endl;
}

(>|<|>=|<|<=|==|!=) {
    std::cout << yytext << ": comparison operation" << std::endl;
}

(<<|>>) {
    std::cout << yytext << ": expression" << std::endl;
}

if|else|while|do|for|return    {
    std::cout << yytext << ": control" << std::endl;
}

int|string|extern {
    std::cout << yytext << ": type" << std::endl;
}

[A-Za-z_][A-Za-z0-9_]* {
    std::cout << yytext << ": identifier" << std::endl;
    Symbol currentSymbol;
    currentSymbol.name = yytext;
    symbol_table.push_back(currentSymbol);
}

"(" {
    std::cout << "(: left_parenthesis" << std::endl;
}
")" {
    std::cout << "): right_parenthesis" << std::endl;
}

"{" {
    std::cout << "{: left_bracket" << std::endl;
}
"}" {
    std::cout << "}: right_bracket" << std::endl;
}

";" {
    std::cout << ";: semi_colon" << std::endl;
}

[0-9]+ {
    std::cout << "const int" << std::endl;
}

\"[^\"]*\" {
    std::cout << "const string" << std::endl;
}

[\n ]  ;

.   {
    std::cerr
        << "ERROR: unrecognized character '" << yytext
        << "' found in line " << yylineno
        << std::endl
        ;
}

%%


/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int ScannerFlexLexer::yywrap () {
    return 1;
}


int main () {
    ScannerFlexLexer scanner;
    scanner.yylex();
    return 0;
}

