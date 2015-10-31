 // Enable line number tracking by Flex.
%option yylineno

 // Generate c++ class instead of bare c functions.
%option c++

 // Scanner does not support handling multiple files.
%option yywrap nounput

%{

#include "scanner.hpp"

#include <cassert>

#include <iostream>
#include <string>
#include <vector>

#include "parser.tab.hpp"

// By default yylex returns int. As part of how we are interfacing with Bison,
// we return parser::Parser::token_type. This means that Flex's EOF return code
// won't work. We need to explicitly use parser::Parser::token_type::END.
#define yyterminate() return token::END

// Each time yylex is invoked, advance the end position's column a number of
// spaces equal to the length of the matched text.
#define YY_USER_ACTION yylloc->columns(yyleng);

%}


%%


%{

// "Import" the token enumeration from the generated Parser class.
// (Originally defined in "parser.tab.hpp" which is generated from
// "parser.yy".)
typedef parser::Parser::token token;
typedef parser::Parser::token_type token_type;

// Reset the begining of the location tracker to its end.
yylloc->step();

%}


"+" { return token::PLUS; }
"-" { return token::MINUS; }
"*" { return token::MULTIPLY; }
"/" { return token::DIVIDE; }
"%" { return token::MODULO; }

">>" { return token::SHIFTRIGHT; }
"<<" { return token::SHIFTLEFT; }

"," { return static_cast<token_type>(','); }
"=" { return static_cast<token_type>('='); }

"==" { return token::EQUAL;        }
"!=" { return token::NEQUAL;       }
"<"  { return token::LESS;         }
">"  { return token::GREATER;      }
"<=" { return token::LESSEQUAL;    }
">=" { return token::GREATEREQUAL; }

if     { return token::IF;     }
else   { return token::ELSE;   }
while  { return token::WHILE;  }
do     { return token::DO;     }
for    { return token::FOR;    }
return { return token::RETURN; }

int    { return token::INT;    }
string { return token::STRING; }
extern { return token::EXTERN; }

[A-Za-z_][A-Za-z0-9_]* {
    yylval->string_value = new std::string(yytext, yyleng);
    return token::IDENT;
}

"("|")"|"{"|"}" { return static_cast<token_type>(*yytext); }

";" { return static_cast<token_type>(';'); }

[0-9]+ {
    yylval->integer_value = atoi(yytext);
    return token::CONST_INT;
}

\"[^\"]*\" {
    yylval->string_value = new std::string(yytext, yyleng);
    return token::CONST_STRING;
}

[ \t]+  yylloc->step();
\n+     yylloc->lines(yyleng); yylloc->step();

.   {
    std::cerr
        << "ERROR: unrecognized character '" << yytext
        << "' found in line " << yylineno
        << std::endl
        ;
}

%%


// This should never be executed.
int ScannerFlexLexer::yylex () {
    assert(false and "This function should never be called. You meant to call parser::Parser::yylex(parser::Parser::semantic_type* yylval).");
}


/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int ScannerFlexLexer::yywrap () {
    return 1;
}
