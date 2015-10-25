# directories
BINDIR   = bin
BUILDDIR = build
SRCDIR   = src

# build tools
CXX  = g++
FLEX = flex

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


# BUILD ACTIONS

all: $(BINARIES)

clean:
	$(RM) $(BINDIR) $(BUILDDIR) $(patsubst $(SRCDIR)/%.lex,$(SRCDIR)/%.yy.cpp,$(LEXS))

.SECONDARY:
.PHONY: all clean


# SPECIFY BINARY DEPENDENCIES
#   All dependencies will automatically be built using the rule patterns
#   specified below.

$(BINDIR)/compiler: $(BUILDDIR)/scanner.yy.o
$(BINDIR)/preprocessor: $(BUILDDIR)/preprocessor.yy.o $(BUILDDIR)/macro.o


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

# dependencies
$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -c -MT "$(BUILDDIR)/$*.o $(BUILDDIR)/$*.d" -MM -MP $(CPPFLAGS) $^ > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.d,$(SRCS))
-include $(patsubst $(SRCDIR)/%.lex,$(BUILDDIR)/%.yy.d,$(LEXS))
endif
