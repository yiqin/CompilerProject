%{
#include “your_file.h”
%}

%token INT
%token STRING
%token CONST_INT
%token CONST_STRING

%token IDENT

%token EXTERN

%token IF
%token ELSE
%token WHILE
%token DO
%token RETURN
%token FOR
%token THEN

%token assignment
%token INF
%token EGAL
%token SUP
%token INFEQUAL
%token SUPEQUAL
%token DIFF

%token PLUS
%token MINUS
%token MULTI
%token DIV

%token SHIFTRIGHT
%token SHIFTLEFT
%token MODULO



%%
program :
external-declaration
| program external-declaration
;

external-declaration :
declaration 	// Declaration Global
| EXTERN declaration // Set Extern attribute
| function-definition
;

function-definition :
type function_declarator decl_glb_fct compound_instruction // generate code function

;

decl_glb_fct :
// Get function name - Create a spot to store the function - set attributes
;

declaration :
type declarator_list ';'
;

type :
INT 					// set INT
| STRING 				// set String
;

declarator_list :
declarator 				// Propagate code
| declarator_list ',' declarator
;

declaration_list :
declaration 				// Set locals
| declaration_list declaration  	// Set locals
;

declarator :
IDENT 					// Create Variable
| function_declarator 		        // Create Function
;

function_declarator :
IDENT '(' ')' 				// Create function name
| IDENT '(' parameter_list ')'  	// Create partial function
;

parameter_list :
parameter_declaration
| parameter_list ',' parameter_declaration // Insert parameters
;

parameter_declaration :
type IDENT 				// Type declaration
;

instruction :
';'
| compound_instruction {$$=$1;}
| expression_instruction  {$$=$1;}
| iteration_instruction  {$$=$1;}
| select_instruction  {$$=$1;}
| jump_instruction {$$=$1;}
;

expression_instruction :
expression ';'
| assignment ';'
;

assignment :
IDENT assignment expression
;

compound_instruction :
block_start declaration_list instruction_list block_end {$$=$3;}
| block_start declaration_list block_end
| block_start instruction_list block_end {$$=$2;}
| block_start block_end
;


block_start :
'{'  // Init your hash table - symbol table
;

block_end :
'}' // Empty hash table
;

instruction_list :
instruction  {$$=$1;}
| instruction_list instruction
;

select_instruction :
cond_instruction instruction
| cond_instruction instruction ELSE instruction
;

cond_instruction :
IF '(' condition ')' {$$=$3;}
;

iteration_instruction :
WHILE '(' condition ')' instruction // Handle while loop
| DO instruction WHILE '(' condition ')'
| FOR '(' assignment ';' condition ';' assignment ')' instruction
;

jump_instruction:
RETURN expression ';'
;

condition :
expression comparison_operator expression
;

comparison_operator :
EGAL  {$$.entier=EGAL;}
| DIFF {$$.entier=DIFF;}
| INF  {$$.entier=INF;}
| SUP  {$$.entier=SUP;}
| INFEQUAL {$$.entier=INFEQUAL;}
| SUPEQUAL {$$.entier=SUPEQUAL;}
;

expression :
expression_additive {$$=$1;}
| expression SHIFTLEFT expression_additive //  Compute expression
| expression SHIFTRIGHT expression_additive // Compute expression
;

expression_additive :
expression_multiplicative {$$=$1;}
| expression_additive PLUS expression_multiplicative // Compute expression
| expression_additive MINUS expression_multiplicative // Compute expression
;

expression_multiplicative :
unary_expression{$$=$1;}
| expression_multiplicative MULTI unary_expression
| expression_multiplicative DIV unary_expression
| expression_multiplicative MODULO unary_expression
;

unary_expression:
expression_postfixee {$$=$1;}
| MINUS unary_expression
;

expression_postfixee :
primary_expression {$$=$1;}
| IDENT '(' argument_expression_list')'
| IDENT '(' ')'
;

argument_expression_list:
expression
| argument_expression_list',' expression
;

primary_expression :
IDENT
| CONST_INT
| CONST_STRING
| '(' expression ')'
;




%%

