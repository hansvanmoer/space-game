#include "Application.h"

using namespace Game;

using namespace std;

ApplicationError::ApplicationError(const ApplicationId& id, const std::string& message) : runtime_error(message), id_(id){
}

ApplicationId ApplicationError::id() const {
    return id_;
}

