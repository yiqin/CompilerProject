%{

#include "scanner.hpp"

#include <string>

// WARNING: USE OF A DIRTY MACRO
//          The parser will by default try to use a global yylex function. We //
//          want it to use the yylex function #define yylex corresponding to its
//          member reference "scanner".
#define yylex scanner.yylex

%}

// Generate a header file.
%defines

// Use the c++ skeleton file.
%skeleton "lalr1.cc"

// Name to use for the generated parser class.
%define "parser_class_name" "Parser"

// Use this namespace to contain all the generated classes related to Parser,
// including Parser itself.
%define "namespace" "parser"

// Pass by reference an instance of the scanner to the parse() function. This
// allows for the parser to drive the scanner.
%parse-param { scanner::Scanner& scanner }

// NOTE: If you are getting errors like "error: 'scanner' has not been
//       declared" when you #include "parser.tab.hpp", you may need to forward
//       declare scanner::Scanner like below:
//
//       namespace scanner { class Scanner; }


%union {
    int          integer_value;
    std::string* string_value;
}

%token INT
%token STRING
%token <integer_value> CONST_INT
%token <string_value>  CONST_STRING

%token <string_value> IDENT

%token EXTERN

%token IF
%token ELSE
%token WHILE
%token DO
%token FOR
%token RETURN

%token EQUAL
%token NEQUAL
%token LESS
%token GREATER
%token LESSEQUAL
%token GREATEREQUAL

%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE

%token SHIFTRIGHT
%token SHIFTLEFT
%token MODULO

%token END 0


%%


program :
    external_declaration
  | program external_declaration
;

external_declaration :
    declaration        // Declaration Global
  | EXTERN declaration // Set Extern attribute
  | function_definition
;

function_definition :
    type function_declarator decl_glb_fct compound_instruction {

    }
;

decl_glb_fct :
// Get function name - Create a spot to store the function - set attributes
;

declaration :
    type declarator_list ';'
;

type :
    INT     // set INT
  | STRING  // set String
;

declarator_list :
    declarator              // Propagate code
  | declarator_list ',' declarator
;

declaration_list :
    declaration                   {
        //std::cout << "- declare local variable: " << $1.string_value << std::endl;
    }
  | declaration_list declaration      // Set locals
;

declarator :
    IDENT                   // Create Variable
  | function_declarator               // Create Function
;

function_declarator :
    IDENT '(' ')'               // Create function name
  | IDENT '(' parameter_list ')'      // Create partial function
;

parameter_list :
    parameter_declaration
  | parameter_list ',' parameter_declaration // Insert parameters
;

parameter_declaration :
    type IDENT              // Type declaration
;

instruction :
    ';'
  | compound_instruction    // {$$=$1;}
  | expression_instruction  // {$$=$1;}
  | iteration_instruction   // {$$=$1;}
  | select_instruction      // {$$=$1;}
  | jump_instruction        // {$$=$1;}
;

expression_instruction :
    expression ';'
  | assignment ';'
;

assignment :
    IDENT '=' expression
;

compound_instruction :
    block_start declaration_list instruction_list block_end  // {$$=$3;}
  | block_start declaration_list block_end
  | block_start instruction_list block_end  // {$$=$2;}
  | block_start block_end
;


block_start :
    '{'  // Init your hash table - symbol table
;

block_end :
    '}' // Empty hash table
;

instruction_list :
    instruction  // {$$=$1;}
  | instruction_list instruction
;

select_instruction :
    cond_instruction instruction
  | cond_instruction instruction ELSE instruction
;

cond_instruction :
    IF '(' condition ')'  // {$$=$3;}
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
    EQUAL         // { $$.entier = EQUAL;        }  // equal
  | NEQUAL        // { $$.entier = NEQUAL;       }  // not-equal
  | LESS          // { $$.entier = LESS;         }  // less-than
  | GREATER       // { $$.entier = GREATER;      }  // greater-than
  | LESSEQUAL     // { $$.entier = LESSEQUAL;    }  // less-than-or-equal
  | GREATEREQUAL  // { $$.entier = GREATEREQUAL; }  // greater-than-or-equal
;

expression :
    additive_expression  // {$$=$1;}
  | expression SHIFTLEFT additive_expression //  Compute expression
  | expression SHIFTRIGHT additive_expression // Compute expression
;

additive_expression :
    multiplicative_expression  // {$$=$1;}
  | additive_expression PLUS multiplicative_expression // Compute expression
  | additive_expression MINUS multiplicative_expression // Compute expression
;

multiplicative_expression :
    unary_expression // {$$=$1;}
  | multiplicative_expression MULTIPLY unary_expression
  | multiplicative_expression DIVIDE unary_expression
  | multiplicative_expression MODULO unary_expression
;

unary_expression:
    postfix_expression  // {$$=$1;}
  | MINUS unary_expression
;

postfix_expression :
    primary_expression  // {$$=$1;}
  | IDENT '(' argument_expression_list ')'
  | IDENT '(' ')'
;

argument_expression_list:
    expression
  | argument_expression_list ',' expression
;

primary_expression :
    IDENT
  | CONST_INT
  | CONST_STRING
  | '(' expression ')'
;


%%


void parser::Parser::error (const location_type& loc, const std::string& message) {
    std::cout << "ERROR: at " << loc << " - " << message << std::endl;
}
