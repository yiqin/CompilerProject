BINDIR   = bin
BUILDDIR = build
SRCDIR   = src

ifdef COMSPEC
	CXX   = g++.exe
	FLEX  = flex.exe
	RM    = rm -rf
	MKDIR = mkdir -p

else
	CXX   = g++
	FLEX  = flex
	RM    = rm -f
	MKDIR = mkdir -p

endif

CPPFLAGS = -I $(SRCDIR)
CXXFLAGS = -std=gnu++11
LDFLAGS  =
LDLIBS   =

BINARIES = compiler preprocessor
BINARIES := $(addprefix $(BINDIR)/,$(BINARIES))


# BUILD ACTIONS

all: $(BINARIES)

clean:
	$(RM) $(BINDIR) $(BUILDDIR)

.SECONDARY:
.PHONY: all clean


# SPECIFY BINARY DEPENDENCIES
#   All dependencies will automatically be built using the rule patterns
#   specified below.

$(BINDIR)/compiler: $(BUILDDIR)/scanner.yy.o
$(BINDIR)/preprocessor: $(BUILDDIR)/preprocessor.yy.o $(BUILDDIR)/macro.o


# RULE PATTERNS

$(BINDIR)/%:
	@$(MKDIR) $(@D)
	$(CXX) -o $@ $(LDFLAGS) $(LDLIBS) $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) -o $@ -c $(CPPFLAGS) $(CXXFLAGS) $<

$(BUILDDIR)/%.yy.cpp: $(SRCDIR)/%.lex
	@$(MKDIR) $(@D)
	$(FLEX) -o $@ $<
