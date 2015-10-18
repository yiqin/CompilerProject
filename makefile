ifdef COMSPEC
	CXX  = g++.exe
	FLEX = flex.exe
	RM   = rm -f

	EXECUTABLES = compiler.exe

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

SRCS = scanner.yy.cpp
OBJS = $(subst .cpp,.o,$(SRCS))


compiler: scanner.yy.o
	$(CXX) $(LDFLAGS) -o compiler $(OBJS) $(LDLIBS)

scanner.yy.o: scanner.yy.cpp

scanner.yy.cpp: scanner.lex
	$(FLEX) -o scanner.yy.cpp scanner.lex

clean:
	$(RM) $(OBJS) scanner.yy.cpp $(EXECUTABLES)
