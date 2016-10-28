/* 
 * File:   PropertyDriver.h
 * Author: hans
 *
 * Created on April 15, 2016, 2:36 PM
 */

#ifndef PROPERTYDRIVER_H
#define	PROPERTYDRIVER_H

#include "Property.h"
#include "PropertyLexer.h"

#include <sstream>

namespace Property {

    class ParserTask{
    public:
        ParserTask(PropertyLexer *lexer_, Map &properties);
        
        void put_value(const std::string &key, const std::string &value);
        
        void report_error(const std::string &message, int line, int column);
        
        const std::string &error_message() const;
        
        int error_line() const;
        
        int error_column() const;
        
        PropertyLexer * const lexer;
        
        ParserTask(const ParserTask &) = delete;
        ParserTask(ParserTask &&) = delete;
        ParserTask &operator=(const ParserTask &) = delete;
        ParserTask &operator==(ParserTask &&) = delete;
        
    private:
        Map &properties_;
        std::string error_message_;
        int error_line_;
        int error_column_;
    };
}

#endif	/* PROPERTYDRIVER_H */

