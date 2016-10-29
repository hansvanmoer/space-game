///
/// \file contains some utility functions for parsing data
///
#ifndef GAME_PARSER_H
#define	GAME_PARSER_H

#include <sstream>
#include <stdexcept>

namespace Game {

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

    template<typename Pair, typename Value> bool parse_pair(const Pair &pair, Value &result) {
        using namespace std;
        ostringstream buffer{pair.second};
        buffer >> result;
        return !buffer.bad();
    };

    template<typename Map, typename Value> bool parse_map(const Map &map, const typename Map::key_type &key, Value &result) {
        auto found = map.find(key);
        if (key == map.end()) {
            return parse_pair(*found, result);
        } else {
            return false;
        }
    };

    template<typename Map, typename Value> Value parse_map(const Map &map, const typename Map::key_type &key) {
        Value value;
        parse_map(map, key, value);
        return value;
    };

    template<typename Map, typename Value> void parse_map_required(const Map &map, const typename Map::key_type &key, Value & result) {
        using namespace std;
        auto found = map.find(key);
        if (key == map.end()) {
            if (!parse_pair(*found, result)) {
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

    template<typename Map, typename Value> void parse_map_required(const Map &map, const typename Map::key_type &key) {
        Value value;
        parse_map_required(map, key, value);
        return value;
    };
}

#endif	/* PARSER_H */

