#ifndef IO_IO_ERROR_H
#define IO_IO_ERROR_H

///
/// Contains basic IO errors
///

#include <stdexcept>

namespace Core{

    ///
    /// A common base for all IO errors
    ///
	class IOError : public std::runtime_error{
	public:

        ///
        /// Creates a new IO error
        ///
        IOError();

        ///
        /// Creates a new IO error
        /// \param message The message associated with the error
        ///
        IOError(const std::string &message);
	};

}

#endif
