#include <fstream>
#include <string>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch/catch.hpp"

TEST_CASE( "Test Cases for Compiler", "[factorial]" ) {

	SECTION("Test Case 1") {
	    // Read the correct result
		std::ifstream ifs_correct("test/testCase1.correct_result");
		// std::string correct_result;
		// correct_result.assign( (std::istreambuf_iterator<char>(ifs_correct)),
		//  					   (std::istreambuf_iterator<char>()) );
		// std::cout << correct_result << std::endl;

		std::string corret_line;


		// Read the compiled result
		std::ifstream ifs_compiled("test/testCase1.compiled_result");
		std::string compiled_line;

		// std::string compiled_result;
		// compiled_result.assign( (std::istreambuf_iterator<char>(ifs_compiled)),
		// 					   (std::istreambuf_iterator<char>()) );
		// std::cout << compiled_result << std::endl;

		// CHECK(correct_result.compare(compiled_result) == 1);	

		while(std::getline(ifs_correct, corret_line)) {
			bool has_compiled_line = std::getline(ifs_compiled, compiled_line);
			REQUIRE(has_compiled_line);
			REQUIRE(corret_line.compare(compiled_line) == 0);
		}
	}

}
