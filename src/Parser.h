///
/// \file contains some utility functions for parsing data
///
#ifndef GAME_PARSER_H
#define	GAME_PARSER_H

#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <algorithm>

namespace Game {

    namespace Parser {
        
        ///
        /// \class a basic error type thrown when a parser encounters an error
        ///
        class ParseError : public std::runtime_error {
        public:
            
            ///
            /// Creates a new parse error
            /// \param message the message for this error
            ///
            ParseError(const std::string &message);
        };

        ///
        /// \class an error to be used when a syntax error was found in a simple value string (single line)
        ///
        class ValueSyntaxError : public ParseError {
        public:
            
            ///
            /// Creates a new syntax error
            /// \param message the message for this error
            ///
            ValueSyntaxError(const std::string &message);
        };

        ///
        /// \class an error to be used when a value was found for the specified key in a value map
        ///
        class ValueNotFoundError : public ParseError {
        public:
            
            ///
            /// Creates a new error indicating a value was not found
            /// \param message the message for this error
            ///
            ValueNotFoundError(const std::string &message);
        };

        namespace Helper {

            template<typename Value> struct ParsePolicy {
            public:

                static bool execute(const std::string &str, Value &result) {
                    using namespace std;
                    istringstream buffer{str};
                    buffer >> result;
                    return !buffer.bad();
                };
            };
            
            template<> struct ParsePolicy<std::string> {
            public:

                static bool execute(const std::string &str, std::string &result) {
                    result = str;
                    return true;
                };
            };

        }

        ///
        /// Parses the value into the result if possible
        /// \param str the formatted string value
        /// \param result will be set to the resulting parsed value
        /// \return true if parsing was successful, false otherwise
        ///
        template<typename Value> bool from_string(const std::string &str, Value &result) {
            return Helper::ParsePolicy<Value>::execute(str, result);
        };

        ///
        /// Parses the value into the result if possible
        /// \param begin a forward char iterator pointing to the beginning of the input value
        /// \param begin a forward char iterator pointing to the end of the input value
        /// \param result will be set to the resulting parsed value
        /// \return true if parsing was successful, false otherwise
        ///
        template<typename Value, typename ForwardIterator> bool from_range(ForwardIterator begin, ForwardIterator end, Value &result) {
            using namespace std;
            return from_string(string{begin, end}, result); //possible to improve this
        };

        ///
        /// Parses the value from the pair's second field into the result if possible
        /// \param pair the pair (key, formatted_value) containing the formatted string value
        /// \param result will be set to the resulting parsed value
        /// \return true if parsing was successful, false otherwise
        ///
        template<typename Pair, typename Value> bool from_pair(const Pair &pair, Value &result) {
            return from_string(pair.second, result);
        };

        ///
        /// Parses a value from the map into the result if possible
        /// \param map the map containing the formatted string value
        /// \param key the key of the formatted string value
        /// \param result will be set to the resulting parsed value
        /// \return true if the value was found and parsing was successful, false otherwise
        ///
        template<typename Map, typename Value> bool from_map(const Map &map, const typename Map::key_type &key, Value &result) {
            auto found = map.find(key);
            if (found != map.end()) {
                return from_pair(*found, result);
            } else {
                return false;
            }
        };

        ///
        /// Parses a value from the map into the result if possible
        /// \param map the map containing the formatted string value
        /// \param key the key of the formatted string value
        /// \return the parsed value or the default value if it was not found or could not be parsed
        ///
        template<typename Map, typename Value> Value from_map(const Map &map, const typename Map::key_type &key) {
            Value value;
            from_map(map, key, value);
            return value;
        };

        ///
        /// Parses a value from the map into the result if possible
        /// \param map the map containing the formatted string value
        /// \param key the key of the formatted string value
        /// \param result will be set to the resulting parsed value
        /// \throw ValueSyntaxError if the value could not be parsed
        /// \throw ValueNotFoundError if the value could not be found
        ///
        template<typename Map, typename Value> void required_from_map(const Map &map, const typename Map::key_type &key, Value & result) {
            using namespace std;
            auto found = map.find(key);
            if (found != map.end()) {
                if (!from_pair(*found, result)) {
                    ostringstream buffer;
                    buffer << "can't parse value for key '" << key << "'";
                    throw ValueSyntaxError{buffer.str()};
                }
            } else {
                ostringstream buffer;
                buffer << "can't find value for key '" << key << "'";
                throw ValueNotFoundError{buffer.str()};
            }
        };

        ///
        /// Parses a value from the map into the result if possible
        /// \param map the map containing the formatted string value
        /// \param key the key of the formatted string value
        /// \return the resulting parsed value
        /// \throw ValueSyntaxError if the value could not be parsed
        /// \throw ValueNotFoundError if the value could not be found
        ///
        template<typename Map, typename Value> void required_from_map(const Map &map, const typename Map::key_type &key) {
            Value value;
            required_from_map(map, key, value);
            return value;
        };
        
        ///
        /// \class Helper type that is used to determine the type for the elements of the sequence to be parsed. This is needed because insert iterators have a void value_type
        ///
        template<typename Element_> struct Sequence{
            using Element = Element_;
            
            ///
            /// The delimiter used in the sequence
            ///
            char delimiter;
            
            ///
            /// Creates a new sequence definition with the standard delimiter (comma)
            ///
            Sequence() : delimiter(','){};
            
            ///
            /// Creates a new sequence definition with the specified delimiter
            /// \param delimiter_ the delimiter used in this sequence
            ///
            Sequence(char delimiter_) : delimiter(delimiter_){};
        };

        ///
        /// Parses a basic sequence from a string
        /// e.g. "1,2,12,3.12" with delimiter ',' results in the elements 1 , 2 , 12 and 3.12 being parsed
        /// If any part of the sequence fails, no results will be pushed to the result insert iterator
        /// \param str the formatted input string
        /// \param sequence contains all information needed to parse the sequence (such as element delimiter and type)
        /// \param result an insert iterator to store the result
        /// \return true if the sequence was parsed successfully, false otherwise
        ///
        template<typename Value, typename InsertIterator> bool sequence_from_string(const std::string &str, Sequence<Value> sequence, InsertIterator result) {
            using namespace std;
            if (str.empty()) {
                return true;
            } else {
                vector<Value> buffer;
                string::const_iterator begin = str.begin();
                string::const_iterator end = find(begin, str.end(), sequence.delimiter);
                while (begin != end) {
                    Value value;
                    if (from_range(begin, end, value)) {
                        buffer.push_back(value);
                    } else {
                        return false;
                    }
                    begin = end;
                    if (begin != str.end()) {
                        ++begin;
                        if (begin == str.end()) {
                            end = begin;
                        } else {
                            end = find(begin, str.end(), sequence.delimiter);
                        }
                    }
                }
                copy(buffer.begin(), buffer.end(), result);
                return true;
            }
        };
        
        ///
        /// Parses a basic sequence from the second field of a pair
        /// e.g. "1,2,12,3.12" with delimiter ',' results in the elements 1 , 2 , 12 and 3.12 being parsed
        /// If any part of the sequence fails, no results will be pushed to the result insert iterator
        /// \param pair the pair (key,  formatted input string)
        /// \param sequence contains all information needed to parse the sequence (such as element delimiter and type)
        /// \param result an insert iterator to store the result
        /// \return true if the sequence was parsed successfully, false otherwise
        ///
        template<typename Value, typename Pair, typename InsertIterator> bool sequence_from_pair(const Pair &pair, Sequence<Value> sequence, InsertIterator result) {
            return sequence_from_string(pair.second, sequence, result);
        };

        ///
        /// Parses a basic sequence from the map
        /// e.g. the input string "1,2,12,3.12" with delimiter ',' results in the elements 1 , 2 , 12 and 3.12 being parsed
        /// If any part of the sequence fails, no results will be pushed to the result insert iterator
        /// \param map the map containing the formatted input string)
        /// \param key the key used to find the formatted input string in the map
        /// \param sequence contains all information needed to parse the sequence (such as element delimiter and type)
        /// \param result an insert iterator to store the result
        /// \return true if the sequence was parsed successfully, false otherwise
        ///
        template<typename Value, typename Map, typename InsertIterator> bool sequence_from_map(const Map &map, const typename Map::key_type &key, Sequence<Value> sequence, InsertIterator result) {
            auto found = map.find(key);
            if (found != map.end()) {
                return sequence_from_pair(*found, sequence, result);
            } else {
                return false;
            }
        };

        ///
        /// Parses a basic sequence from the map
        /// e.g. the input string "1,2,12,3.12" with delimiter ',' results in the elements 1 , 2 , 12 and 3.12 being parsed
        /// If any part of the sequence fails, no results will be pushed to the result insert iterator
        /// \param map the map containing the formatted input string)
        /// \param key the key used to find the formatted input string in the map
        /// \param sequence contains all information needed to parse the sequence (such as element delimiter and type)
        /// \param result an insert iterator to store the result
        /// \throw ValueSyntaxError if the value could not be parsed
        /// \throw ValueNotFoundError if the value could not be found
        ///
        template<typename Value, typename Map, typename InsertIterator> void required_sequence_from_map(const Map &map, const typename Map::key_type &key, Sequence<Value> sequence, InsertIterator result) {
            using namespace std;
            auto found = map.find(key);
            if (found != map.end()) {
                if (!sequence_from_pair(*found, sequence, result)) {
                    ostringstream buffer;
                    buffer << "can't parse value for key '" << key << "'";
                    throw ValueSyntaxError{buffer.str()};
                }
            } else {
                ostringstream buffer;
                buffer << "can't find value for key '" << key << "'";
                throw ValueNotFoundError{buffer.str()};
            }
        };
        
    }

}

#endif	/* PARSER_H */

