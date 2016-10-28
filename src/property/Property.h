///
/// \file Contains the main interface of the property parser library
///

#ifndef SIMPLE_PROPERTY_PROPERTY_H
#define SIMPLE_PROPERTY_PROPERTY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Property {

    ///
    /// \class a basic error type for all errors thrown by the property parser
    ///
    class PropertyError : public std::runtime_error {
    public:
        
        ///
        /// Creates a new property error
        /// \param msg the message for this error
        ///
        PropertyError(const std::string &msg);
    };
    
    ///
    /// \class an error thrown when the parser encounters a syntax error
    ///
    class SyntaxError : public PropertyError{
    public:
        
        ///
        /// Creates a new syntax error
        /// \param msg the message for this error
        /// \param line the line where the error occurred
        /// \param column the column where the error occurred
        ///
        SyntaxError(const std::string &msg, int line, int column);
        
        ///
        /// Returns the line number where the error occurred (starts at 0)
        /// \return the line number
        ///
        int line() const;
        
        ///
        /// Returns the column number where the error occurred (starts at 0)
        /// \return the column number
        ///
        int column() const;
        
    private:
        const int line_;
        const int column_;
    };
    
    ///
    /// \typedef a basic property map
    ///
    using Map = std::unordered_map<std::string, std::string>;

    ///
    /// Reads the properties from this input stream
    /// \param input an input stream containing ASCII or UTF-8 encoded properties
    ///
    Map read_properties(std::istream &input);
    
}

#endif
