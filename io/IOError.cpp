#include "IOError.h"

using namespace Core;
using namespace std;

IOError::IOError(const string &message) : runtime_error(message){}
