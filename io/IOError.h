#ifndef IO_IO_ERROR_H
#define IO_IO_ERROR_H

#include <stdexcept>

namespace Core{

	class IOError : public std::runtime_error{
	public:
        IOError(const std::string &message);
	};

}

#endif
