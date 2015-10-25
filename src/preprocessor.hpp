#ifndef __CSTR_COMPILER__PREPROCESSOR_HPP
#define __CSTR_COMPILER__PREPROCESSOR_HPP

#ifndef YY_DECL
#define YY_DECL \
    int PreprocessorFlexLexer::yylex()
#endif

#undef yyFlexLexer
#define yyFlexLexer PreprocessorFlexLexer
#include <FlexLexer.h>



#endif  // __CSTR_COMPILER__PREPROCESSOR_HPP
