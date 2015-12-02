
#include <iostream>

// NOTE: These two files must be included in this order.
#include "scanner.hpp"
#include "symbol_table.hpp"

#include "ast.hpp"
#include "llvm.hpp"
#include "parser.tab.hpp"

int main (int argc, char** argv) {
    scanner::Scanner scanner;
    parser::Symbol_Table::Ptr symbol_table =
        parser::Symbol_Table::construct("global scope", parser::location());
    llvm::LLVM_Generator llvm_generator(std::cout);
    llvm_generator.indentation("  ");
    parser::Parser parser(scanner, symbol_table, llvm_generator);

    parser.parse();

    // std::cout << std::endl << "SYMBOL TABLES" << std::endl << std::endl;
    // parser::Symbol_Table::print_tables();

    return 0;
}
