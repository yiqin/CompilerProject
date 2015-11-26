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
System Requirement:
* flex 2.5.39
* bison 3.0.2
* clang 3.6.2-1
* llvm 3.6.2
* Operating system is 64 bit


===============================================================================

Currently the Preprocessor and the Scanner are separate executables. They both
take input from stdin and output to stdout. To run one into the other, just pipe
the output from "preprocessor" into the input of "compiler".

    $> ./bin/preprocessor < foo.c | ./bin/compiler

Notes about the Scanner implementation:
    1) The Lexer has no notion of type, so when building the symbol table, it
       leaves that field blank.

===============================================================================
How to run LLVM

```
clang -S -emit-llvm sample.c -o sample.ll
opt-3.6 -S sample.ll
llc-3.6 -O3 sample.ll -march=x86-64 -o sample-x86-64.s
gcc sample-x86-64.s -o sample-x86-64
./sample-x86-64
```
opt-3.6 is to optimize the code. 

If you want to try LLVM online, [http://ellcc.org/demo/index.cgi](http://ellcc.org/demo/index.cgi).

===============================================================================
To Do:
- [ ] Print variables when printf is called. printf is a special function.
- [ ]  Assume all integers are ```%32```.
===============================================================================

REFERENCES:

* The usage pattern of Bison in this project (and also by extension Flex) is based
on the example found at [https://panthema.net/2007/flex-bison-cpp-example/](https://panthema.net/2007/flex-bison-cpp-example/).

* Quick Start LLVM Tutorial
at [https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/](https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/).