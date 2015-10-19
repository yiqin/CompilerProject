%{
#include<stdio.h>
#include<string.h>
#include <stdbool.h>

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

Symbol a[1000];
int i = 0;

%}

%%

\/\/.* ; 
\/\*(.*\n)*.*\*\/ ; 

extern {printf("extern: external_declaration\n"); strcpy(currentScope, "extern");}
int {printf("%s: integer_declaration\n", yytext); isNewSymbol = true; strcpy(currentType, "int");}

for {printf("for control\n"); isForLoop = true;}
for"(" {printf("for control\n"); isForLoop = true;}
if|else|while|do|return    {printf("control\n");}


[A-Za-z]([A-Za-z]|[0-9])*"(" {
    size_t indexOfNullTerminator = strlen(yytext);
    yytext[indexOfNullTerminator-1] = 0; 
    printf("%s: function identifier\n", yytext);
    if (isNewSymbol) {
        struct Symbol currentSymbol;
        strcpy(currentSymbol.name, yytext);
        strcpy(currentSymbol.type, "function, ");
        strcat(currentSymbol.type, currentType);

        if(strcmp(currentScope, "") == 0) {
            strcpy(currentSymbol.scope, "global");
        } else {
            strcpy(currentSymbol.scope, currentScope);
        }
        isFunctionParameters = true;
        
        
        a[i] = currentSymbol;
        i++;
        isNewSymbol = false;
        strcpy(currentType, "");
        
    }
}

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

        a[i] = currentSymbol;
        i++;
        isNewSymbol = false;
        strcpy(currentType, "");
        
    }

}

"(" {printf("(: left_parenthesis\n"); }
")" {
    printf("): right_parenthesis\n");
    if (isFunctionParameters) {
        isFunctionParameters = false;
    }
    if (isForLoop) {
        isForLoop = false;
    }
}

"{" {
    blocksCount++; 
    strcpy(currentScope, "block local");
}
"}" {
    blocksCount--;
    if (blocksCount < 0) {
        blocksCount = 0;
    }
    if (blocksCount == 0) {
        strcpy(currentScope, "");
    }
    
}

";" {}

.   ;
\n  ;

%%

int main()
{
    yylex();
    
    printf("\n\n\nSymbol Table\n");
    printf("--------------------------------------------------------\n");
    printf("Symbol name  |           Type       |   Scope\n");
    printf("--------------------------------------------------------\n");
    for(int j = 0; j <= i-1; j++)
        printf("%s       |    %s    |        %s\n", a[j].name, a[j].type, a[j].scope);

    return 0;
}

int yywrap()
{
    return 1;
}
