%option main


%{

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

= {
    std::cout << "=: affectation" << std::endl;
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

\"[^\"]\" {
    std::cout << "const string" << std::endl;
}

.   ;
\n  ;

%%
