
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
    llvm::Register::id_factory_.reset();
    llvm::String::id_factory_.reset();
}


TEST_CASE ("Abstract Syntax Tree") {
    reset_ids();

    SECTION ("Symbol Declaration: int i;") {
        // int i;
        // %i = alloca i32, align 4

        std::string expected_output = std::string("%i = alloca i32, align 4\n");

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);

        ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);

        REQUIRE(symbol_declarator->emit_llvm_ir() == expected_output);
    }
	
	SECTION ("Symbol Declaration: string s;") {
    // string s;
    // %s = alloca i8*, align 8

    std::string expected_output = std::string("%s = alloca i8*, align 8\n");
    
    parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
    symbol->type(parser::Type::STRING);
    
    ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);
    
    REQUIRE(symbol_declarator->emit_llvm_ir() == expected_output);    
	}

	
	SECTION ("Declare a list of symbols: string s, t;") {
    // string s, t;
    // %s = alloca i8*, align 4
    // %t = alloca i8*, align 4
    
        std::string expected_output = std::string("%s = alloca i8*, align 8\n");
        expected_output += "%t = alloca i8*, align 8\n";
        
        std::string str1 = "s";
        parser::Symbol::Ptr symbol1 = std::make_shared<parser::Symbol>(std::move(str1));
        symbol1->type(parser::Type::STRING);
        
        std::string str2 = "t";
        parser::Symbol::Ptr symbol2 = std::make_shared<parser::Symbol>(std::move(str2));
        symbol2->type(parser::Type::STRING);
        
        parser::Symbol_List symbol_list;
        symbol_list.push_back(symbol1);
        symbol_list.push_back(symbol2);
        
        ast::Declaration::Ptr declaration = std::make_shared<ast::Declaration>(symbol_list);
        
        REQUIRE (declaration->emit_llvm_ir() == expected_output);
	}
	
    SECTION ("Declare a list of symbols: string s, t;") {
        // string s, t;
        // %i = alloca i32, align 4
        // %j = alloca i32, align 4

        std::string expected_output = std::string("%i = alloca i32, align 4\n");
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

        ast::Declaration::Ptr declaration = std::make_shared<ast::Declaration>(symbol_list);

        REQUIRE (declaration->emit_llvm_ir() == expected_output);
    }

    SECTION ("Declare function: int foo(int a, int b)") {
        // int foo(int a, int b)
        // define i32 @foo(i32 %a, i32 %b) #0

        std::string expected_output = std::string("define i32 @foo(i32 %a, i32 %b) #0\n");

        parser::Type type = parser::Type::INT;
        // function_declarator includes the argument list
        parser::Function::Ptr function_declarator = std::make_shared<parser::Function>(std::move("foo"));

        parser::Symbol::Ptr argument1 = std::make_shared<parser::Symbol>(std::move("a"));
        argument1->type(parser::Type::INT);
        function_declarator->argument_list().push_back(argument1);

        parser::Symbol::Ptr argument2 = std::make_shared<parser::Symbol>(std::move("b"));
        argument2->type(parser::Type::INT);
        function_declarator->argument_list().push_back(argument2);

        ast::Function_Definition::Ptr function_definition = std::make_shared<ast::Function_Definition>(type, function_declarator);
        REQUIRE (function_definition->emit_llvm_ir() == expected_output);
    }


    SECTION ("Return const_int: return 0;") {
        // return 0;
        //
        // %P.0 = alloca i32, align 4
        // store i32 0, i32* %P.0
        // %V.1 = load i32* %P.0, align 4
        // ret i32 %1

        std::string expected_output = std::string("%P.0 = alloca i32, align 4\n");
        expected_output += "store i32 0, i32* %P.0\n";
        expected_output += "%V.1 = load i32* %P.0, align 4\n";
        expected_output += "ret i32 %V.1\n";

        std::string output = "";

        // Expression - Const_Integer
        ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(0));

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(const_integer);
        output += return_instruction->emit_llvm_ir();

        REQUIRE ( output == expected_output );
    }


    SECTION ("Return variable: return a;") {
        // return a;
        // a is either INT type or STRING type.

        // INT
        // %V.1 = load i32* %a, align 4
        // ret i32 %V.1
        std::string expected_output_1 = std::string("%V.1 = load i32* %a, align 4\n");
        expected_output_1 += "ret i32 %V.1\n";

        // Prepare
        // Expression - Variable
        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("a"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction_1 = std::make_shared<ast::Return_Instruction>(variable_1);

        REQUIRE (return_instruction_1->emit_llvm_ir() == expected_output_1);
        
        
        // STRING
        // %V.3 = load i8** %a, align 8
        // ret i8* %V.3
        std::string expected_output_2 = std::string("%V.3 = load i8** %a, align 8\n");
        expected_output_2 += "ret i8* %V.3\n";

        // Prepare
        // Expression - Variable
        parser::Symbol::Ptr symbol_2 = std::make_shared<parser::Symbol>(std::move("a"));
        symbol_2->type(parser::Type::STRING);
        ast::Variable::Ptr variable_2 = std::make_shared<ast::Variable>(symbol_2);

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction_2 = std::make_shared<ast::Return_Instruction>(variable_2);

        REQUIRE (return_instruction_2->emit_llvm_ir() == expected_output_2);
    }


    SECTION ("Return expression: return a+1-2;") {
        // return a+1-2;

        std::string expected_output;
        
        expected_output += "%V.7 = load i32* %a, align 4\n";
        expected_output += "%P.1 = alloca i32, align 4\n";
        expected_output += "store i32 1, i32* %P.1\n";
        expected_output += "%V.8 = load i32* %P.1, align 4\n";
        expected_output += "%V.9 = add i32 %V.7, %V.8\n";
        
        expected_output += "%P.3 = alloca i32, align 4\n";
        expected_output += "store i32 %V.9, i32* %P.3\n";
        expected_output += "%V.6 = load i32* %P.3, align 4\n";
        expected_output += "%P.2 = alloca i32, align 4\n";
        expected_output += "store i32 2, i32* %P.2\n";
        expected_output += "%V.10 = load i32* %P.2, align 4\n";
        expected_output += "%V.11 = sub i32 %V.6, %V.10\n";
        
        expected_output += "%P.4 = alloca i32, align 4\n";
        expected_output += "store i32 %V.11, i32* %P.4\n";
        expected_output += "%V.5 = load i32* %P.4, align 4\n";
        expected_output += "ret i32 %V.5\n";

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

        REQUIRE (return_instruction->emit_llvm_ir() == expected_output);
    }


    SECTION ("Assignment viariable with const_int: i = 450;") {
        // i = 450;
        //
        // %P.1 = alloca i32, align 4
        // store i32 450, i32* %P.1
        // %V.3 = load i32* %P.1, align 4
        // store i32 %V.3, i32* %i

        std::string expected_output = std::string("%P.1 = alloca i32, align 4\n");
        expected_output += "store i32 450, i32* %P.1\n";
        expected_output += "%V.3 = load i32* %P.1, align 4\n";
        expected_output += "store i32 %V.3, i32* %i\n";

        std::string output = "";

        // rhs
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // lhs
        ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(450));

        ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(variable, const_integer);
        output += assignment->emit_llvm_ir();

        REQUIRE ( output == expected_output );
    }
    
    
    SECTION ("Const String") {
        std::string expected_output_1 = "@.str_0 = private unnamed_addr constant [12 x i8] c\"hello world\\00\", align 1\n";
        ast::Const_String::Ptr const_string_1 = std::make_shared<ast::Const_String>(std::string("hello world"));
        REQUIRE (const_string_1->declare_llvm_ir() == expected_output_1 );
        
        std::string expected_output_2 = "@.str_1 = private unnamed_addr constant [6 x i8] c\"hello\\00\", align 1\n";
        ast::Const_String::Ptr const_string_2 = std::make_shared<ast::Const_String>(std::string("hello"));
        REQUIRE (const_string_2->declare_llvm_ir() == expected_output_2 );        
    }
    
    SECTION ("Assignment viariable with const_string: s = \"hello world\";") {
        // s = "hello";
        //
        //
        //
        std::string expected_output = "%P.1 = alloca i8*, align 8\n";
        expected_output += "store i8* getelementptr inbounds ([12 x i8]* @.str_0, i32 0, i32 0), i8** %P.1, align 8\n";
        expected_output += "%V.3 = load i8** %P.1, align 8\n";
        expected_output += "store i8* %V.3, i8** %s\n";
        
        // rhs
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
        symbol->type(parser::Type::STRING);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
        // lhs
        ast::Const_String::Ptr const_string = std::make_shared<ast::Const_String>(std::string("hello world"));
        
        ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(variable, const_string);
        
        REQUIRE (assignment->emit_llvm_ir() == expected_output );
    }

    
    SECTION ("For Loop") {
        //   for ( i = -10; i <= 10; i = i+1 )
        //      // printd(i); empty instruction now

        std::string expected_output = 
            "\n"
            "; For_Instruction\n"
            "\n"
            "%P.1 = alloca i32, align 4\n"
            "store i32 -10, i32* %P.1\n"
            "%V.9 = load i32* %P.1, align 4\n"
            "store i32 %V.9, i32* %i\n"
            "br label %Label_0\n"
            "\n"
            "Label_0:\n"
            "%V.10 = load i32* %i, align 4\n"
            "%P.3 = alloca i32, align 4\n"
            "store i32 10, i32* %P.3\n"
            "%V.11 = load i32* %P.3, align 4\n"
            "%V.5 = icmp sle i32 %V.10, %V.11\n"
            "br i1 %V.5, label %Label_1, label %Label_3\n"
            "\n"
            "Label_1:\n"
            "; /undefine Expression - Node Class/ \n"
            "br label %Label_2\n"
            "\n"
            "Label_2:\n"
            "%V.13 = load i32* %i, align 4\n"
            "%P.6 = alloca i32, align 4\n"
            "store i32 1, i32* %P.6\n"
            "%V.14 = load i32* %P.6, align 4\n"
            "%V.15 = add i32 %V.13, %V.14\n"
            "%P.7 = alloca i32, align 4\n"
            "store i32 %V.15, i32* %P.7\n"
            "%V.12 = load i32* %P.7, align 4\n"
            "store i32 %V.12, i32* %i\n"
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
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN_OR_EQUAL, const_integer_2);
        
        // std::string expected_output_2 = 
        //     "%V.10 = load i32* %i, align 4\n"
        //     "%P.3 = alloca i32, align 4\n"
        //     "store i32 10, i32* %P.3\n"
        //     "%V.11 = load i32* %P.3, align 4\n"
        //     "%V.5 = icmp sle i32 %V.10, %V.11\n"
        //     ;
        // REQUIRE ( condition->emit_llvm_ir() == expected_output_2 );        
        
        // increment - Assignment with expression.
        // i = i + 1
        // 1
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));
        // i + 1
        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_3);

        // i = i + 1
        ast::Assignment::Ptr increment = std::make_shared<ast::Assignment>(variable, add_expression);

        // std::string expected_output_3 = 
        //     "%V.13 = load i32* %i, align 4\n"
        //     "%P.6 = alloca i32, align 4\n"
        //     "store i32 1, i32* %P.6\n"
        //     "%V.14 = load i32* %P.6, align 4\n"
        //     "%V.15 = add i32 %V.13, %V.14\n"
        //     "%P.7 = alloca i32, align 4\n"
        //     "store i32 %V.15, i32* %P.7\n"
        //     "%V.12 = load i32* %P.7, align 4\n"
        //     "store i32 %V.12, i32* %i\n"
        //     ;
        // REQUIRE ( increment->emit_llvm_ir() == expected_output_3 );

        // instruction
        // It's the body of the loop. Only single instruction, not multiply lines.
        ast::Instruction::Ptr instruction = std::make_shared<ast::Instruction>();

        // for_instruction
        ast::For_Instruction::Ptr for_instruction = std::make_shared<ast::For_Instruction>(initialization, condition, increment, instruction);

        REQUIRE (for_instruction->emit_llvm_ir() == expected_output);
    }


    SECTION ("Binary_Expression: 1+2") {
        // 1+2;

        std::string expected_output;
        expected_output += "%P.1 = alloca i32, align 4\n";
        expected_output += "store i32 1, i32* %P.1\n";
        expected_output += "%V.4 = load i32* %P.1, align 4\n";
        expected_output += "%P.2 = alloca i32, align 4\n";
        expected_output += "store i32 2, i32* %P.2\n";
        expected_output += "%V.5 = load i32* %P.2, align 4\n";
        expected_output += "%V.6 = add i32 %V.4, %V.5\n";
        expected_output += "%P.3 = alloca i32, align 4\n";
        expected_output += "store i32 %V.6, i32* %P.3\n";
        

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, const_integer_1, const_integer_2);

        REQUIRE (add_expression->emit_llvm_ir() == expected_output);
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
        
        std::string output;
    
        parser::Type type = parser::Type::STRING;
        // function_declarator includes the argument list
        parser::Function::Ptr function_declarator = std::make_shared<parser::Function>(std::move("foo"));
    
        // parser::Symbol::Ptr argument1 = std::make_shared<parser::Symbol>(std::move("a"));
        // argument1->type(parser::Type::INT);
        // function_declarator->argument_list().push_back(argument1);
    
        // parser::Symbol::Ptr argument2 = std::make_shared<parser::Symbol>(std::move("b"));
        // argument2->type(parser::Type::INT);
        // function_declarator->argument_list().push_back(argument2);    
    
        ast::Function_Definition::Ptr function_definition = std::make_shared<ast::Function_Definition>(type, function_declarator);
        output += function_definition->emit_llvm_ir();
        
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
        symbol->type(parser::Type::STRING);
    
        ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);
        output += symbol_declarator->emit_llvm_ir();
        
        
        // Unfinished....
        
        // REQUIRE (output == "" );
	}

    SECTION ( "Cond_Instruction" ) {
        // if (-10 == 10)
        //   i = 1;
        // else 
        //   i = -1;
        
        std::string expected_output;
        
        
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_4 = std::make_shared<ast::Const_Integer>(std::move(-1));
        
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(const_integer_1, ast::Comparison_Operation::EQUAL, const_integer_2);
        
        parser::Symbol::Ptr symbol_1 = std::make_shared<parser::Symbol>(std::move("i"));
        symbol_1->type(parser::Type::INT);
        ast::Variable::Ptr variable_1 = std::make_shared<ast::Variable>(symbol_1);
        
        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable_1, const_integer_3);
        ast::Assignment::Ptr assignment_2 = std::make_shared<ast::Assignment>(variable_1, const_integer_4);
        
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);
        ast::Expression_Instruction::Ptr instruction_2 = std::make_shared<ast::Expression_Instruction>(assignment_2);
        
        ast::Cond_Instruction::Ptr cond_instruction_1 = std::make_shared<ast::Cond_Instruction>(condition, instruction_1, instruction_2);
        
        REQUIRE (cond_instruction_1->emit_llvm_ir() == expected_output);
        
        ast::Cond_Instruction::Ptr cond_instruction_2 = std::make_shared<ast::Cond_Instruction>(condition, instruction_1);        
        REQUIRE (cond_instruction_2->emit_llvm_ir() == expected_output);
    }
    
    SECTION ( "While_Instruction" ) {
        // while ( i < 10 ) {
        //   i = i+2;
        // }
        
        std::string expected_output;

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
        
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));
        
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN, const_integer_1);
        
        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_2);
        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable, add_expression);
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);
        
        ast::While_Instruction::Ptr while_instruction = std::make_shared<ast::While_Instruction>(condition, instruction_1);
        
        REQUIRE (while_instruction->emit_llvm_ir() == expected_output);
    }
    
    SECTION ( "Do_Instruction" ) {
        // do {
        //   i = i + 2
        // } while ( i < 10 );
        
        std::string expected_output;

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
        
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));
        
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN, const_integer_1);
        
        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_2);
        ast::Assignment::Ptr assignment_1 = std::make_shared<ast::Assignment>(variable, add_expression);
        ast::Expression_Instruction::Ptr instruction_1 = std::make_shared<ast::Expression_Instruction>(assignment_1);
        
        ast::Do_Instruction::Ptr do_instruction = std::make_shared<ast::Do_Instruction>(condition, instruction_1);
        
        REQUIRE (do_instruction->emit_llvm_ir() == expected_output);
    }
    
    SECTION ( "Compound_Instruction" ) {
        // i = 1;
        // i = -1;
        
        std::string expected_output;
        
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
        
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(const_integer_1, ast::Comparison_Operation::EQUAL, const_integer_2);
        
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
        
        REQUIRE (compound_instruction->emit_llvm_ir()==expected_output);
    }
    
}
