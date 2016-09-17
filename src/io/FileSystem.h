///
/// Includes basic file system utility functions and types
///

#ifndef IO_FILE_SYSTEM_H
#define IO_FILE_SYSTEM_H

#include <string>
#include <iostream>

#include "IOError.h"

namespace Core {

///
/// A constant set to the system dependent path separator
///
extern const char pathSeparatorChar;

///
/// A constant set to the system dependent path separator
///
extern const std::string pathSeparator;

///
/// An error class for path errors
///
class PathError : public IOError{
public:

    ///
    /// Constructs a new error
    /// \param message the specified error
    ///
    PathError(const std::string &message);
};

///
/// An error signifying that an invalid path was used in an operation requiring a vaid path
///
class InvalidPathError : public PathError{
public:
    ///
    /// Constructs a new error
    ///
    InvalidPathError();
};

///
/// A wrapper class around a string used to ensure correct system dependent paths
/// Can also be used to create and manipulate paths
///
class Path {
public:

    ///
    /// Creates a new invalid path
    ///
    Path();

    ///
    /// Creates a new path using the specified value
    /// \param value A system dependent string representation of the path
    /// \throw PathError if unable to create a valid path from the specified value
    ///
    Path(const std::string &value);

    ///
    /// Creates a new path using the specified parent and relative path
    /// \param ancestor The ancestor path
    /// \param relative_path a relative path from the ancestor
    /// \throw InvalidPathError if the ancestor is invalid
    /// \throw PathError if unable to create a valid path from the specified ancestor and value
    ///
    Path(const Path &ancestor, const std::string &relative_path);

    ///
    /// Creates a new child path from the current path
    /// \param name the child's name, should not contain the path element separator
    /// \throw PathError if the child name is not valid or contains a path element separator
    ///
    Path create_child(const std::string &name) const;

    ///
    /// Creates a new descendant path from the current path
    /// \param relative_path a system dependent relative path from this path
    /// \throw PathError if the relative path is not valid
    ///
    Path create_descendant(const std::string &relative_path);

    ///
    /// Returns a system dependent representation of this path
    /// An invalid path is returned as an empty string
    /// The returned value will not contain a trailing path element separator
    /// On Unix like systems a leading path element separator signifies an absolute path
    /// \return the string representation of this path
    ///
    const std::string &value() const;

    ///
    /// \return true if the path is absolute, false if otherwise
    ///
    bool absolute() const;

    ///
    /// \return true if the path is relative, false if otherwise
    ///
    bool relative() const;

    ///
    /// \return true if the path is valid, false otherwise
    ///
    bool valid() const;

    ///
    /// Can be used to test if two paths are identical.
    /// Note that a single file or folder can have multiple distinct paths (e.g. a relative and an absolute) so this should not be used to test file or older equality
    /// \return true if both paths are identical, false otherwise
    ///
    bool operator==(const Path &path) const;

    ///
    /// Can be used to test if two paths are not identical.
    /// \return false if both paths are identical, true otherwise
    ///
    bool operator!=(const Path &path) const;

    ///
    /// \return true if the path is valid, false otherwise
    ///
    operator bool() const;

    ///
    /// \return false if the path is valid, true otherwise
    ///
    bool operator!() const;

private:

    Path(const std::string &valid_value, bool);

    std::string value_;
};


///
/// A standard output operator for the Path type
/// \param output the output stream
/// \param path the path
/// \return a reference to the output stream
///
std::ostream &operator<<(std::ostream &output, const Path &path);

}

#endif // IO_PATH_H
