
#include "catch.hpp"
#include "symbol.hpp"
#include "ast.hpp"
#include <sstream>
#include <vector>

#include <iostream>


TEST_CASE ("Abstract Syntax Tree") { 

	SECTION ("Symbol Declaration: int i;") {
		// int i;
		// %i = alloca i32, align 4
		
		std::string expected_output = std::string("%i = alloca i32, align 4");
		
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		
		ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);
		
		REQUIRE(symbol_declarator->emit_llvm_ir() == expected_output);
	}

	
	SECTION ("Declare a list of symbols: int i, j;") {
		// int i, j;
		// %i = alloca i32, align 4
		// %j = alloca i32, align 4
		
		std::string expected_output = std::string("%i = alloca i32, align 4");
		expected_output += "\n";
		expected_output += "%j = alloca i32, align 4";
		
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
		// ret i32 0
		
		std::string expected_output = std::string("ret i32 0");
		
		// Expression - Const_Integer
		ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(0));
		
		// Return_Instruction
		ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(const_integer);
		
		REQUIRE (return_instruction->emit_llvm_ir() == expected_output);
	}
	
	
	SECTION ("Return variable: return a;") {
		// return a;
		// %3 = load i32* %a, align 4
		// ret i32 %3
		
		// %3 is the next register
		std::string expected_output = std::string("%3 = load i32* %a, align 4");
		expected_output += "\n";
		expected_output += "ret i32 %3";
		
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
		
		// Expression
		
		
		// Return_Instruction
	}
	

	SECTION ("Assignment viariable with const_int: i = 450;") {
		// i = 450;
		// store i32 450, i32* %i, align 4
		
		std::string expected_output = std::string("store i32 450, i32* %i, align 4");
		
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		
		ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
		ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(450));
		
		ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(const_integer->type(), variable, const_integer);
		
		REQUIRE (assignment->emit_llvm_ir() == expected_output);
	}


	SECTION ("For Loop") {
		//   for ( i = -10; i <= 10; i = i+1 )
    	//		printd(i);
		
		ast::register_number = 0;
		
		std::string expected_output = std::string("");
		
		// initialization
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
		
		ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
		
		ast::Assignment::Ptr initialization = std::make_shared<ast::Assignment>(const_integer_1->type(), variable, const_integer_1);
		
		// condition
		ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
		
		ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN_OR_EQUAL, const_integer_2);
		
		REQUIRE (condition->emit_llvm_ir() == "....");
		
		// increment - Assignment with expression.
		
		
		
		// instruction
		// It's the body of the loop. Only single instruction, not multiply lines.
		
		
		// 
		
		REQUIRE (initialization->emit_llvm_ir() == expected_output);
	}
	

	SECTION ("Binary_Expression") {
        REQUIRE (1   == 1);
    }
	

}