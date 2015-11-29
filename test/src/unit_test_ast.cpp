
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

        std::string expected_output = std::string("define i32 @foo(i32 %a, i32 %b) #0");

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
        // %0 = alloca i32, align 4
        // store i32 0, i32* %0
        // %1 = load i32* %0, align 4
        // ret i32 %1

        std::string expected_output = std::string("%0 = alloca i32, align 4\n");
        expected_output += "store i32 0, i32* %0\n";
        expected_output += "%1 = load i32* %0, align 4\n";
        expected_output += "ret i32 %1\n";

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
        //
        // %3 = load i32* %a, align 4
        // ret i32 %3

        std::string expected_output = std::string("%3 = load i32* %a, align 4\n");
        expected_output += "ret i32 %3\n";

        // Prepare
        // Expression - Variable
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("a"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // Return_Instruction
        ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(variable);

        REQUIRE (return_instruction->emit_llvm_ir() == expected_output);
    }


    SECTION ("Return expression: return a+1-2;") {
        // return a+1-2;

        std::string expected_output;

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
        // %1 = alloca i32, align 4
        // store i32 450, i32* %1
        // %3 = load i32* %1, align 4
        // store i32 %3, i32* %i, align 4

        // This may be wrong.
        std::string expected_output = std::string("%1 = alloca i32, align 4\n");
        expected_output += "store i32 450, i32* %1\n";
        expected_output += "%3 = load i32* %1, align 4\n";
        expected_output += "store i32 %3, i32* %i, align 4\n";

        std::string output = "";

        // rhs
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // lhs
        ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(450));

        ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(const_integer->type(), variable, const_integer);
        output += assignment->emit_llvm_ir();

        REQUIRE ( output == expected_output );
    }


    SECTION ("For Loop \n- condition i<=10\n") {
        //   for ( i = -10; i <= 10; i = i+1 )
        //      printd(i);

        std::string expected_output = std::string("");

        // initialization
        // i = -10
        // create i
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        // -10
        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));

        // i = -10 Assignment
        ast::Assignment::Ptr initialization = std::make_shared<ast::Assignment>(const_integer_1->type(), variable, const_integer_1);


        // condition
        // i <= 10
        // 10
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));

        // i <= 10 Condition
        ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN_OR_EQUAL, const_integer_2);

        std::string expected_output_1 = std::string("; <label>:0\n");
        expected_output_1 += "%5 = load i32* %0, align 4\n";
        expected_output_1 += "%6 = load i32* %3, align 4\n";
        expected_output_1 += "%4 = icmp sle i32 %5, %6\n";

        // REQUIRE (condition->emit_llvm_ir() == expected_output_1);

        // increment - Assignment with expression.
        // i = i + 1
        // 1
        ast::Const_Integer::Ptr const_integer_3 = std::make_shared<ast::Const_Integer>(std::move(1));
        // i + 1
        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, variable, const_integer_3);

        // i = i + 1
        ast::Assignment::Ptr increment = std::make_shared<ast::Assignment>(variable->type(), variable, add_expression);


        // instruction
        // It's the body of the loop. Only single instruction, not multiply lines.
        ast::Instruction::Ptr instruction = std::make_shared<ast::Instruction>();

        ast::For_Instruction::Ptr for_instruction = std::make_shared<ast::For_Instruction>(initialization, condition, increment, instruction);

        REQUIRE (for_instruction->emit_llvm_ir() == expected_output);
    }


    SECTION ("Binary_Expression: 1+2") {
        // 1+2;

        std::string expected_output = std::string("");
        std::string output;

        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
        symbol->type(parser::Type::INT);
        ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);

        ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(1));
        ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));

        ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, const_integer_1, const_integer_2);
        output += add_expression->emit_llvm_ir();

        REQUIRE (output == "");
    }


    SECTION ("Condi_Instruction") {
    
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
        // REQUIRE (function_definition->emit_llvm_ir() == "" );
        
        
        parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("s"));
        symbol->type(parser::Type::STRING);
    
        ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);
        output += symbol_declarator->emit_llvm_ir();
        REQUIRE (output == "" );
    
	}

	
	
	SECTION ("Assignment viariable with const_string: i = \"hello\"") {

    }


}
