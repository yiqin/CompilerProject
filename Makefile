# directories
BINDIR   = bin
BUILDDIR = build
SRCDIR   = src
TESTDIR  = test

# build tools
CXX   = g++
FLEX  = flex
BISON = bison

MKDIR = mkdir -p
MV    = mv -f
RM    = rm -rf

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

all: $(BINARIES)

clean:
	$(RM) $(BINDIR) $(BUILDDIR) \
		$(patsubst $(SRCDIR)/%.lex,$(SRCDIR)/%.yy.cpp, $(LEXS)) \
		$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.cpp,$(BSNS)) \
		$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.hpp,$(BSNS)) \
		$(SRCDIR)/location.hh $(SRCDIR)/position.hh $(SRCDIR)/stack.hh \
		$(TESTDIR)/$(BINDIR) $(TESTDIR)/$(BUILDDIR)

.SECONDARY:
.PHONY: all clean test test_catch


# TEST ACTIONS

# test:
# 	./$(BINDIR)/preprocessor | ./$(BINDIR)/compiler <$(TEST)/testCase1.c >$(TEST)/testCase1.compiled_result; \
# 	sdiff -s $(TEST)/testCase1.correct_result $(TEST)/testCase1.compiled_result >$(TEST)/testCase1.test_report; \

# test_catch:
# 	./$(BINDIR)/preprocessor | ./$(BINDIR)/compiler <$(TEST)/testCase1.c >$(TEST)/testCase1.compiled_result; \
# 	$(CXX) $(CXXFLAGS) -I src $(TEST)/testCatch.cpp -o $(TEST)/testCatch; \
# 	./$(TEST)/testCatch >$(TEST)/test_report.txt

unit_test: $(TESTDIR)/$(BINDIR)/unit_test
	$<


# SPECIFY BINARY DEPENDENCIES
#   All dependencies will automatically be built using the rule patterns
#   specified below.

$(BINDIR)/compiler: $(BUILDDIR)/compiler_main.o \
	$(BUILDDIR)/scanner.yy.o $(BUILDDIR)/parser.tab.o $(BUILDDIR)/symbol_table.o
$(BINDIR)/preprocessor: $(BUILDDIR)/preprocessor.yy.o $(BUILDDIR)/macro.o

$(TESTDIR)/$(BINDIR)/unit_test: $(TESTDIR)/$(BUILDDIR)/unit_test_main.o \
	$(TESTDIR)/$(BUILDDIR)/unit_test_scanner.o $(BUILDDIR)/scanner.yy.o


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
