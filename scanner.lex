%{
#include<stdio.h>
#include<string.h>
#include <stdbool.h>

typedef enum {FUNCTION, INT} symbol_type;
typedef enum {EXTERN, FUNCTION_PARAMETER, GLOBAL, BLOCK_LOCAL, FOR_LOOP_STATEMENT} symbol_scope;

struct Symbol
{
    char name[100];
    symbol_type type;
    symbol_scope scope;
};

bool isNewSymbol = false;

typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a, size_t initialSize) {
  a->array = (int *)malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, int element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (int *)realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

Symbol a[100];
int i = 0;

%}

%%

\/\/.* ; 
\/\*(.*\n)*.*\*\/ ; 

extern {printf("extern: external_declaration\n");}
int {printf("%s: integer_declaration\n", yytext); isNewSymbol = true;}
if|else|while|do|for|return    {printf("control\n");}

[a-z]([a-z]|[0-9])* {
    printf("%s: identifier\n", yytext);
    if (isNewSymbol) {
        struct Symbol currentSymbol;
        strcpy(currentSymbol.name, yytext);
        a[i] = currentSymbol;
        i++;
        isNewSymbol = false;
    }

}

([0-9][a-z])

"(" {printf("(: left_parenthesis\n");}
")" {printf("): right_parenthesis\n");}

";" {}

%%

int main()
{
    yylex();
    
    printf("\n\n\nSymbol Table\n");
    printf("--------------------------------------------------------\n");
    printf("Symbol name  |           Type       |   Scope\n");
    printf("--------------------------------------------------------\n");
    for(int j = 0; j <= i; j++)
        printf("%s\n", a[j].name);

    return 0;
}

int yywrap()
{
    return 1;
}
