#include <fstream>
#include <string>
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch/catch.hpp"

TEST_CASE("Test Case for Compiler") {

	SECTION("Test Case 1") {
		// Read the correct result
		std::ifstream ifs_correct("testCase1.correct_result");
		std::string correct_result;
		correct_result.assign( (std::istreambuf_iterator<char>(ifs_correct)),
		 					   (std::istreambuf_iterator<char>()) );
		// std::cout << correct_result << std::endl;

		// Read the compiled result
		std::ifstream ifs_compiled("testCase1.compiled_result");
		std::string compiled_result;
		compiled_result.assign( (std::istreambuf_iterator<char>(ifs_compiled)),
		 					   (std::istreambuf_iterator<char>()) );
		// std::cout << compiled_result << std::endl;

		REQUIRE(correct_result.compare(compiled_result) == 0);
	}



}