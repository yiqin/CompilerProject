%{

#include "scanner.hpp"

#include <algorithm>
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
%parse-param { Symbol_Table::Ptr symbol_table }

%code requires {
    #include "symbol_table.hpp"
    namespace scanner { class Scanner; }
}

// Track locations within source file (stdin) for error reporting.
%locations
%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("<stdin>");
}

%code {
    parser::Function* last_function_;
    bool new_function_definition_;
}


%union {
    int           integer_value;
    std::string*  string_value;
    Type          type_value;
    Symbol*       symbol;
    Symbol_List*  symbol_list;
    Symbol_Table* symbol_table;
}

%destructor { delete $$; } <string_value>
%destructor { delete $$; } <symbol_list>

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


// non-terminal type specifications

%type <symbol_list> declaration
%type <symbol_list> declarator_list
%type <symbol_list> parameter_list
%type <symbol> declarator
%type <symbol> function_declarator
%type <symbol> parameter_declaration
%type <type_value> type


%%


program :
    external_declaration         { std::cout << "program: external_declaration" << std::endl; }
  | program external_declaration { std::cout << "program: program external_declaration" << std::endl; }
;

external_declaration :
    declaration { /* Nothing to do. Symbol was already added to symbol_table. */ }
  | EXTERN declaration {
        for (auto& symbol : *$2) {
            symbol->set(Symbol::Attribute::EXTERN);
            std::cout << "- flag '" << symbol->name() << "' as extern" << std::endl;
        }
    }
  | function_definition { std::cout << "external_declaration: function_definition" << std::endl; }
;

function_definition :
    type function_declarator decl_glb_fct compound_instruction {
        // Check function return type.
        auto function = static_cast<Function*>($2);
        function->type($1);

        // A function can be declared before it is defined.
        if (symbol_table->is_visible(function->name())) {
            auto declared_func = std::dynamic_pointer_cast<Function>(
                symbol_table->lookup(function->name()));
            if (!declared_func) {
                // TODO (Emery): Move this semantic error to a dedicated function.
                std::cerr
                    << "ERROR: Attempt to define function with name of variable '"
                    << function->name() << "'." << std::endl
                    ;
            } else if (function->type() != declared_func->type()) {
                // Verify the return type. (type-checking)
                std::cerr
                    << "ERROR: Return type of function definition does not match declaration."
                    << std::endl
                    ;
            }
        }

        // Or it can be declared for the first time here.
        else {
            symbol_table->add($2->name(), Symbol::Ptr($2));
            $2->print_semantic_action();
        }
    }
;

decl_glb_fct : {
        std::cout << "- define function '" << last_function_->name() << "'" << std::endl;

        Function* function = last_function_;
        bool error = false;

        // A function can be declared before it is defined.
        if (symbol_table->is_visible(function->name())) {
            auto declared_func = std::dynamic_pointer_cast<Function>(
                symbol_table->lookup(function->name()));
            if (!declared_func) {
                // TODO (Emery): Move this semantic error to a dedicated function.
                std::cerr
                    << "ERROR: Attempt to define function with name of variable '"
                    << function->name() << "'." << std::endl
                    ;
                error = true;
            } else {
                // Verify the argument list. (type-checking)
                if (function->argument_list().size() != declared_func->argument_list().size()) {
                    std::cerr
                        << "ERROR: Signature mismatch between function definition and declaration."
                        << std::endl
                        ;
                    error = true;
                } else {
                    auto pair = std::mismatch(
                        std::begin(function->argument_list()), std::end(function->argument_list()),
                        std::begin(declared_func->argument_list()),
                        [] (Symbol::Ptr a, Symbol::Ptr b) { return a->type() == b->type(); }
                    );
                    if (pair.first != std::end(function->argument_list())) {
                        std::cerr
                            << "ERROR: Signature mismatch between function definition and declaration."
                            << std::endl
                            ;
                        error = true;
                    }
                }
            }
        }

        // Raise a flag to let "block_start" know that the symbol-table has
        // already been created.
        new_function_definition_ = true;

        // Create the new symbol-table for this function.
        symbol_table = Symbol_Table::construct(
            "function scope - " + function->name(),
            @$,
            symbol_table
        );

        // NOTE: compound_instruction will have not yet already taken care of creating a
        //       new symbol-table for the new scope. However, it will not add
        //       the function parameters to this scope. We must do that here.
        for (auto& symbol : function->argument_list()) {
            symbol_table->add(symbol->name(), symbol);
        }
    }
;

declaration :
    type declarator_list ';' {
        $$ = $2;
        for (auto& symbol : *$$) {
            symbol->type($1);

            if (symbol_table->is_in_this_scope(symbol->name())) {
                // TODO (Emery): Move this to some sort of centralized function
                //               for handling semantic errors.
                std::cerr
                    << "ERROR: Duplicate declaration of symbol '"
                    << symbol->name() << "'" << std::endl
                    ;
            } else {
                symbol_table->add(symbol->name(), symbol);
                symbol->print_semantic_action();
            }
        }
    }
;

type :
    INT    { $$ = Type::INT; }
  | STRING { $$ = Type::STRING; }
;

declarator_list :
    declarator {
        $$ = new Symbol_List;
        $$->push_back(Symbol::Ptr($1));
    }
  | declarator_list ',' declarator {
        $$ = $1;
        $$->push_back(Symbol::Ptr($3));
    }
;

declaration_list :
    declaration {
        // Nothing to do. Semantic action of "declaration" handles symbol creation.
    }
  | declaration_list declaration  {
        // Nothing to do. Semantic action of "declaration" handles symbol creation.
    }
;

declarator :
    IDENT { $$ = new Symbol(std::move(*$1)); }
  | function_declarator { $$ = $1; }
;

function_declarator :
    IDENT '(' ')' {
        last_function_ = new Function(std::move(*$1));
        $$ = last_function_;
    }
  | IDENT '(' parameter_list ')'  {
        last_function_ = new Function(std::move(*$1));
        $$ = last_function_;
        for (auto& symbol : *$3) {
            symbol->set(Symbol::Attribute::FUNCTION_PARAM);
            static_cast<Function*>($$)->argument_list().push_back(symbol);
        }
    }
;

parameter_list :
    parameter_declaration {
        $$ = new Symbol_List;
        $$->push_back(Symbol::Ptr($1));
    }
  | parameter_list ',' parameter_declaration {
        $$ = $1;
        $$->push_back(Symbol::Ptr($3));
    }
;

parameter_declaration :
    type IDENT { $$ = new Symbol(std::move(*$2)); $$->type($1); }
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
    '{' {
        if (not new_function_definition_) {
            symbol_table = Symbol_Table::construct("anonymous block", @$, symbol_table);
        } else {
            new_function_definition_ = false;
        }
    }
;

block_end :
    '}' {
        symbol_table->loc.end = @$.end;
        symbol_table = symbol_table->parent();
    }
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


void parser::Parser::error (
    const parser::Parser::location_type& loc,
    const std::string& message
) {
    std::cerr << "ERROR: at " << loc << " - " << message << std::endl;
}
