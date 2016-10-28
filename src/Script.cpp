#include "Script.h"
#include "Name.h"

using namespace std;
using namespace Game;
using namespace Game::Script;

ScriptError::ScriptError(const std::string& message) : runtime_error(message){
}

ObjectError::ObjectError(const std::string& message) : ScriptError(message){
}

DuplicateObjectError::DuplicateObjectError(const std::string& id) : ObjectError(string{"duplicate object id: "}+id){
}

NoSuchObjectError::NoSuchObjectError(const std::string& id) : ObjectError(string{"unknown object id: "}+id){
}

NullHandleError::NullHandleError() : ObjectError("null handle dereference"){}


