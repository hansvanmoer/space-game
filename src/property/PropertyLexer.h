/* 
 * File:   PropertyLexer.h
 * Author: hans
 *
 * Created on April 15, 2016, 8:53 PM
 */

#ifndef PROPERTY_PROPERTY_LEXER_H
#define	PROPERTY_PROPERTY_LEXER_H

/**
 * define YY_DECL macro
 */
#ifndef YY_DECL
#define	YY_DECL						\
    Property::Parser::token_type			\
    Property::PropertyLexer::lex(				\
	Property::Parser::semantic_type* yylval,        \
	Property::Parser::location_type* yylloc		\
    )
#endif

/**
 rename flexlexer to property flex lexer
 */
#ifndef __FLEX_LEXER_H
#define yyFlexLexer PropertyFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif

#include "PropertyParser.hpp"

namespace Property{
    
    class PropertyLexer : public PropertyFlexLexer{
    public:
        
        PropertyLexer(std::istream *input, std::ostream *output);
        
        virtual Parser::token_type lex(Parser::semantic_type *value, Parser::location_type *location);
        
    };
    
}

#endif	/* PROPERTYLEXER_H */

