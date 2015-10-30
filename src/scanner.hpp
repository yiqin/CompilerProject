#ifndef __CSTR_COMPILER__SCANNER_HPP
#define __CSTR_COMPILER__SCANNER_HPP

#ifndef YY_DECL
#define YY_DECL \
    int ScannerFlexLexer::yylex()
#endif

#undef yyFlexLexer
#define yyFlexLexer ScannerFlexLexer
#include <FlexLexer.h>


#endif  // __CSTR_COMPILER__SCANNER_HPP
