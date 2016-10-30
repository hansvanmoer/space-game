///
/// \file contains helper classes to parse command line arguments
///
#ifndef CLI_H
#define	CLI_H

#include <unordered_map>
#include <stdexcept>
#include <iostream>

#include <boost/filesystem.hpp>

namespace Game{
    
    namespace CLI{
        
        ///
        /// \class basic error thrown when an argument could not be parsed
        ///
        class ArgumentError : public std::runtime_error{
        public:
            ArgumentError(const std::string &msg);
        };
        
        ///
        /// \typedef an argument map (argument_name -> argument_value)
        ///
        using Arguments = std::unordered_map<std::string, std::string>;
        
        ///
        /// \class a representation of the call that started the application
        ///
        struct Call{
            
            ///
            /// the current application run folder
            ///
            boost::filesystem::path application_run_path;
            
            ///
            /// a map with the arguments of the current application
            ///
            Arguments arguments;
            
            ///
            /// Constructs a new, empty Call
            ///
            Call();
        };
        
        ///
        /// Prints the call in a human readable format
        /// \param output the output stream
        /// \param call the call to print
        /// \return the output stream
        ///
        std::ostream &operator<<(std::ostream &output, const Call &call);
        
        ///
        /// \class represents a definition of a CLI argument
        ///
        struct ArgumentDefinition{
            ///
            /// the name of the argument, used as a key in the argument map
            ///
            std::string name;
            
            ///
            /// the long version of the switch for this argument
            /// e.g. long_switch "version" and name "appVersion" results in a CLI argument "--version 1.0" being parsed as "appVersion" -> "1.0" in the argument map
            ///
            std::string long_switch;
            
            ///
            /// the short version of the switch for this argument
            /// e.g. short_switch "v" and name "appVersion" results in a CLI argument "-v1.0" or "-v 1.0" being parsed as "appVersion" -> "1.0" in the argument map
            ///
            char short_switch;
            
            ///
            /// the default value of this argument, ignored if required is true
            ///
            std::string default_value;
            
            ///
            /// if set to true, an ArgumentError will be thrown when the arguments are parsed if the argument is not present
            /// Note: default_value is ignored if this parameter is set to true
            ///
            bool required;
            
            ///
            /// Creates a new argument definition
            ///
            /// \param name the name of the argument (e.g. "appVersion")
            /// \param long_switch the long switch of the argument (e.g. "version")
            /// \param short_switch the short switch of the argument (e.g. "v")
            /// \param default_value the default value of this argument (e.g. "1.0.0"), or an empty string to denote the argument has no default value
            /// \param required set to true if the argument is required for the application to run properly
            ///
            ArgumentDefinition(const std::string &name,  const std::string &long_switch, char short_switch, const std::string &default_value, bool required);
        };
        
        ///
        /// \class a parser that interprets the command line arguments and stores them in a convenient format
        ///
        class ArgumentParser{
        public:
            
            ///
            /// Creates a new argument parser
            ///
            ArgumentParser();

            ///
            /// Creates a new argument parser
            /// \param argument_definitions the argument definitions for this parser
            /// \throw ArgumentError if there are conflicting argument definitions
            ///
            ArgumentParser(std::initializer_list<ArgumentDefinition> argument_definitions);
            
            ///
            /// Destroys this argument parser
            ///
            ~ArgumentParser();
            
            ///
            /// Adds a new argument definition
            /// \param argument_definition the argument definition
            /// \throw ArgumentError if there are conflicting argument definitions
            ///
            ArgumentParser &add(const ArgumentDefinition &argument_definition);

            ///
            /// Parses the command line input into the call object
            /// \param call to store the result in
            /// \param arg_count the number of unparsed arguments
            /// \param args the unparsed arguments
            /// \throw ArgumentError if there was an error in parsing the arguments
            ///
            void parse(Call &call, int arg_count, const char **args) const;
            
            ///
            /// Parses the command line input into the call object
            /// \param arg_count the number of unparsed arguments
            /// \param args the unparsed arguments
            /// \throw ArgumentError if there was an error in parsing the arguments
            /// \return the resulting parameters wrapped into a Call
            ///
            Call parse(int arg_count, const char **args) const;
            
        private:
            std::unordered_map<std::string, const ArgumentDefinition *> argument_definitions_;
            std::unordered_map<char, const ArgumentDefinition *> short_switches_;
            std::unordered_map<std::string, const ArgumentDefinition *> long_switches_;
        };
        
    }
    
}

#endif	/* CLI_H */

