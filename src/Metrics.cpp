#include "Metrics.h"

using namespace Game;
using namespace std;

GeometryError::GeometryError(const std::string& message) : runtime_error(message){}

ZeroVectorError::ZeroVectorError(const std::string& message) : GeometryError(message){}

ZeroVectorError::ZeroVectorError() : GeometryError("invalid operation on a zero vector"){}