/* 
 * File:   String.h
 * Author: hans
 *
 * Created on October 14, 2016, 8:49 PM
 */

///
/// \file contains various utility functions to manipulate strings
///

#ifndef STRING_H
#define	STRING_H

#include <string>
#include <algorithm>

namespace Game{
    
    ///
    /// Trims the specified string's leading whitespace (as defined by std::isspace)
    /// \param str a reference to the string that is to be stripped of its leading whitespace
    ///
    inline void trim_leading_whitespace(std::string &str){
        using namespace std;
        str.erase(str.begin(), find_if(str.begin(), str.end(), [](const char c){return !std::isspace(c);}));
    };
    
    ///
    /// Trims the specified string's trailing whitespace (as defined by std::isspace)
    /// \param str a reference to the string that is to be stripped of its trailing whitespace
    ///
    inline void trim_trailing_whitespace(std::string &str){
        using namespace std;
        str.erase(find_if(str.rbegin(), str.rend(), [](const char c){return !std::isspace(c);}).base(), str.end());
    };
    
        ///
    /// Trims the specified string's leading and trailing whitespace (as defined by std::isspace)
    /// \param str a reference to the string that is to be stripped of its leading and trailing whitespace
    ///
    inline void trim_whitespace(std::string &str){
        trim_leading_whitespace(str);
        trim_trailing_whitespace(str);
    };
    
}

#endif	/* STRING_H */

