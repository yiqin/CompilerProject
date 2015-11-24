
#include "catch.hpp"
#include "symbol.hpp"
#include "ast.hpp"
#include <sstream>

#include <iostream>


TEST_CASE ("Abstract Syntax Tree") { 

	SECTION ("Assignment i = 450;") {
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

	SECTION ("Binary_Expression") {
		
		
		
        REQUIRE (1   == 1);
    }
	

}