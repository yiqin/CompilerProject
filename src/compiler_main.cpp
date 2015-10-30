
// NOTE: These two files must be included in this order.
#include "scanner.hpp"
#include "parser.tab.hpp"


int main (int argc, char** argv) {
    scanner::Scanner scanner;
    parser::Parser parser(scanner);

    parser.parse();

    return 0;
}
