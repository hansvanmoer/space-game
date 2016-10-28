#include "Property.h"

#include "PropertyTask.h"

using namespace std;
using namespace Property;

PropertyError::PropertyError(const std::string &msg) : runtime_error(msg){}

SyntaxError::SyntaxError(const std::string &msg, int line, int column) : PropertyError(msg), line_(line), column_(column){}

int SyntaxError::line() const{
    return line_;
}

int SyntaxError::column() const{
    return column_;
}

Map Property::read_properties(std::istream& input){
    Map result;
    PropertyLexer lexer{&input, &std::cout};
    ParserTask task{&lexer, result};
    Parser parser{task};
    if(parser.parse() != 0){
        throw SyntaxError{std::string{"parsing did not complete successfully: "} + task.error_message(), task.error_line(), task.error_column()};
    }
    return result;
}