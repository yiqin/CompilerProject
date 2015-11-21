# CompilerProject
MPCS 51300: Compilers

This project will give you the first building block of your own CSTR compiler. By the
end of this project you will have a subset C-compiler generating the assembly code
of your choice. As we saw during the first parts of this class, a compiler is divided
into two main parts: the front-end and the back-end. In this initial project, we will
tackle the first one. You will use the language/the library of your choice to create a
scanner for the CSTR programming language. Your scanner will run through the
source program, recognizing CSTR tokens. For each token, your scanner will return
the right attributes and you will print the information on the stdout.
An attribute can be an identifier name, a numerical value, a string, an operator, a
symbol.

This project will give you the grammar that you will use for this compiler.


===============================================================================

Currently the Preprocessor and the Scanner are separate executables. They both
take input from stdin and output to stdout. To run one into the other, just pipe
the output from "preprocessor" into the input of "compiler".

    $> ./bin/preprocessor < foo.c | ./bin/compiler

Notes about the Scanner implementation:
    1) The Lexer has no notion of type, so when building the symbol table, it
       leaves that field blank.

===============================================================================

REFERENCES:

* The usage pattern of Bison in this project (and also by extension Flex) is based
on the example found at [https://panthema.net/2007/flex-bison-cpp-example/](https://panthema.net/2007/flex-bison-cpp-example/).

* Quick Start LLVM Tutorial
at [https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/](https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/).