#include "IOError.h"

using namespace Core;
using namespace std;

IOError::IOError() : IOError(string{"an unspecified IO error has occurred"}){}

IOError::IOError(const string &message) : runtime_error(message){}
