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

        class ParseError : public std::runtime_error {
        public:
            ParseError(const std::string &message);
        };

        class ValueSyntaxError : public ParseError {
        public:
            ValueSyntaxError(const std::string &message);
        };

        class ValueNotFoundError : public ParseError {
        public:
            ValueNotFoundError(const std::string &message);
        };

        namespace Helper {

            template<typename Value> struct ParsePolicy {
            public:

                static bool execute(const std::string &str, Value &result) {
                    using namespace std;
                    ostringstream buffer{str};
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

        template<typename Value> bool from_string(const std::string &str, Value &result) {
            return Helper::ParsePolicy<Value>::execute(str, result);
        };

        template<typename Value, typename ForwardIterator> bool from_range(ForwardIterator begin, ForwardIterator end, Value &result) {
            using namespace std;
            return from_string(string{begin, end}, result); //possible to improve this
        };

        template<typename Pair, typename Value> bool from_pair(const Pair &pair, Value &result) {
            return from_string(pair.second, result);
        };

        template<typename Map, typename Value> bool from_map(const Map &map, const typename Map::key_type &key, Value &result) {
            auto found = map.find(key);
            if (found != map.end()) {
                return from_pair(*found, result);
            } else {
                return false;
            }
        };

        template<typename Map, typename Value> Value from_map(const Map &map, const typename Map::key_type &key) {
            Value value;
            from_map(map, key, value);
            return value;
        };

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

        template<typename Map, typename Value> void required_from_map(const Map &map, const typename Map::key_type &key) {
            Value value;
            required_from_map(map, key, value);
            return value;
        };

        template<typename InsertIterator> bool sequence_from_string(const std::string &str, char delimiter, InsertIterator result) {
            using namespace std;
            using Value = typename iterator_traits<InsertIterator>::value_type;

            if (str.empty()) {
                return true;
            } else {
                vector<Value> buffer;
                string::const_iterator begin = str.begin();
                string::const_iterator end = find(begin, str.end(), delimiter);
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
                            end = find(begin, str.end(), delimiter);
                        }
                    }
                }
                copy(buffer.begin(), buffer.end(), result);
                return true;
            }
        };

        template<typename Pair, typename InsertIterator> bool sequence_from_pair(const Pair &pair, char delimiter, InsertIterator result) {
            return sequence_from_string(pair.second, delimiter, result);
        };

        template<typename Map, typename InsertIterator> bool sequence_from_map(const Map &map, const typename Map::key_type &key, char delimiter, InsertIterator result) {
            auto found = map.find(key);
            if (found != map.end()) {
                return sequence_from_pair(*found, delimiter, result);
            } else {
                return false;
            }
        };

        template<typename Map, typename InsertIterator> bool required_sequence_from_map(const Map &map, const typename Map::key_type &key, char delimiter, InsertIterator result) {
            using namespace std;
            auto found = map.find(key);
            if (found != map.end()) {
                if (!sequence_from_pair(*found, delimiter, result)) {
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

