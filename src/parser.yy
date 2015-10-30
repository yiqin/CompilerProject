%{

#include "scanner.hpp"

#include <string>

// WARNING: USE OF A DIRTY MACRO
//          The parser will by default try to use a global yylex function. We
//          want it to use the lex function of its member reference scanner. So
//          #define yylex to refer to this.
#define yylex scanner.lex

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

// Track locations within source file (stdin) for error reporting.
%locations
%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("<stdin>");
}


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
    external_declaration         { std::cout << "program: external_declaration" << std::endl; }
  | program external_declaration { std::cout << "program: program external_declaration" << std::endl; }
;

external_declaration :
    declaration         { std::cout << "external_declaration: declaration" << std::endl; }  // Declaration Global
  | EXTERN declaration  { std::cout << "external_declaration: EXTERN declaration" << std::endl; }  // Set Extern attribute
  | function_definition { std::cout << "external_declaration: function_definition" << std::endl; }
;

function_definition :
    type function_declarator decl_glb_fct compound_instruction {
        std::cout << "function_definition: type function_declarator decl_glb_fct compound_instruction" << std::endl;
    }
;

decl_glb_fct : { std::cout << "decl_glb_fct:" << std::endl; }
// Get function name - Create a spot to store the function - set attributes
;

declaration :
    type declarator_list ';' { std::cout << "declaration: type declarator_list ';'" << std::endl; }
;

type :
    INT    { std::cout << "type: INT" << std::endl; }  // set INT
  | STRING { std::cout << "type: STRING" << std::endl; }  // set String
;

declarator_list :
    declarator                     { std::cout << "declarator_list: declarator" << std::endl; }  // Propagate code
  | declarator_list ',' declarator { std::cout << "declarator_list: declarator_list ',' declarator" << std::endl; }
;

declaration_list :
    declaration                   {
        std::cout << "declaration_list: declaration" << std::endl;
        //std::cout << "- declare local variable: " << $1.string_value << std::endl;
    }
  | declaration_list declaration  { std::cout << "declaration_list: declaration_list declaration" << std::endl; }  // Set locals
;

declarator :
    IDENT               { std::cout << "declarator: IDENT" << std::endl; }    // Create Variable
  | function_declarator { std::cout << "declarator: function_declarator" << std::endl; }              // Create Function
;

function_declarator :
    IDENT '(' ')'                 { std::cout << "declare function '" << *$1 << "'" << std::endl; }  // Create function name
  | IDENT '(' parameter_list ')'  { std::cout << "declare function '" << *$1 << "' with parameters" << std::endl; }  // Create partial function
;

parameter_list :
    parameter_declaration                    { std::cout << "parameter_list: parameter_declaration" << std::endl; }
  | parameter_list ',' parameter_declaration { std::cout << "parameter_list: parameter_list ',' parameter_declaration" << std::endl; }  // Insert parameters
;

parameter_declaration :
    type IDENT { std::cout << "parameter_declaration: type IDENT" << std::endl; }  // Type declaration
;

instruction :
    ';'                    { std::cout << "instruction: ';'"                    << std::endl; }
  | compound_instruction   { std::cout << "instruction: compound_instruction"   << std::endl; }  // {$$=$1;}
  | expression_instruction { std::cout << "instruction: expression_instruction" << std::endl; }  // {$$=$1;}
  | iteration_instruction  { std::cout << "instruction: iteration_instruction"  << std::endl; }  // {$$=$1;}
  | select_instruction     { std::cout << "instruction: select_instruction"     << std::endl; }  // {$$=$1;}
  | jump_instruction       { std::cout << "instruction: jump_instruction"       << std::endl; }  // {$$=$1;}
;

expression_instruction :
    expression ';' { std::cout << "expression_instruction: expression ';'" << std::endl; }
  | assignment ';' { std::cout << "expression_instruction: assignment ';'" << std::endl; }
;

assignment :
    IDENT '=' expression { std::cout << "assignment: IDENT '=' expression" << std::endl; }
;

compound_instruction :
    block_start declaration_list instruction_list block_end { std::cout << "compound_instruction: block_start declaration_list instruction_list block_end" << std::endl; }  // {$$=$3;}
  | block_start declaration_list block_end { std::cout << "compound_instruction: block_start declaration_list block_end" << std::endl; }
  | block_start instruction_list block_end { std::cout << "compound_instruction: block_start instruction_list block_end" << std::endl; }  // {$$=$2;}
  | block_start block_end { std::cout << "compound_instruction: block_start block_end" << std::endl; }
;


block_start :
    '{' { std::cout << "block_start: '{'" << std::endl; }  // Init your hash table - symbol table
;

block_end :
    '}' { std::cout << "block_end '}'" << std::endl; }  // Empty hash table
;

instruction_list :
    instruction                  { std::cout << "instruction_list: instruction" << std::endl; }  // {$$=$1;}
  | instruction_list instruction { std::cout << "instruction_list: instruction_list instruction" << std::endl; }
;

select_instruction :
    cond_instruction instruction                  { std::cout << "select_instruction: cond_instruction instruction" << std::endl; }
  | cond_instruction instruction ELSE instruction { std::cout << "select_instruction: cond_instruction instruction ELSE instruction" << std::endl; }
;

cond_instruction :
    IF '(' condition ')' { std::cout << "cond_instruction: IF '(' condition ')'" << std::endl; }  // {$$=$3;}
;

iteration_instruction :
    WHILE '(' condition ')' instruction                             { std::cout << "iteration_instruction: WHILE '(' condition ')' instruction" << std::endl; }  // Handle while loop
  | DO instruction WHILE '(' condition ')'                          { std::cout << "iteration_instruction: DO instruction WHILE '(' condition ')'" << std::endl; }
  | FOR '(' assignment ';' condition ';' assignment ')' instruction { std::cout << "iteration_instruction: FOR '(' assignment ';' condition ';' assignment ')' instruction" << std::endl; }
;

jump_instruction:
    RETURN expression ';' { std::cout << "jump_instruction: RETURN expression ';'" << std::endl; }
;

condition :
    expression comparison_operator expression { std::cout << "condition: expression comparison_operator expression" << std::endl; }
;

comparison_operator :
    EQUAL        { std::cout << "comparison_operator: EQUAL"        << std::endl; }  // { $$.entier = EQUAL;        }  // equal
  | NEQUAL       { std::cout << "comparison_operator: NEQUAL"       << std::endl; }  // { $$.entier = NEQUAL;       }  // not-equal
  | LESS         { std::cout << "comparison_operator: LESS"         << std::endl; }  // { $$.entier = LESS;         }  // less-than
  | GREATER      { std::cout << "comparison_operator: GREATER"      << std::endl; }  // { $$.entier = GREATER;      }  // greater-than
  | LESSEQUAL    { std::cout << "comparison_operator: LESSEQUAL"    << std::endl; }  // { $$.entier = LESSEQUAL;    }  // less-than-or-equal
  | GREATEREQUAL { std::cout << "comparison_operator: GREATEREQUAL" << std::endl; }  // { $$.entier = GREATEREQUAL; }  // greater-than-or-equal
;

expression :
    additive_expression                       { std::cout << "expression: additive_expression" << std::endl; }  // {$$=$1;}
  | expression SHIFTLEFT additive_expression  { std::cout << "expression: expression SHIFTLEFT additive_expression" << std::endl; }  //  Compute expression
  | expression SHIFTRIGHT additive_expression { std::cout << "expression: expression SHIFTRIGHT additive_expression" << std::endl; }  // Compute expression
;

additive_expression :
    multiplicative_expression                           { std::cout << "additive_expression: multiplicative_expression" << std::endl; }  // {$$=$1;}
  | additive_expression PLUS multiplicative_expression  { std::cout << "additive_expression: additive_expression PLUS multiplicative_expression" << std::endl; }  // Compute expression
  | additive_expression MINUS multiplicative_expression { std::cout << "additive_expression: additive_expression MINUS multiplicative_expression" << std::endl; }  // Compute expression
;

multiplicative_expression :
    unary_expression                                    { std::cout << "multiplicative_expression: unary_expression" << std::endl; }  // {$$=$1;}
  | multiplicative_expression MULTIPLY unary_expression { std::cout << "multiplicative_expression: multiplicative_expression MULTIPLY unary_expression" << std::endl; }
  | multiplicative_expression DIVIDE unary_expression   { std::cout << "multiplicative_expression: multiplicative_expression DIVIDE unary_expression" << std::endl; }
  | multiplicative_expression MODULO unary_expression   { std::cout << "multiplicative_expression: multiplicative_expression MODULO unary_expression" << std::endl; }
;

unary_expression:
    postfix_expression     { std::cout << "unary_expression: postfix_expression" << std::endl; }  // {$$=$1;}
  | MINUS unary_expression { std::cout << "unary_expression: MINUS unary_expression" << std::endl; }
;

postfix_expression :
    primary_expression                     { std::cout << "postfix_expression: primary_expression" << std::endl; }  // {$$=$1;}
  | IDENT '(' argument_expression_list ')' { std::cout << "postfix_expression: IDENT '(' argument_expression_list ')'" << std::endl; }
  | IDENT '(' ')'                          { std::cout << "postfix_expression: IDENT '(' ')'" << std::endl; }
;

argument_expression_list:
    expression                              { std::cout << "argument_expression_list: expression" << std::endl; }
  | argument_expression_list ',' expression { std::cout << "argument_expression_list: argument_expression_list ',' expression" << std::endl; }
;

primary_expression :
    IDENT              { std::cout << "primary_expression: IDENT" << std::endl; }
  | CONST_INT          { std::cout << "primary_expression: CONST_INT" << std::endl; }
  | CONST_STRING       { std::cout << "primary_expression: CONST_STRING" << std::endl; }
  | '(' expression ')' { std::cout << "primary_expression: '(' expression ')'" << std::endl; }
;


%%


void parser::Parser::error (const location_type& loc, const std::string& message) {
    std::cerr << "ERROR: at " << loc << " - " << message << std::endl;
}
