#include "CLI.h"

#include <sstream>

#include <boost/filesystem.hpp>

using namespace Game::CLI;

using namespace std;

ArgumentError::ArgumentError(const std::string& msg) : runtime_error(msg) {
}

ArgumentDefinition::ArgumentDefinition(const std::string& name_, const std::string& long_switch_, char short_switch_, const std::string& default_value_, bool required_) : name(name_), long_switch(long_switch_), short_switch(short_switch_), default_value(default_value_), required(required_) {
}

Call::Call() : application_run_path(), arguments(){}

ostream &Game::CLI::operator<<(ostream &output, const Call &call){
    output << "run path: '" << call.application_run_path << '\'' << endl;
    output << "arguments:" << endl;
    for(auto entry : call.arguments){
        output << "   " << entry.first << " = '" << entry.second << '\'' << endl;
    }
    return output;
}

ArgumentParser::ArgumentParser() : argument_definitions_(), short_switches_(), long_switches_() {
}

ArgumentParser::ArgumentParser(std::initializer_list<ArgumentDefinition> argument_definitions) {
    for (ArgumentDefinition def : argument_definitions) {
        add(def);
    };
}

template<typename Value> inline void assert_argument(const string &name, const Value &value, unordered_map<Value, const ArgumentDefinition *> map) {
    if (map.find(value) != map.end()) {
        std::ostringstream buffer;
        buffer << "conflicting argument definition: argument with " << name << " '" << value << "' was already defined";
        throw ArgumentError{buffer.str()};
    }
};

ArgumentParser& ArgumentParser::add(const ArgumentDefinition& argument_definition) {
    assert_argument("name", argument_definition.name, argument_definitions_);
    assert_argument("short switch", argument_definition.short_switch, short_switches_);
    assert_argument("long switch", argument_definition.long_switch, long_switches_);
    ArgumentDefinition *managed_definition = new ArgumentDefinition{argument_definition};
    argument_definitions_.emplace(argument_definition.name, managed_definition);
    short_switches_.emplace(argument_definition.short_switch, managed_definition);
    long_switches_.emplace(argument_definition.long_switch, managed_definition);
}

ArgumentParser::~ArgumentParser() {
    for (auto entry : argument_definitions_) {
        delete entry.second;
    }
}


template<typename Iterator> bool parse_short_switch(Iterator &i, Iterator end, char &short_switch) {
    if (*i == '-') {
        ++i;
        if (i == end) {
            throw ArgumentError{"invalid token: '-'"};
        } else {
            short_switch = *i;
            ++i;
            return true;
        }
    }
    return false;
}

template<typename Iterator> bool parse_long_switch(Iterator &i, Iterator end, string &long_switch) {
    Iterator j = i;
    if (*j == '-') {
        ++j;
        if (j != end) {
            if (*j == '-') {
                ++j;
                if(j == end){
                    throw ArgumentError{"unexpected end of argument: expected long switch"};
                }else{
                    long_switch = string{j, end};
                    i = end;
                    return true;
                }
            }
        }
    }
    return false;
}


void ArgumentParser::parse(Call& result, int arg_count, const char** args) const {
    result = parse(arg_count, args);
}

Call ArgumentParser::parse(int arg_count, const char** args) const {
    if(arg_count == 0){
        throw ArgumentError{"expected at least one argument"};
    }
   
    Call call;
    
    call.application_run_path = boost::filesystem::path{args[0]};
    
    int arg_index = 1;
    char short_switch;
    string long_switch;
    const ArgumentDefinition *incomplete = nullptr;
    while (arg_index != arg_count) {
        const string arg{args[arg_index]};
        string::const_iterator i = arg.begin();
        const ArgumentDefinition * def;
        if (parse_long_switch(i, arg.end(), long_switch)) {
            auto found = long_switches_.find(long_switch);
            if (found == long_switches_.end()) {
                throw ArgumentError{"unknown switch: " + arg};
            }
            def = found->second;
        }else if (parse_short_switch(i, arg.end(), short_switch)) {
            auto found = short_switches_.find(short_switch);
            if (found == short_switches_.end()) {
                throw ArgumentError{"unknown switch: " + arg};
            }
            def = found->second;
        } else {
            if (incomplete) {
                call.arguments.emplace(incomplete->name, arg);
                incomplete = nullptr;
            } else {
                throw ArgumentError{"unexpected tokens: " + arg};
            }
        }

        if (incomplete) {
            if (!incomplete->default_value.empty()) {
                call.arguments.emplace(incomplete->name, incomplete->default_value);
            }
        }

        if (i == arg.end()) {
            incomplete = def;
        } else {
            call.arguments.emplace(def->name, string {
                i, arg.end()
            });
            incomplete = nullptr;
        }
        ++arg_index;
    }
    if (incomplete) {
        if (!incomplete->default_value.empty()) {
            call.arguments.emplace(incomplete->name, incomplete->default_value);
        }
    }

    for (auto entry : argument_definitions_) {
        if (entry.second->required && call.arguments[entry.second->name].empty()) {
            throw ArgumentError{string{"required argument not found: "}+entry.second->name};
        }else if(!entry.second->default_value.empty()){
            call.arguments.emplace(entry.second->name, entry.second->default_value);
        }
    }
    return call;
}




