
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
		
		std::string expected_output = std::string("%i = alloca i32, align 4\n");
		
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		
		ast::Symbol_Declarator::Ptr symbol_declarator = std::make_shared<ast::Symbol_Declarator>(symbol);
		
		REQUIRE(symbol_declarator->emit_llvm_ir() == expected_output);
	}

	
	SECTION ("Declare a list of symbols: int i, j;") {
		// int i, j;
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
		
		ast::reset();
		
		std::string expected_output = std::string("%0 = alloca i32, align 4\n");
		expected_output += "store i32 0, i32* %0\n";
		expected_output += "%1 = load i32* %0, align 4\n";
		expected_output += "ret i32 %1\n";
		
		std::string output = "";
		
		// Expression - Const_Integer
		ast::Const_Integer::Ptr const_integer = std::make_shared<ast::Const_Integer>(std::move(0));
		output += const_integer->emit_llvm_ir();
		
		// Return_Instruction
		ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(const_integer);
		output += return_instruction->emit_llvm_ir();
		
		REQUIRE ( output == expected_output );
	}
	
	
	SECTION ("Return variable: return a;") {
		// return a;
		// 
		// %1 = load i32* %0, align 4
		// ret i32 %1
		
		ast::reset();
		
		std::string expected_output = std::string("%1 = load i32* %0, align 4\n");
		expected_output += "ret i32 %1\n";
		
		// Prepare
		// Expression - Variable
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("a"));
		symbol->type(parser::Type::INT);
		ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
		
		// Return_Instruction
		ast::Return_Instruction::Ptr return_instruction = std::make_shared<ast::Return_Instruction>(variable);
		
		REQUIRE (return_instruction->emit_llvm_ir() == expected_output);		
	}
	
	
	// SECTION ("Return expression: return a+1-2;") {
		// return a+1-2;
		
		// Expression
		
		
		// Return_Instruction
	// }
	

	SECTION ("Assignment viariable with const_int: i = 450;") {
		// i = 450;
		//
		// %1 = alloca i32, align 4
		// store i32 450, i32* %1
		// %3 = load i32* %1, align 4
		// store i32 %3, i32* %i, align 4
		
		ast::reset();
		
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
		output += const_integer->emit_llvm_ir();
		
		ast::Assignment::Ptr assignment = std::make_shared<ast::Assignment>(const_integer->type(), variable, const_integer);
		output += assignment->emit_llvm_ir(); 
		
		REQUIRE ( output == expected_output );
	}


	SECTION ("For Loop") {
		//   for ( i = -10; i <= 10; j = 2 )
    	//		printd(i);
		
		ast::reset();
		
		std::string expected_output = std::string("");
		
		// initialization
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
		
		// register 1
		ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(-10));
		
		// register 2
		ast::Assignment::Ptr initialization = std::make_shared<ast::Assignment>(const_integer_1->type(), variable, const_integer_1);
		
		// register 3
		ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(10));
		
		// condition
		// register 4
		ast::Condition::Ptr condition = std::make_shared<ast::Condition>(variable, ast::Comparison_Operation::LESS_THAN_OR_EQUAL, const_integer_2);
		
		std::string expected_output_1 = std::string("; <label>:0\n");
		expected_output_1 += "%5 = load i32* %0, align 4\n";
		expected_output_1 += "%6 = load i32* %3, align 4\n";
		expected_output_1 += "%4 = icmp sle i32 %5, %6\n";
		
		REQUIRE (condition->emit_llvm_ir() == expected_output_1);
		
		// increment - Assignment with expression.
		// 
		
		// We can't continue until we setup binary expression.
		
		// instruction
		// It's the body of the loop. Only single instruction, not multiply lines.
		
		
		REQUIRE (initialization->emit_llvm_ir() == expected_output);
	}
	

	SECTION ("Binary_Expression") {
		// i = 1+2;
		
		ast::reset();
		
		parser::Symbol::Ptr symbol = std::make_shared<parser::Symbol>(std::move("i"));
		symbol->type(parser::Type::INT);
		ast::Variable::Ptr variable = std::make_shared<ast::Variable>(symbol);
		
		ast::Const_Integer::Ptr const_integer_1 = std::make_shared<ast::Const_Integer>(std::move(1));
		ast::Const_Integer::Ptr const_integer_2 = std::make_shared<ast::Const_Integer>(std::move(2));
		
		ast::Binary_Expression::Ptr add_expression = std::make_shared<ast::Binary_Expression>(parser::Type::INT, ast::Operation::ADDITION, const_integer_1, const_integer_2);
				
        REQUIRE (add_expression->emit_llvm_ir() == "");
    }
	

}