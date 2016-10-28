#ifndef SIMPLE_PROPERTY_PROPERTY_H
#define SIMPLE_PROPERTY_PROPERTY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Property {

    class PropertyError : public std::runtime_error {
    public:
        PropertyError(const std::string &msg);
    };
    
    class SyntaxError : public PropertyError{
    public:
        SyntaxError(const std::string &msg, int line, int column);
        
        int line() const;
        
        int column() const;
        
    private:
        const int line_;
        const int column_;
    };
    
    using Map = std::unordered_map<std::string, std::string>;

    Map read_properties(std::istream &input);
    
}

#endif
