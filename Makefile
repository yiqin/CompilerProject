# directories
BINDIR   = bin
BUILDDIR = build
SRCDIR   = src

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


# BUILD ACTIONS

all: $(BINARIES)

clean:
	$(RM) $(BINDIR) $(BUILDDIR) \
	$(patsubst $(SRCDIR)/%.lex,$(SRCDIR)/%.yy.cpp, $(LEXS)) \
	$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.cpp,$(BSNS)) \
	$(patsubst $(SRCDIR)/%.yy, $(SRCDIR)/%.tab.hpp,$(BSNS)) \
	$(SRCDIR)/location.hh $(SRCDIR)/position.hh $(SRCDIR)/stack.hh

.SECONDARY:
.PHONY: all clean


# SPECIFY BINARY DEPENDENCIES
#   All dependencies will automatically be built using the rule patterns
#   specified below.

$(BINDIR)/compiler: $(BUILDDIR)/compiler_main.o \
	$(BUILDDIR)/scanner.yy.o $(BUILDDIR)/parser.tab.o
$(BINDIR)/preprocessor: $(BUILDDIR)/preprocessor.yy.o $(BUILDDIR)/macro.o


# SPECIFY SPECIAL DEPENDENCIES
#   Most dependency sequences should be derrived automatically. The only ones
#   that aren't are dependencies on generated header files. Those are specified
#   here.

$(SRCDIR)/compiler_main.cpp: $(SRCDIR)/parser.tab.hpp
$(SRCDIR)/scanner.yy.cpp: $(SRCDIR)/parser.tab.hpp


# RULE PATTERNS

# link
$(BINDIR)/%:
	@$(MKDIR) $(@D)
	$(CXX) -o $@ $(LDFLAGS) $(LDLIBS) $^

# compile/assemble
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
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
	$(CXX) -c -MT "$(BUILDDIR)/$*.o $(BUILDDIR)/$*.d" -MM -MP $(CPPFLAGS) $^ > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.d,$(SRCS))
-include $(patsubst $(SRCDIR)/%.lex,$(BUILDDIR)/%.yy.d,$(LEXS))
endif
