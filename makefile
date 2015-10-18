ifdef COMSPEC
	CXX  = g++.exe
	FLEX = flex.exe
	RM   = rm -f

	EXECUTABLES = compiler.exe preprocessor.exe

else
	CXX  = g++
	FLEX = flex
	RM   = rm -f

	EXECUTABLES = compiler

endif

CPPFLAGS =
CXXFLAGS = -std=gnu++11
LDFLAGS  =
LDLIBS   =

LEXS = preprocessor.lex scanner.lex
LEX_CPP = $(subst .lex,.yy.cpp,$(LEXS))

SRCS = macro.cpp $(LEX_CPP)
OBJS = $(subst .cpp,.o,$(SRCS))


compiler: scanner.yy.o
	$(CXX) $(LDFLAGS) -o compiler scanner.yy.o $(LDLIBS)

preprocessor: preprocessor.yy.o macro.o
	$(CXX) $(LDFLAGS) -o preprocessor preprocessor.yy.o macro.o $(LDLIBS)

all: compiler preprocessor

macro.o: macro.cpp

preprocessor.yy.o: preprocessor.yy.cpp

scanner.yy.o: scanner.yy.cpp

scanner.yy.cpp: scanner.lex
	$(FLEX) -o scanner.yy.cpp scanner.lex

preprocessor.yy.cpp: preprocessor.lex
	$(FLEX) -o preprocessor.yy.cpp preprocessor.lex

clean:
	$(RM) $(OBJS) $(LEX_CPP) $(EXECUTABLES)
