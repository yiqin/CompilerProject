%{

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <vector>

typedef enum {FUNCTION, INT} symbol_type;
typedef enum {EXTERN, FUNCTION_PARAMETER, GLOBAL, BLOCK_LOCAL, FOR_LOOP_STATEMENT} symbol_scope;

struct Symbol
{
    char name[100];
    char type[100];
    char scope[100];
};

bool isNewSymbol = false;
bool isFunctionParameters = false;
bool isForLoop = false;
int blocksCount = 0;

char currentType[100];
char currentScope[100] = "global";

std::vector<Symbol> symbol_table;
int i = 0;

%}

%%

"const int" {
    printf("const int\n");
}

"const string" {
    printf("const string\n");
}

("+"|"-"|"++"|"--"|"+="|"=+"|"-="|"=-") {
    printf("%s: additive operation\n", yytext);
}

(=|>|<|>=|<|<=|==|!=) {
    printf("%s: comparison operation\n", yytext);
}

(<<|>>) {
    printf("%s: expression\n", yytext);
}

if|else|while|do|for|return    {printf("%s: control\n", yytext);}

int|string|extern {printf("%s: type\n", yytext);}

[a-z]([a-z]|[0-9])* {
    printf("%s: identifier\n", yytext);
    if (isNewSymbol) {
        struct Symbol currentSymbol;
        strcpy(currentSymbol.name, yytext);
        strcpy(currentSymbol.type, currentType);

        if (isFunctionParameters) {
            strcpy(currentSymbol.scope, "function parameter");
        } else if(isForLoop) {
            strcpy(currentSymbol.scope, "for-loop statement");
        } else {
            strcpy(currentSymbol.scope, currentScope);
        }

        symbol_table.push_back(currentSymbol);
        i++;
        isNewSymbol = false;
        strcpy(currentType, "");
    }
}

"(" {printf("(: left_parenthesis\n"); }
")" {
    printf("): right_parenthesis\n");
}

"{" {
    printf("{: left_bracket\n");
}
"}" {
    printf("}: right_bracket\n");

}

";" {}

.   ;
\n  ;

%%

int main()
{
    yylex();

    return 0;
}

int yywrap()
{
    return 1;
}
