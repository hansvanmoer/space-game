#include "Parser.h"

using namespace std;

using namespace Game;

ParseError::ParseError(const std::string& message) : runtime_error(message){
}

ValueSyntaxError::ValueSyntaxError(const std::string& message) : ParseError(message){
}

ValueNotFoundError::ValueNotFoundError(const std::string& message) : ParseError(message){

}

