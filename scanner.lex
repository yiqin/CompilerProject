%{
#include<stdio.h>
#include<string.h>

typedef enum {FUNCTION, INT} symbol_type;
typedef enum {EXTERN, FUNCTION_PARAMETER, GLOBAL, BLOCK_LOCAL, FOR_LOOP_STATEMENT} symbol_scope;

struct Symbol
{
    symbol_type type;
    symbol_scope scope;
};

struct Symbol currentSymbol;

%}

%%

\/\/.* ; 
\/\*(.*\n)*.*\*\/ ; 

extern {printf("extern: external_declaration\n");}
int {printf("%s: integer_declaration\n", yytext);}
if|else|while|do|for|return    {printf("control\n");}
[a-z]([a-z]|[0-9])* {printf("%s: identifier\n", yytext);}

"(" {printf("(: left_parenthesis\n");}
")" {printf("): right_parenthesis\n");}



";" {}

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
