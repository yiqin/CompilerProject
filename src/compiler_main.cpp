
#include <iostream>

// NOTE: These two files must be included in this order.
#include "scanner.hpp"
#include "symbol_table.hpp"
#include "parser.tab.hpp"

#include "ast.hpp"

int main (int argc, char** argv) {
    scanner::Scanner scanner;
    parser::Symbol_Table::Ptr symbol_table =
        parser::Symbol_Table::construct("global scope", parser::location());
    parser::Parser parser(scanner, symbol_table);

    parser.parse();

    std::cout << std::endl << "SYMBOL TABLES" << std::endl << std::endl;
    parser::Symbol_Table::print_tables();


    // TEMP: implement the building llvm IR
    // Test here.
    // TODO: build unit test
    


    std::cout << std::endl << "AST" << std::endl << std::endl;

    ast::Binary_Expression tmp = ast::Binary_Expression();
    tmp.char = "+";

    std::cout << tmp.build_llvm_ir() << std::endl;





    return 0;
}
