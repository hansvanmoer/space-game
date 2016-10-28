#include "PropertyTask.h"

using namespace std;

using namespace Property;

ParserTask::ParserTask(PropertyLexer* lexer_, Map& properties) : lexer(lexer_), properties_(properties), error_message_(), error_line_(), error_column_(){
}

void ParserTask::put_value(const std::string& key, const std::string& value) {
    properties_[key] = value;
}

void ParserTask::report_error(const std::string& message, int line, int column){
    error_message_ = message;
    error_line_ = line;
    error_column_ = column;
}

const std::string& ParserTask::error_message() const {
    return error_message_;
}

int ParserTask::error_line() const {
    return error_line_;
}

int ParserTask::error_column() const{
    return error_column_;
}




