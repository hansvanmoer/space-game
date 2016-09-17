#include "FileSystem.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

using namespace Core;
using namespace std;

PathError::PathError(const string &message) : IOError {message} {};

InvalidPathError::InvalidPathError() : PathError {"operation expected a valid path"} {};

#ifdef TARGET_OS_UNIX_LIKE

const char Core::pathSeparatorChar {
    '/'
};

const string Core::pathSeparator {
    "/"
};

bool is_absolute(const string &non_empty_path) {
    return *non_empty_path.begin() == pathSeparatorChar;
}

#endif

#ifdef TARGET_OS_WINDOWS

//TODO: add windows specific code

#endif

template<typename CharIterator, typename OutputIterator> void parse_segments(CharIterator begin, CharIterator end, OutputIterator dest) {
    char c;
    ostringstream element_buffer;
    bool not_empty = false;
    while(begin != end) {
        c = *begin;
        if(c == pathSeparatorChar) {
            if(not_empty) {
                *dest = element_buffer.str();
                ++dest;
                element_buffer.str("");
                element_buffer.clear();
                not_empty = false;
            }
        } else {
            not_empty = true;
            element_buffer.put(c);
        }
        ++begin;
    }
    if(not_empty) {
        *dest = element_buffer.str();
        ++dest;
    }
}

string normalize_whole_path(const string &value) {
    vector<string> elements;
    parse_segments(value.begin(), value.end(), back_inserter(elements));
    if(elements.empty()) {
        throw InvalidPathError {};
    } else {
        ostringstream buffer;
        if(is_absolute(value)) {
            buffer.put(pathSeparatorChar);
        }
        vector<string>::const_iterator i = elements.begin();
        buffer << *i;
        for(; i != elements.end(); ++i) {
            buffer << pathSeparatorChar << *i;
        }
        return buffer.str();
    }
}

string normalize_relative_path(const string &value) {
    vector<string> elements;
    parse_segments(value.begin(), value.end(), back_inserter(elements));
    if(elements.empty()) {
        throw InvalidPathError {};
    } else {
        ostringstream buffer;
        for(auto i = elements.begin(); i != elements.end(); ++i) {
            buffer << pathSeparatorChar << *i;
        }
        return buffer.str();
    }
}

Path::Path() : value_() {}

Path::Path(const string &value) : value_(normalize_whole_path(value)) {}

Path::Path(const Path &ancestor, const string &relative_path) : value_(ancestor.value()+normalize_relative_path(relative_path)) {
    if(!ancestor){
        throw InvalidPathError{};
    }
}

Path::Path(const string &value, bool) : value_(value){};

Path Path::create_child(const string& name) const {
    ostringstream buffer;
    if(name.find(pathSeparatorChar) == string::npos){
        buffer << value_ << pathSeparatorChar << name;
        return Path{buffer.str(), true};
    }else{
        buffer << "invalid child for path '" << value_ << "' with name '" << name << "'";
        throw PathError{buffer.str()};
    }
}

Path Path::create_descendant(const string& relative_path) {
    return Path{*this, relative_path};
}

const string &Path::value() const {
    return value_;
}

bool Path::absolute() const {
    return is_absolute(value_);
}

bool Path::relative() const {
    return !is_absolute(value_);
}

bool Path::valid() const {
    return !value_.empty();
}

bool Path::operator==(const Path& path) const {
    return value_ == path.value_;
}

bool Path::operator!=(const Path& path) const {
    return value_ != path.value_;
}

Path::operator bool() const {
    return valid();
}

bool Path::operator!() const {
    return !valid();
}

ostream &Core::operator<<(ostream& output, const Path& path) {
    return output << path.value();
}
