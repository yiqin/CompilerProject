%{

#include "scanner.hpp"

#include <algorithm>
#include <string>

// WARNING: USE OF A DIRTY MACRO
//          The parser will by default try to use a global yylex function. We
//          want it to use the lex function of its member reference "scanner".
//          So #define yylex to refer to this.
#define yylex scanner.lex


%}

// Generate a header file.
%defines

// Use the c++ skeleton file.
%skeleton "lalr1.cc"

// Name to use for the generated parser class.
%define parser_class_name {Parser}

// Use this namespace to contain all the generated classes related to Parser,
// including Parser itself.
%define api.namespace {parser}

// Pass by reference an instance of the scanner to the parse() function. This
// allows for the parser to drive the scanner.
%parse-param { scanner::Scanner& scanner }
%parse-param { Symbol_Table::Ptr symbol_table }

%code requires {
    #include "ast.hpp"
    #include "symbol_table.hpp"
    namespace scanner { class Scanner; }
}

// Track locations within source file (stdin) for error reporting.
%locations
%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("<stdin>");
}

%code {
    parser::Function::Ptr last_function_;
    bool new_function_definition_;
}

%define api.value.type variant


%token INT
%token STRING
%token <int>         CONST_INT
%token <std::string> CONST_STRING

%token <std::string> IDENT

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

%type <Symbol_List> declaration
%type <Symbol_List> declarator_list
%type <Symbol_List> parameter_list
%type <Symbol::Ptr> declarator
%type <Function::Ptr> function_declarator
%type <Symbol::Ptr> parameter_declaration
%type <Type> type

%type <ast::Expression::Ptr> primary_expression
%type <ast::Expression::Ptr> postfix_expression
%type <ast::Expression::Ptr> unary_expression
%type <ast::Expression::Ptr> multiplicative_expression
%type <ast::Expression::Ptr> additive_expression
%type <ast::Expression::Ptr> assignment
%type <ast::Expression::Ptr> expression

// FIXME: must be condition. Changing to Expression breaks the compiler somehow.
%type <ast::Comparison_Operation> comparison_operator
%type <ast::Condition::Ptr> condition
%type <ast::Condition::Ptr> cond_instruction

%type <ast::Instruction::Ptr> instruction
%type <ast::Instruction::Ptr> compound_instruction
%type <ast::Instruction::Ptr> expression_instruction
%type <ast::Instruction::Ptr> iteration_instruction
%type <ast::Instruction::Ptr> select_instruction
%type <ast::Instruction::Ptr> jump_instruction

%type <std::vector<ast::Expression::Ptr>> argument_expression_list
%type <std::vector<ast::Instruction::Ptr>> instruction_list


%%


program :
    external_declaration         {
        /* std::cout << "program: external_declaration" << std::endl; */
    }
  | program external_declaration {
        /* std::cout << "program: program external_declaration" << std::endl; */
    }
;

external_declaration :
    declaration         {
        /* Nothing to do. Symbol was already added to symbol_table. */
    }
  | EXTERN declaration  {
        for (auto& symbol : $2) {
            symbol->set(Symbol::Attribute::EXTERN);
            // std::cout << "- flag '" << symbol->name() << "' as extern" << std::endl;
        }
    }
  | function_definition {
        /* std::cout << "external_declaration: function_definition" << std::endl; */
    }
;

function_definition :
    type function_declarator decl_glb_fct compound_instruction {
        // Set function return type.
        auto function = $2;
        function->type($1);

        // A function can be declared before it is defined.
        if (symbol_table->is_visible(function->name())) {
            // Retrieve previously declared function.
            auto declared_func = std::dynamic_pointer_cast<Function>(
                symbol_table->lookup(function->name()));

            // Verify the return type. (type-checking)
            if (function->type() != declared_func->type()) {
                std::ostringstream oss;
                oss
                    << "Return type of function definition, "
                    << (function->type() == Type::INT ? "int" : "string")
                    << ", does not match declaration, "
                    << (declared_func->type() == Type::INT ? "int" : "string")
                    << "."
                    ;
                throw syntax_error(@$, oss.str());
            }

            // Rest of type checking is done when processing `decl_glb_fct`.
        }

        // Or it can be declared for the first time here.
        else {
            symbol_table->add($2->name(), Symbol::Ptr($2));
            // $2->print_semantic_action();
        }

        // Emit LLVM IR.
        // emit constant strings
        // emit function header
        // emit function body
        // emit function trailer
    }
;

decl_glb_fct : {
        std::cout << "- define function '" << last_function_->name() << "'" << std::endl;

        Function::Ptr function = last_function_;

        // A function can be declared before it is defined.
        if (symbol_table->is_visible(function->name())) {
            // Retrieve previously declared function.
            auto declared_func = std::dynamic_pointer_cast<Function>(
                symbol_table->lookup(function->name()));

            // Verify that previously declared identifier is a function.
            if (not declared_func) {
                throw syntax_error(@$, "Attempt to define function with name of "
                    "variable '" + function->name() + "'.");
            }

            // Return type checked when processing `function_definition`.

            // Verify the argument list. (type-checking)
            if (function->argument_list().size() != declared_func->argument_list().size()) {
                throw syntax_error(@$, "Signature mismatch between function definition and declaration.");
            }
            auto pair = std::mismatch(
                std::begin(function->argument_list()), std::end(function->argument_list()),
                std::begin(declared_func->argument_list()),
                [] (Symbol::Ptr a, Symbol::Ptr b) { return a->type() == b->type(); }
            );
            if (pair.first != std::end(function->argument_list())) {
                throw syntax_error(@$, "Signature mismatch between function definition and declaration.");
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

        // NOTE: compound_instruction will have not yet already taken care of
        //       creating a new symbol-table for the new scope. However, it will
        //       not add the function parameters to this scope. We must do that
        //       here.
        for (auto& symbol : function->argument_list()) {
            symbol_table->add(symbol->name(), symbol);
        }
    }
;

declaration :
    type declarator_list ';' {
        $$ = $2;
        for (auto& symbol : $$) {
            symbol->type($1);

            if (symbol_table->is_in_this_scope(symbol->name())) {
                throw syntax_error(@$, "Duplicate declaration of symbol '" +
                    symbol->name() + "'");
            }

            symbol_table->add(symbol->name(), symbol);
            // symbol->print_semantic_action();  // TODO: Remove for Part 3.
        }


    }
;

type :
    INT    {
        $$ = Type::INT;
    }
  | STRING {
        $$ = Type::STRING;
    }
;

declarator_list :
    declarator                     {
        $$.push_back($1);
    }
  | declarator_list ',' declarator {
        $$ = std::move($1);
        $$.push_back($3);
    }
;

declaration_list :
    declaration                   {
        // Nothing to do. Semantic action of "declaration" handles symbol creation.
    }
  | declaration_list declaration  {
        // Nothing to do. Semantic action of "declaration" handles symbol creation.
    }
;

declarator :
    IDENT               {
        $$ = std::make_shared<Symbol>(std::move($1));
    }
  | function_declarator {
        $$ = $1;
    }
;

function_declarator :
    IDENT '(' ')'                 {
        $$ = std::make_shared<Function>(std::move($1));
        last_function_ = $$;
    }
  | IDENT '(' parameter_list ')'  {
        $$ = std::make_shared<Function>(std::move($1));
        last_function_ = $$;
        for (auto& symbol : $3) {
            symbol->set(Symbol::Attribute::FUNCTION_PARAM);
            std::static_pointer_cast<Function>($$)->argument_list().push_back(symbol);
        }
    }
;

parameter_list :
    parameter_declaration                    {
        $$.push_back($1);
    }
  | parameter_list ',' parameter_declaration {
        $$ = std::move($1);
        $$.push_back($3);
    }
;

parameter_declaration :
    type IDENT {
        $$ = std::make_shared<Symbol>(std::move($2));
        $$->type($1);
    }
;

instruction :
    ';'                    {
        /* std::cout << "instruction: ';'"                    << std::endl; */
        // No instruction to forward up. Nothing to do.
    }
  | compound_instruction   {
        /* std::cout << "instruction: compound_instruction"   << std::endl; */
        $$ = $1;
    }
  | expression_instruction {
        /* std::cout << "instruction: expression_instruction" << std::endl; */
        $$ = $1;
    }
  | iteration_instruction  {
        /* std::cout << "instruction: iteration_instruction"  << std::endl; */
        $$ = $1;
    }
  | select_instruction     {
        /* std::cout << "instruction: select_instruction"     << std::endl; */
        $$ = $1;
    }
  | jump_instruction       {
        /* std::cout << "instruction: jump_instruction"       << std::endl; */
        $$ = $1;
    }
;

expression_instruction :
    expression ';' {
        /* std::cout << "expression_instruction: expression ';'" << std::endl; */
        $$ = std::make_shared<ast::Expression_Instruction>($1);
    }
  | assignment ';' {
        /* std::cout << "expression_instruction: assignment ';'" << std::endl; */
        $$ = std::make_shared<ast::Expression_Instruction>($1);
    }
;

assignment :
    IDENT '=' expression {
        /* std::cout << "assignment: IDENT '=' expression" << std::endl; */
        // std::cout << "- assignment " << $1 << std::endl;

        if (not symbol_table->is_visible($1)) {
            throw syntax_error(@$, $1 + " is not defined.");
        }

        Symbol::Ptr symbol = symbol_table->lookup($1);
        if (symbol->type() != $3->type()) {
            std::string expression_str;
            if ($3->type() == Type::INT) {
                expression_str = "int";
            } else if ($3->type() == Type::STRING) {
                expression_str = "string";
            } else {
                expression_str = "expression is not defined.";
            }
            throw syntax_error(@$, "Type checking error. Assign " + symbol->type_str() + " to " + expression_str + ".");
        }

        auto variable = std::make_shared<ast::Variable>(symbol);
        $$ = std::make_shared<ast::Assignment>(variable, $3);
    }
;

compound_instruction :
    block_start declaration_list instruction_list block_end {
        /* std::cout << "compound_instruction: block_start declaration_list instruction_list block_end" << std::endl; */
        $$ = std::make_shared<ast::Compound_Instruction>(std::move($3));
    }
  | block_start declaration_list block_end {
        /* std::cout << "compound_instruction: block_start declaration_list block_end" << std::endl; */
        // No instructions to foward up. Nothing to do.
    }
  | block_start instruction_list block_end {
        /* std::cout << "compound_instruction: block_start instruction_list block_end" << std::endl; */
        $$ = std::make_shared<ast::Compound_Instruction>(std::move($2));
    }
  | block_start block_end {
        /* std::cout << "compound_instruction: block_start block_end" << std::endl; */
        // No instructions to forward up. Nothing to do.
    }
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
    instruction                  {
        /* std::cout << "instruction_list: instruction" << std::endl; */
        $$.push_back($1);
    }
  | instruction_list instruction {
        /* std::cout << "instruction_list: instruction_list instruction" << std::endl; */
        $$ = std::move($1);
        $$.push_back($2);
    }
;

select_instruction :
    cond_instruction instruction                  {
        /* std::cout << "select_instruction: cond_instruction instruction" << std::endl; */
        $$ = std::make_shared<ast::Cond_Instruction>($1, $2);
    }
  | cond_instruction instruction ELSE instruction {
        /* std::cout << "select_instruction: cond_instruction instruction ELSE instruction" << std::endl; */
        $$ = std::make_shared<ast::Cond_Instruction>($1, $2, $4);
    }
;

cond_instruction :
    IF '(' condition ')' {
        /* std::cout << "cond_instruction: IF '(' condition ')'" << std::endl; */
        $$ = $3;
    }
;

iteration_instruction :
    WHILE '(' condition ')' instruction                             {
        /* std::cout << "iteration_instruction: WHILE '(' condition ')' instruction" << std::endl; */
        $$ = std::make_shared<ast::While_Instruction>($3, $5);
    }
  | DO instruction WHILE '(' condition ')'                          {
        /* std::cout << "iteration_instruction: DO instruction WHILE '(' condition ')'" << std::endl; */
        $$ = std::make_shared<ast::Do_Instruction>($5, $2);
    }
  | FOR '(' assignment ';' condition ';' assignment ')' instruction {
        /* std::cout << "iteration_instruction: FOR '(' assignment ';' condition ';' assignment ')' instruction" << std::endl; */
        $$ = std::make_shared<ast::For_Instruction>($3, $5, $7, $9);

    }
;

jump_instruction:
    RETURN expression ';' {
        /* std::cout << "jump_instruction: RETURN expression ';'" << std::endl; */
        $$ = std::make_shared<ast::Return_Instruction>($2);
    }
;

condition :
    expression comparison_operator expression {
        /* std::cout << "condition: expression comparison_operator expression" << std::endl; */
        $$ = std::make_shared<ast::Condition>($1, $2, $3);
    }
;

comparison_operator :
    EQUAL        { /* std::cout << "comparison_operator: EQUAL"        << std::endl; */ $$ = ast::Comparison_Operation::EQUAL;                 }
  | NEQUAL       { /* std::cout << "comparison_operator: NEQUAL"       << std::endl; */ $$ = ast::Comparison_Operation::NOT_EQUAL;             }
  | LESS         { /* std::cout << "comparison_operator: LESS"         << std::endl; */ $$ = ast::Comparison_Operation::LESS_THAN;             }
  | GREATER      { /* std::cout << "comparison_operator: GREATER"      << std::endl; */ $$ = ast::Comparison_Operation::GREATER_THAN;          }
  | LESSEQUAL    { /* std::cout << "comparison_operator: LESSEQUAL"    << std::endl; */ $$ = ast::Comparison_Operation::LESS_THAN_OR_EQUAL;    }
  | GREATEREQUAL { /* std::cout << "comparison_operator: GREATEREQUAL" << std::endl; */ $$ = ast::Comparison_Operation::GREATER_THAN_OR_EQUAL; }
;

expression :
    additive_expression                       {
        /* std::cout << "expression: additive_expression" << std::endl; */
        $$ = $1;
    }
  | expression SHIFTLEFT additive_expression  {
        /* std::cout << "expression: expression SHIFTLEFT additive_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::LEFT_SHIFT, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '<<' operation.");
        }
    }
  | expression SHIFTRIGHT additive_expression {
        /* std::cout << "expression: expression SHIFTRIGHT additive_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::RIGHT_SHIFT, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '>>' operation.");
        }
    }
;

additive_expression :
    multiplicative_expression                           {
        /* std::cout << "additive_expression: multiplicative_expression" << std::endl; */
        $$ = $1;
    }
  | additive_expression PLUS multiplicative_expression  {
        /* std::cout << "additive_expression: additive_expression PLUS multiplicative_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::ADDITION, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '+' operation.");
        }
    }
  | additive_expression MINUS multiplicative_expression {
        /* std::cout << "additive_expression: additive_expression MINUS multiplicative_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::SUBTRACTION, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '-' operation.");
        }
    }
;

multiplicative_expression :
    unary_expression                                    {
        /* std::cout << "multiplicative_expression: unary_expression" << std::endl; */
        $$ = $1;
    }
  | multiplicative_expression MULTIPLY unary_expression {
        /* std::cout << "multiplicative_expression: multiplicative_expression MULTIPLY unary_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::MULTIPLICATION, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '*' operation.");
        }
    }
  | multiplicative_expression DIVIDE unary_expression   {
        /* std::cout << "multiplicative_expression: multiplicative_expression DIVIDE unary_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::DIVISION, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do '/' operation.");
        }
    }
  | multiplicative_expression MODULO unary_expression   {
        /* std::cout << "multiplicative_expression: multiplicative_expression MODULO unary_expression" << std::endl; */
        if ($1->type() == Type::INT and $3->type() == Type::INT) {
            $$ = std::make_shared<ast::Binary_Expression>(Type::INT, ast::Operation::MODULUS, $1, $3);
        } else {
            throw syntax_error(@$, "Only two integers can do modulo operation.");
        }
    }
;

unary_expression:
    postfix_expression     {
        /* std::cout << "unary_expression: postfix_expression" << std::endl; */
        $$ = $1;
    }
  | MINUS unary_expression {
        /* std::cout << "unary_expression: MINUS unary_expression" << std::endl; */
        if ($2->type() == Type::INT) {
            // $$ = std::make_shared<ast::Unary_Expression>(Type::INT, ast::Operation::SUBTRACTION, $2);
            $$ = std::make_shared<ast::Unary_Expression>($2);
        } else {
            throw syntax_error(@$, "Unary MINUS is not defined for this type.");
        }
    }
;

postfix_expression :
    primary_expression                     {
        /* std::cout << "postfix_expression: primary_expression" << std::endl; */
        $$ = $1;
    }
  | IDENT '(' argument_expression_list ')' {
        /* std::cout << "postfix_expression: IDENT '(' argument_expression_list ')'" << std::endl; */
        /* std::cout << "postfix_expression: IDENT '(' ')'" << *$1 << std::endl; */
        if (not symbol_table->is_visible($1)) {
            throw syntax_error(@$, $1 + " is not defined");
        }

        // Check if IDENT is defined.
        auto symbol = symbol_table->lookup($1);
        if (not symbol) {
            throw syntax_error(@$, "Attempt to call function that is not defined '" + $1 + "'.");
        }

        // Check if IDENT is a function.
        auto declared_func = std::dynamic_pointer_cast<Function>(symbol);
        if (not declared_func) {
            throw syntax_error(@$, "'" + $1 + "' identifies a variable, not a function.");
        }

        // Verify the argument list. (type-checking)
        if ($3.size() != declared_func->argument_list().size()) {
            throw syntax_error(@$, "Signature mismatch between function definition and function call.");
        }
        auto pair = std::mismatch(
            std::begin($3), std::end($3),
            std::begin(declared_func->argument_list()),
            [] (ast::Expression::Ptr a, Symbol::Ptr b) { return a->type() == b->type(); }
        );
        if (pair.first != std::end($3)) {
            throw syntax_error(@$, "Signature mismatch between function definition and function call.");
        }

        $$ = std::make_shared<ast::Function_Call>(declared_func, $3);
    }
  | IDENT '(' ')'                          {
        /* std::cout << "postfix_expression: IDENT '(' ')'" << *$1 << std::endl; */
        if (not symbol_table->is_visible($1)) {
            throw syntax_error(@$, $1 + " is not defined");
        }

        // Check if IDENT is defined.
        auto symbol = symbol_table->lookup($1);
        if (not symbol) {
            throw syntax_error(@$, "Attempt to call function that is not defined '" + $1 + "'.");
        }

        // Check if IDENT is a function.
        auto declared_func = std::dynamic_pointer_cast<Function>(symbol);
        if (not declared_func) {
            throw syntax_error(@$, "'" + $1 + "' identifies a variable, not a function.");
        }

        // Verify the argument list. (type-checking)
        if (0 != declared_func->argument_list().size()) {
            throw syntax_error(@$, "Signature mismatch between function definition and function call.");
        }

        $$ = std::make_shared<ast::Function_Call>(declared_func);
    }
;

argument_expression_list:
    expression                              {
        /* std::cout << "argument_expression_list: expression" << std::endl; */
        $$.push_back($1);
    }
  | argument_expression_list ',' expression {
        /* std::cout << "argument_expression_list: argument_expression_list ',' expression" << std::endl; */
        $$ = std::move($1);
        $$.push_back($3);
  }
;

primary_expression :
    IDENT              {
        /* std::cout << "primary_expression: IDENT " << *$1 << std::endl; */
        if (not symbol_table->is_visible($1)) {
            throw syntax_error(@$, $1 + " is not defined");
        }

        // Check if IDENT is defined.
        auto symbol = symbol_table->lookup($1);
        if (not symbol) {
            throw syntax_error(@$, "Attempt to reference symbol that is not defined '" + $1 + "'.");
        }

        $$ = std::make_shared<ast::Variable>(symbol);
    }
  | CONST_INT          {
        /* std::cout << "primary_expression: CONST_INT " << $1 << std::endl; */
        $$ = std::make_shared<ast::Const_Integer>($1);
    }
  | CONST_STRING       {
        /* std::cout << "primary_expression: CONST_STRING " << $1 << std::endl; */
        $$ = std::make_shared<ast::Const_String>($1);
    }
  | '(' expression ')' {
        /* std::cout << "primary_expression: '(' expression ')'" << std::endl; */
        $$ = $2;
    }
;


%%


void parser::Parser::error (
    const parser::Parser::location_type& loc,
    const std::string& message
) {
    std::cerr << "ERROR: at " << loc << " - " << message << std::endl;
}
