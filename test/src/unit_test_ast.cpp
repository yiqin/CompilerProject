
#include "catch.hpp"
#include "symbol.hpp"

#include <sstream>
#include <vector>

#include <iostream>

#define protected public
#define private public
#include "ast.hpp"
#include "llvm.hpp"
#undef protected
#undef private


void reset_ids () {
    llvm::Label::id_factory_.reset();
    // llvm::String::id_factory_.reset();
    // ast::Nonterminal::id_factory_.reset();
}


TEST_CASE ("Generate LLVM --from-- Abstract Syntax Tree") {
    reset_ids();
    std::string expected_output;
    std::ostringstream output_stream;

    llvm::LLVM_Generator generator(output_stream);

    SECTION ("Symbol Declaration: int i;") {
        // int i;
        // %i = alloca i32, align 4

        expected_output = std::string("%i = alloca i32, align 4\n");

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);

        ast::Declaration_List::Ptr declaration_list = std::make_shared<ast::Declaration_List>(parser::Symbol_List {symbol});

        declaration_list->emit_code(&generator);
        REQUIRE(output_stream.str() == expected_output);
    }

	SECTION ("Symbol Declaration: string s;") {
        // string s;
        // %s = alloca i8*, align 8

        expected_output = std::string("%s = alloca i8*, align 8\n");

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
        symbol->type(parser::Type::STRING);

        ast::Declaration_List::Ptr declaration_list = std::make_shared<ast::Declaration_List>(parser::Symbol_List {symbol});

        declaration_list->emit_code(&generator);
        REQUIRE(output_stream.str() == expected_output);
	}

    SECTION ("Declare a list of symbols: string s, t;") {
        // string s, t;
        // %i = alloca i32, align 4
        // %j = alloca i32, align 4

        expected_output = std::string("%i = alloca i32, align 4\n");
        expected_output += "%j = alloca i32, align 4\n";

        std::string str1 = "i";
        parser::Symbol::Ptr symbol1 = std::make_shared<parser::Symbol>(std::move(str1));
        symbol1->type(parser::Type::INT);

        std::string str2 = "j";
        parser::Symbol::Ptr symbol2 = std::make_shared<parser::Symbol>(std::move(str2));
        symbol2->type(parser::Type::INT);

        parser::Symbol_List symbol_list;
        symbol_list.push_back(symbol1);
        symbol_list.push_back(symbol2);

        ast::Declaration_List::Ptr declaration = std::make_shared<ast::Declaration_List>(symbol_list);

        declaration->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }

    SECTION ("Declare function: int foo(int a, int b)") {
        // int foo(int a, int b)
        // define i32 @foo(i32 %a, i32 %b)

        expected_output = std::string("declare i32 @foo(i32, i32)\n");

        parser::Type type = parser::Type::INT;
        // function_declarator includes the argument list
        parser::Function::Ptr function_declarator = std::make_shared<parser::Function>(std::move("foo"));

        parser::Symbol::Ptr argument1 = std::make_shared<parser::Symbol>(std::move("a"));
        argument1->type(parser::Type::INT);
        function_declarator->argument_list().push_back(argument1);

        parser::Symbol::Ptr argument2 = std::make_shared<parser::Symbol>(std::move("b"));
        argument2->type(parser::Type::INT);
        function_declarator->argument_list().push_back(argument2);

        ast::Function_Declaration::Ptr function_declaration = std::make_shared<ast::Function_Declaration>(type, function_declarator);
        function_declaration->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    SECTION ("Return const_int: return 0;") {
        // return 0;
        //
        // ret i32 0

        expected_output = std::string("ret i32 0\n");

        // Expression - Const_Integer
        ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(0));

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(const_integer);

        return_instruction->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    SECTION ("Return INT variable: return a;") {
        // return a;

        // INT
        // %V.1 = load i32* %a, align 4
        // ret i32 %V.1
        std::string expected_output_1 = "%a.1 = load i32* %a\nret i32 %a.1\n";

        // Prepare
        // Expression - Variable
        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("a"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction_1 = std::make_shared<ast::Return_Instruction>(variable_1);

        return_instruction_1->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output_1);
    }


    SECTION ("Binary_Expression of literal INTs: 1+2") {
        // 1+2;

        expected_output ="%tmp.0 = add i32 1, 2\n";

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, const_integer_1, const_integer_2);

        add_expression->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    // TODO(Emery): Come back to this.
    // SECTION ("Return STRING variable: return a;") {
    //     // STRING
    //     // %V.3 = load i8** %a, align 8
    //     // ret i8* %V.3
    //     std::string expected_output_2 = "ret i8* %a\n";

    //     // Prepare
    //     // Expression - Variable
    //     parser::Symbol::Ptr symbol_2 = std::make_shared<parser::Symbol>(std::move("a"));
    //     symbol_2->type(parser::Type::STRING);
    //     ast::Variable::Ptr variable_2 = std::make_shared<ast::Variable>(symbol_2);

    //     // Return_Instruction
    //     ast::Return_Instruction::Ptr return_instruction_2 = std::make_shared<ast::Return_Instruction>(variable_2);

    //     REQUIRE (return_instruction_2->emit_llvm_ir() == expected_output_2);
    // }


    SECTION ("Return expression: return a+1-2;") {
        // return a+1-2;

        expected_output =
            "%a.1 = load i32* %a\n"
            "%tmp.1 = add i32 %a.1, 1\n"
            "%tmp.0 = sub i32 %tmp.1, 2\n"
            "ret i32 %tmp.0\n"
            ;


        // Prepare
        // Expression - Variable
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("a"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_1);
        ast::Binary_Expression::Ptr minus_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::SUBTRACTION, add_expression, const_integer_2);

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(minus_expression);

        return_instruction->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    SECTION ("Assign variable with const_int: i = 450;") {
        // i = 450;
        //
        // %P.1 = alloca i32, align 4
        // store i32 450, i32* %P.1
        // %V.3 = load i32* %P.1, align 4
        // store i32 %V.3, i32* %i

        expected_output = "store i32 450, i32* %i\n";

        // rhs
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // lhs
        ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(450));

        ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(variable, const_integer);

        assignment->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    // TODO(Emery): Come back to this once you move these global specifications out of "parser.yy".
    // SECTION ("Const String") {
    //     std::string expected_output_1 = "@.str_0 = private unnamed_addr constant [12 x i8] c\"hello world\\00\", align 1\n";
    //     ast::Const_String::Ptr const_string_1 = std::make_shared<ast::Const_String>(std::string("hello world"));
    //     REQUIRE (const_string_1->declare_llvm_ir() == expected_output_1 );

    //     std::string expected_output_2 = "@.str_1 = private unnamed_addr constant [6 x i8] c\"hello\\00\", align 1\n";
    //     ast::Const_String::Ptr const_string_2 = std::make_shared<ast::Const_String>(std::string("hello"));
    //     REQUIRE (const_string_2->declare_llvm_ir() == expected_output_2 );
    // }

    // TODO(Emery): Come back to this.
    // SECTION ("Assignment viariable with const_string: s = \"hello world\";") {
    //     // s = "hello";
    //     //
    //     //
    //     //
    //     expected_output = "%P.1 = alloca i8*, align 8\n";
    //     expected_output += "store i8* getelementptr inbounds ([12 x i8]* @.str_0, i32 0, i32 0), i8** %P.1, align 8\n";
    //     expected_output += "%V.3 = load i8** %P.1, align 8\n";
    //     expected_output += "store i8* %V.3, i8** %s\n";

    //     // rhs
    //     parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
    //     symbol->type(parser::Type::STRING);
    //     ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
    //     // lhs
    //     ast::Const_String::Ptr const_string = std::make_shared<ast::Const_String>(std::string("hello world"));

    //     ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(variable, const_string);

    //     REQUIRE (assignment->emit_llvm_ir() == expected_output );
    // }


    SECTION ("For Loop") {
        //   for ( i = -10; i <= 10; i = i+1 )
        //      // printd(i); empty instruction now

        expected_output =
            "\n"
            "; For_Instruction\n"
            "\n"
            "store i32 -10, i32* %i\n"
            "br label %Label_0\n"
            "\n"
            "Label_0:\n"
            "%i.1 = load i32* %i\n"
            "%tmp.1 = icmp sle i32 %i.1, 10\n"
            "br i1 %tmp.1, label %Label_1, label %Label_3\n"
            "\n"
            "Label_1:\n"
            "br label %Label_2\n"
            "\n"
            "Label_2:\n"
            "%i.1 = load i32* %i\n"
            "%tmp.2 = add i32 %i.1, 1\n"
            "store i32 %tmp.2, i32* %i\n"
            "br label %Label_0\n"
            "\n"
            "Label_3:\n"
            ;


        // initialization
        // i = -10
        // create i
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // -10
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));

        // i = -10 Assignment
        ast::Assignment::Ptr initialization = std::make_shared<ast::Assignment>(variable, const_integer_1);

        // We are not going to test several expressions in for_intruction.
        // Registers are different.

        // std::string expected_output_1 =
        //     "%P.1 = alloca i32, align 4\n"
        //     "store i32 -10, i32* %P.1\n"
        //     "%V.9 = load i32* %P.1, align 4\n"
        //     "store i32 %V.9, i32* %i\n"
        //     ;
        // REQUIRE ( initialization->emit_llvm_ir() == expected_output_1 );

        // condition
        // i <= 10
        // 10
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));

        // i <= 10 Condition
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::LESS_THAN_OR_EQUAL, variable, const_integer_2);

        // increment - Assignment with expression.
        // i = i + 1
        // 1
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));
        // i + 1
        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_3);

        // i = i + 1
        ast::Assignment::Ptr increment = std::make_shared<ast::Assignment>(variable, add_expression);

        // instruction
        // It's the body of the loop. Only empty instruction, not multiply lines.
        ast::Instruction::Ptr instruction = std::make_shared<ast::Instruction>();

        // for_instruction
        ast::For_Instruction::Ptr for_instruction = std::make_shared<ast::For_Instruction>(initialization, condition, increment, instruction);

        for_instruction->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    // string type
    SECTION ("Function return string type \n  string func() {\n    string s;\n    s=\"hello\";\n    return s;\n  }") {
        //
        // string foo() {
        //   string s;
        //   s = "hello";
        //   return s;
        // }
        //
        //
        // define i8* @foo() #0 {
        //   %s = alloca i8*, align 8
        //
        //   %R.1 = alloca i8*, align 8
        //   store i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i8** %R.1, align 8
        //
        //   %V.2 = load i8** %R.1, align 8
        //   store i8* %V.2, i8** %s, align 8
        //
        //   %V.3 = load i8** %s, align 8
        //   ret i8* %V.3
        // }
        //
        // @.str = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
        //

        parser::Type type = parser::Type::STRING;
        // function_declarator includes the argument list
        parser::Function::Ptr function_declarator = std::make_shared<parser::Function>(std::move("foo"));

        // parser::Symbol::Ptr argument1 = std::make_shared<parser::Symbol>(std::move("a"));
        // argument1->type(parser::Type::INT);
        // function_declarator->argument_list().push_back(argument1);

        // parser::Symbol::Ptr argument2 = std::make_shared<parser::Symbol>(std::move("b"));
        // argument2->type(parser::Type::INT);
        // function_declarator->argument_list().push_back(argument2);

        ast::Function_Declaration::Ptr function_declaration = std::make_shared<ast::Function_Declaration>(type, function_declarator);
        function_declaration->emit_code(&generator);

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
        symbol->type(parser::Type::STRING);

        ast::Declaration_List::Ptr declaration_list = std::make_shared<ast::Declaration_List>(parser::Symbol_List {symbol});
        declaration_list->emit_code(&generator);


        // Unfinished....

        // REQUIRE (output == "" );
    }

    SECTION ( "Cond_Instruction if else" ) {
        // if (-10 == 10)
        //   i = 1;
        // else
        //   i = -1;

        expected_output =
            "\n"
            "; Cond_Instruction\n"
            "\n"
            "%tmp.0 = icmp eq i32 -10, 10\n"
            "br i1 %tmp.0, label %Label_0, label %Label_1\n"
            "\n"
            "Label_0:\n"
            "store i32 1, i32* %i\n"
            "br label %Label_2\n"
            "\n"
            "Label_1:\n"
            "store i32 -1, i32* %i\n"
            "br label %Label_2\n"
            "\n"
            "Label_2:\n"
            ;

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_4 = std::make_shared<ast::Const_Integer>(std::move(-1));

        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::EQUAL, const_integer_1, const_integer_2);

        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("i"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable_1, const_integer_3);
        ast::Assignment::Ptr assignment_2 = std::make_shared<ast::Assignment>(variable_1, const_integer_4);

        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);
        ast::Expression_Instruction::Ptr instruction_2 = std::make_shared<ast::Expression_Instruction>(assignment_2);

        ast::Cond_Instruction::Ptr cond_instruction_1 = std::make_shared<ast::Cond_Instruction>(condition, instruction_1, instruction_2);

        cond_instruction_1->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }

    SECTION ( "Cond_Instruction if" ) {
        // if (-10 == 10)
        //   i = 1;

        expected_output =
            "\n"
            "; Cond_Instruction\n"
            "\n"
            "%tmp.0 = icmp eq i32 -10, 10\n"
            "br i1 %tmp.0, label %Label_0, label %Label_1\n"
            "\n"
            "Label_0:\n"
            "store i32 1, i32* %i\n"
            "br label %Label_2\n"
            "\n"
            "Label_1:\n"
            "br label %Label_2\n"
            "\n"
            "Label_2:\n"
            ;

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));

        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::EQUAL, const_integer_1, const_integer_2);

        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("i"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable_1, const_integer_3);
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);

        ast::Cond_Instruction::Ptr cond_instruction_1 = std::make_shared<ast::Cond_Instruction>(condition, instruction_1);

        cond_instruction_1->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }


    SECTION ( "While_Instruction" ) {
        // while ( i < 10 ) {
        //   i = i+2;
        // }

        expected_output =
            "\n"
            "; While_Instruction\n"
            "\n"
            "br label %Label_0\n"
            "\n"
            "Label_0:\n"
            "%i.1 = load i32* %i\n"
            "%tmp.0 = icmp slt i32 %i.1, 10\n"
            "br i1 %tmp.0, label %Label_1, label %Label_2\n"
            "\n"
            "Label_1:\n"
            "%i.2 = load i32* %i\n"
            "%tmp.4 = add i32 %i.2, 2\n"
            "store i32 %tmp.4, i32* %i\n"
            "br label %Label_0\n"
            "\n"
            "Label_2:\n"
            ;

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::LESS_THAN, variable, const_integer_1);

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_2);
        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable, add_expression);
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);

        ast::While_Instruction::Ptr while_instruction = std::make_shared<ast::While_Instruction>(condition, instruction_1);

        while_instruction->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }

    SECTION ( "Do_Instruction" ) {
        // do {
        //   i = i + 2
        // } while ( i < 10 );

        expected_output =
            "\n"
            "; Do_Instruction\n"
            "\n"
            "br label %Label_0\n"
            "\n"
            "Label_0:\n"
            "%i.1 = load i32* %i\n"
            "%tmp.1 = add i32 %i.1, 2\n"
            "store i32 %tmp.1, i32* %i\n"
            "br label %Label_1\n"
            "\n"
            "Label_1:\n"
            "%i.2 = load i32* %i\n"
            "%tmp.4 = icmp slt i32 %i.2, 10\n"
            "br i1 %tmp.4, label %Label_0, label %Label_2\n"
            "\n"
            "Label_2:\n"
            ;

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::LESS_THAN, variable, const_integer_1);

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_2);
        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable, add_expression);
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);

        ast::Do_Instruction::Ptr do_instruction = std::make_shared<ast::Do_Instruction>(condition, instruction_1);

        do_instruction->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }

    SECTION ( "Compound_Instruction" ) {
        // i = 10;
        // i = -10;
        expected_output = "store i32 -10, i32* %i\n";
        expected_output += "store i32 10, i32* %i\n";

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));

        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(ast::Comparison_Operation::EQUAL, const_integer_1, const_integer_2);

        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("i"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable_1, const_integer_1);
        ast::Assignment::Ptr assignment_2 = std::make_shared<ast::Assignment>(variable_1, const_integer_2);

        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);
        ast::Expression_Instruction::Ptr instruction_2 = std::make_shared<ast::Expression_Instruction>(assignment_2);

        std::vector<ast::Instruction::Ptr> instruction_list;
        instruction_list.push_back(instruction_1);
        instruction_list.push_back(instruction_2);

        ast::Compound_Instruction::Ptr compound_instruction = std::make_shared<ast::Compound_Instruction>(std::move(instruction_list));

        compound_instruction->emit_code(&generator);
        REQUIRE (output_stream.str()==expected_output);
    }

    SECTION ( "Function_Call" ) {
        // int foo(int x, int y) {
        //   return x+y;
        // }
        //
        // i = foo(2, 4);
        //
        // The test case is foo(2, 4, "hello world");
        //

        expected_output = "%str.0 = getelementptr inbounds [12 x i8]* @str.0, i32 0, i32 0\n";
        expected_output += "%tmp.0 = call i32 ()* @foo(2, 4, %str.0)\n";

        parser::Function::Ptr function = std::make_shared<parser::Function>(std::move("foo"));
        function->type(parser::Type::INT);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(2));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(4));
        ast::Const_String::Ptr const_string_1 = std::make_shared<ast::Const_String>(std::string("hello world"));


        std::vector<ast::Expression::Ptr> argument_list;
        argument_list.push_back(const_integer_1);
        argument_list.push_back(const_integer_2);
        argument_list.push_back(const_string_1);

        ast::Function_Call::Ptr function_call = std::make_shared<ast::Function_Call>(function, argument_list);

        function_call->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }

    SECTION ( "Const_Integer" ) {
        // -a
        // a is int type.

        // A constant integer should have no prep-work and no inline execution.
        // It is only referenced by others.

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(2));

        const_integer_1->emit_code(&generator);
        REQUIRE (output_stream.str() == "");
        REQUIRE (generator.register_reference_.size() == 1);

        auto iter = std::begin(generator.register_reference_);
        REQUIRE (iter->first == const_integer_1);
        REQUIRE (iter->second == "2");
    }

    SECTION ( "Unary_Expression" ) {
        // -a
        // a is int type.

        expected_output =
            "%a.1 = load i32* %a\n"
            "%tmp.0 = sub i32 0, %a.1\n"
            ;

        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("a"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        ast::Unary_Expression::Ptr unary_expression = std::make_shared<ast::Unary_Expression>(variable_1);

        unary_expression->emit_code(&generator);
        REQUIRE (output_stream.str() == expected_output);
    }
}
