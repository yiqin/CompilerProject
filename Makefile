# directories
BINDIR   = bin
BUILDDIR = build
SRCDIR   = src
TESTDIR  = test

# build tools
CXX   = g++
LLC   = llc
FLEX  = flex
BISON = bison

MKDIR = mkdir -p
RM    = rm -rf

ifdef COMSPEC
	FLEX  = C:/dev-tools/win_flex_bison-2.5.5/win_flex.exe
	BISON = C:/dev-tools/win_flex_bison-2.5.5/win_bison.exe

	MKDIR = C:/MinGW/msys/1.0/bin/mkdir.exe -p
	RM    = C:/MinGW/msys/1.0/bin/rm.exe -rf
endif

CPPFLAGS = -I $(SRCDIR)
CXXFLAGS = -std=gnu++11
LDFLAGS  =
LDLIBS   =

BINARIES = compiler preprocessor
BINARIES := $(addprefix $(BINDIR)/,$(BINARIES))
SRCS = $(wildcard $(SRCDIR)/*.cpp)
LEXS = $(wildcard $(SRCDIR)/*.lex)
BSNS = $(wildcard $(SRCDIR)/*.yy)
TEST_SRCS = $(wildcard $(TESTDIR)/$(SRCDIR)/*.cpp)


# BUILD ACTIONS

all: $(BINARIES) $(BUILDDIR)/string_lib.o

clean:
	$(RM) $(BINDIR) $(BUILDDIR) \
		$(patsubst $(SRCDIR)/%.lex,$(SRCDIR)/%.yy.cpp, $(LEXS)) \
		$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.cpp,$(BSNS)) \
		$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.hpp,$(BSNS)) \
		$(SRCDIR)/location.hh $(SRCDIR)/position.hh $(SRCDIR)/stack.hh \
		$(TESTDIR)/$(BINDIR) $(TESTDIR)/$(BUILDDIR) \
		$(TESTDIR)/test_cases.cstr $(TESTDIR)/test_cases.gcc

tests: unit_tests integration_tests

.SECONDARY:
.PHONY: all clean tests unit_tests integration_tests


# TEST ACTIONS

# test:
# 	./$(BINDIR)/preprocessor | ./$(BINDIR)/compiler <$(TEST)/testCase1.c >$(TEST)/testCase1.compiled_result; \
# 	sdiff -s $(TEST)/testCase1.correct_result $(TEST)/testCase1.compiled_result >$(TEST)/testCase1.test_report; \

# test_catch:
# 	./$(BINDIR)/preprocessor | ./$(BINDIR)/compiler <$(TEST)/testCase1.c >$(TEST)/testCase1.compiled_result; \
# 	$(CXX) $(CXXFLAGS) -I src $(TEST)/testCatch.cpp -o $(TEST)/testCatch; \
# 	./$(TEST)/testCatch >$(TEST)/test_report.txt

unit_tests: $(TESTDIR)/$(BINDIR)/unit_tests
	$<

integration_tests: $(BINDIR)/compiler $(BUILDDIR)/string_lib.o
	bash test/integration_tests.sh

#
# llc-3.6 -O3 sample.ll -march=x86-64 -o sample-x86-64.s
# gcc sample-x86-64.s -o sample-x86-64
#
#


# SPECIFY BINARY DEPENDENCIES
#   All dependencies will automatically be built using the rule patterns
#   specified below.

$(BINDIR)/compiler: $(BUILDDIR)/compiler_main.o \
	$(BUILDDIR)/scanner.yy.o $(BUILDDIR)/parser.tab.o \
	$(BUILDDIR)/symbol_table.o $(BUILDDIR)/ast.o $(BUILDDIR)/llvm.o
$(BINDIR)/preprocessor: $(BUILDDIR)/preprocessor.yy.o $(BUILDDIR)/macro.o

$(TESTDIR)/$(BINDIR)/unit_tests: $(TESTDIR)/$(BUILDDIR)/unit_test_main.o \
	$(TESTDIR)/$(BUILDDIR)/unit_test_scanner.o $(BUILDDIR)/scanner.yy.o \
	$(TESTDIR)/$(BUILDDIR)/unit_test_ast.o $(BUILDDIR)/ast.o $(BUILDDIR)/llvm.o


# SPECIFY SPECIAL DEPENDENCIES
#   Most dependency sequences should be derrived automatically. The only ones
#   that aren't are dependencies on generated header files. Those are specified
#   here.

$(SRCDIR)/compiler_main.cpp: $(SRCDIR)/parser.tab.hpp
$(SRCDIR)/scanner.yy.cpp: $(SRCDIR)/parser.tab.hpp
$(SRCDIR)/symbol_table.cpp: $(SRCDIR)/location.hh

$(TESTDIR)/$(SRCDIR)/unit_test_scanner.cpp: $(SRCDIR)/parser.tab.hpp


# RULE PATTERNS

# link
$(BINDIR)/% $(TESTDIR)/$(BINDIR)/%:
	@$(MKDIR) $(@D)
	$(CXX) -o $@ $(LDFLAGS) $(LDLIBS) $^

# compile/assemble
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -o $@ -c $(CPPFLAGS) $(CXXFLAGS) $<
$(TESTDIR)/$(BUILDDIR)/%.o: $(TESTDIR)/$(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -o $@ -c $(CPPFLAGS) $(CXXFLAGS) $<
$(BUILDDIR)/%.s: $(SRCDIR)/%.ll
	@$(MKDIR) $(@D)
	$(LLC) -o $@ $<

# Flex
$(SRCDIR)/%.yy.cpp: $(SRCDIR)/%.lex
	@$(MKDIR) $(@D)
	$(FLEX) -o $@ $<

# Bison
$(SRCDIR)/%.tab.cpp $(SRCDIR)/%.tab.hpp: $(SRCDIR)/%.yy
	@$(MKDIR) $(@D)
	$(BISON) -o $(SRCDIR)/$*.tab.cpp $<

# dependencies
$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -c -MT "$(BUILDDIR)/$*.o $(BUILDDIR)/$*.d" -MM -MP $(CPPFLAGS) $(CXXFLAGS) $^ > $@
$(TESTDIR)/$(BUILDDIR)/%.d: $(TESTDIR)/$(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -c -MT "$(TESTDIR)/$(BUILDDIR)/$*.o $(TESTDIR)/$(BUILDDIR)/$*.d" -MM -MP $(CPPFLAGS) $(CXXFLAGS) $^ > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.d,$(SRCS))
-include $(patsubst $(SRCDIR)/%.lex,$(BUILDDIR)/%.yy.d,$(LEXS))
-include $(patsubst $(TESTDIR)/$(SRCDIR)/%.cpp,$(TESTDIR)/$(BUILDDIR)/%.d,$(TEST_SRCS))
endif
