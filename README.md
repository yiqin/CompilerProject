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

    $> ./bin/preprocessor < foo.c | ./bin/compiler >foo.ll

```foo.ll``` is in LLVM assembly IR.

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

Due to the different version of LLVM compilers, sample.ll will be different.

===============================================================================
To Do:
- [ ] Print variables when printf is called. printf is a special function.
- [ ] Assume all integers are ```%32```.

===============================================================================

REFERENCES:

* The usage pattern of Bison in this project (and also by extension Flex) is based
on the example found at [https://panthema.net/2007/flex-bison-cpp-example/](https://panthema.net/2007/flex-bison-cpp-example/).

* Quick Start LLVM Tutorial
at [https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/](https://idea.popcount.org/2013-07-24-ir-is-better-than-assembly/).

* Run LLVM Assembly in Brower
at [https://kripken.github.io/llvm.js/demo.html](https://kripken.github.io/llvm.js/demo.html).

* Implement a String Data Type in LLVM?
at [http://stackoverflow.com/questions/1061753/how-can-i-implement-a-string-data-type-in-llvm](http://stackoverflow.com/questions/1061753/how-can-i-implement-a-string-data-type-in-llvm).

* Local variables in LLVM IR
at [http://stackoverflow.com/questions/24605063/assigning-literal-value-to-local-variable-in-llvm-ir](http://stackoverflow.com/questions/24605063/assigning-literal-value-to-local-variable-in-llvm-ir).

* The Often Misunderstood GEP Instruction
at [http://llvm.org/docs/GetElementPtr.html#what-is-the-first-index-of-the-gep-instruction](http://llvm.org/docs/GetElementPtr.html#what-is-the-first-index-of-the-gep-instruction)

* L/R shift operations in C. logical vs arithmetic
at [http://stackoverflow.com/questions/7622/shift-operator-in-c](http://stackoverflow.com/questions/7622/shift-operator-in-c)
Based on the discussion on this page, we chose to implement them as arithmetic shifts. (The test file "test/test_cases/rsh.c" doesn't seem to test for one or the other though.)

* Reference for colored output in bash script.
at [http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux](http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux)
