///
/// \file Contains a simple logging framework
///

#ifndef LOG_H
#define	LOG_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace Game {

    namespace Log {

        ///
        /// \class Models the log level
        ///

        enum class Level {
            none, debug, info, warning, error, fatal
        };

        ///
        /// \typedef a logger id type
        ///
        using Id = std::string;

        class Logger;

        class LoggerFactory;

        ///
        /// \class A logger's output handle, this extra level of indirection exists so it is possible to redirect log output after loggers have been created
        ///

        class Output {
        public:

            ///
            /// Creates an output with log level NONE and all streams set to the specified ignore stream
            /// \param factory the LoggerFactory that created this output
            /// \param id the logger's ID
            ///
            Output(LoggerFactory &factory, const Id &id);

            ///
            /// Configures this output
            /// All corresponding streams will be either set to the supplied stream or to the logger factory ignore stream
            ///
            void configure(Level level, std::ostream *stream);

        private:
            std::ostream *debug_output_;
            std::ostream *info_output_;
            std::ostream *warning_output_;
            std::ostream *error_output_;
            std::ostream *fatal_output_;

            LoggerFactory &factory_;
            Level level_;
            Id id_;

            friend class Logger;
        };


        ///
        /// \class A stream buffer that ignores all input
        ///

        class IgnoreStreamBuffer : public std::streambuf {
        protected:
            virtual std::ostream::int_type overflow(std::ostream::int_type c);
        private:
            char buffer_[23];
        };

        ///
        /// \class An output stream that ignores all input
        ///

        class IgnoreStream : private IgnoreStreamBuffer, public std::ostream {
        public:
            IgnoreStream();
        };

        ///
        /// \class a singleton class that manages all loggers, their output and their streams
        ///

        class LoggerFactory {
        public:

            ///
            /// Configures a logger's output for the specified id
            /// If the logger already exists, it is reconfigured
            /// \param id the logger's id
            /// \param level the log level
            /// \param stream the logger's new output stream
            /// \param owned true if the stream should be destroyed after the logger factory shuts down, false otherwise
            /// \return true if the logger was created, false if an existing logger was reconfigured
            bool configure(const Id &id, Level level, std::ostream *stream, bool owned);

            ///
            /// Creates a new logger from the output specified by the id
            /// Note: the logger can be used before it is configured but will ignore all input
            /// \param id names the output to be used
            /// \return a new logger attached to the output specified by the id
            ///
            Logger create_logger(const Id &id);

            ///
            /// Returns the logger factory instance
            /// \return the instance
            ///
            static LoggerFactory &instance();

            ///
            /// Returns a pointer to this logger factory's ignore stream
            /// \return a pointer to an ostream that ignores all output
            ///
            std::ostream *ignore();

            ///
            /// Destroys this factory, all outputs and owned streams
            ///
            ~LoggerFactory();

        private:
            LoggerFactory();

            IgnoreStream ignore_;
            std::unordered_map<Id, Output *> outputs_;
            std::unordered_map<std::ostream *, bool> streams_;
        };

        namespace Policies {

            template<typename Arg, typename... Args> struct Append {
            public:

                static void execute(std::ostream &output, char delimiter, Arg &&value, Args && ... values) {
                    output << delimiter << value;
                    Append < Args...>::execute(output, delimiter, std::forward <Args>(values) ...);
                };
            };

            template<typename Arg> struct Append<Arg> {
            public:

                static void execute(std::ostream &output, char delimiter, Arg &&value) {
                    output << delimiter << value;
                };
            };

        }

        ///
        /// \class a logger class that acts as a handler to an Output
        ///

        class Logger {
        public:

            ///
            /// Creates a new logger
            /// \param the output for this logger
            ///
            Logger(const Output *output);

            ///
            /// Logs the specified values to the debug output
            /// The values are printed on a single line, preceded by a the prefix 'DEBUG:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &debug(Arg &&value, Args &&... values) {
                return log<Arg, Args...>(*output_->debug_output_, "DEBUG:  ", std::forward<Arg>(value), std::forward < Args>(values)...);
            };

            ///
            /// Logs the specified values to the debug output over multiple lines
            /// The values are printed on a single line, preceded by a the prefix 'DEBUG:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &debug_lines(Arg &&value, Args &&... values) {
                return log_lines<Arg, Args...>(*output_->debug_output_, "DEBUG:  ", std::forward<Arg>(value), std::forward < Args>(values)...);
            };


            ///
            /// Logs the specified values to the info output
            /// The values are printed on a single line, preceded by a the prefix 'INFO:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &info(Arg &&value, Args &&... values) {
                return log< Arg, Args...>(*output_->info_output_, "INFO:   ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the info output over multiple lines
            /// The values are printed on a single line, preceded by a the prefix 'INFO:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &info_lines(Arg &&value, Args &&... values) {
                return log_lines< Arg, Args...>(*output_->info_output_, "INFO:   ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the warning output
            /// The values are printed on a single line, preceded by a the prefix 'WARNING:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &warning(Arg &&value, Args &&... values) {
                return log<Arg, Args...>(*output_->warning_output_, "WARNING:", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the warning output over multiple lines
            /// The values are printed on a single line, preceded by a the prefix 'WARNING:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &warning_lines(Arg &&value, Args &&... values) {
                return log_lines<Arg, Args...>(*output_->warning_output_, "WARNING:", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the error output
            /// The values are printed on a single line, preceded by a the prefix 'ERROR:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &error(Arg &&value, Args &&... values) {
                return log<Arg, Args...>(*output_->error_output_, "ERROR:  ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the error output over multiple lines
            /// The values are printed on a single line, preceded by a the prefix 'ERROR:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &error_lines(Arg &&value, Args &&... values) {
                return log_lines<Arg, Args...>(*output_->error_output_, "ERROR:  ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the fatal output
            /// The values are printed on a single line, preceded by a the prefix 'FATAL:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &fatal(Arg &&value, Args &&... values) {
                return log<Arg, Args...>(*output_->fatal_output_, "FATAL:  ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Logs the specified values to the fatal output over multiple lines
            /// The values are printed on a single line, preceded by a the prefix 'FATAL:' and separated by spaces
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &fatal_lines(Arg &&value, Args &&... values) {
                return log_lines<Arg, Args...>(*output_->fatal_output_, "FATAL:  ", std::forward<Arg>(value), std::forward < Args...>(values)...);
            };

            ///
            /// Does not log the specified lines, this method is effectively a no-op
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &none(Arg &&value, Args &&... values) {
                return *this;
            };

            ///
            /// Does not log the specified lines, this method is effectively a no-op
            /// \param value the first value
            /// \param values optional trailing values
            /// \return a reference to this logger to allow chaining
            ///

            template<typename Arg, typename... Args> Logger &none_lines(Arg &&value, Args &&... values) {
                return *this;
            };

        private:

            template<typename Arg, typename... Args> Logger &log(std::ostream &output, const char *prefix, Arg &&value, Args &&... values) {
                output << prefix;
                Policies::Append<Arg, Args...>::execute(output, ' ', std::forward<Arg>(value), std::forward < Args>(values) ...);
                output << std::endl;
                return *this;
            };

            template<typename Arg, typename... Args> Logger &log_lines(std::ostream &output, const char *prefix, Arg &&value, Args &&... values) {
                output << prefix;
                Policies::Append<Arg, Args...>::execute(output, '\n', std::forward<Arg>(value), std::forward < Args>(values) ...);
                output << std::endl;
                return *this;
            };

            const Output *output_;
        };

        template<typename Arg, typename... Args> class Adapter {
        public:

            using Function = Logger & (Logger::*)(Arg &&, Args && ...);
            
            Adapter(const Logger &logger, Logger & (Logger::*log)(Arg &&, Args && ...) = &Logger::debug) : logger_(logger), log_(log) {};

            void log(Arg &&value, Args &&... values) {
                (logger_.*log_)(std::forward<Arg>(value), std::forward<Args>(values)...);
            };

            void operator()(Arg &&value, Args &&... values) {
                log(std::forward<Arg>(value), std::forward<Args>(values)...);
            };
        private:
            Logger logger_;
            Logger & (Logger::*log_)(Arg &&, Args && ...);
        };

        template<typename Arg, typename... Args> class LogAdapter : public Adapter<Arg, Args...>{
        public:
            LogAdapter(const Logger &logger, Level level) : Adapter<Arg,Args...>(logger, get_func(level)){};
        
        private:
            static typename Adapter<Arg,Args...>::Function get_func(Level level){
                switch(level){
                    case Level::debug:
                        return &Logger::debug;
                    case Level::info:
                        return &Logger::info;
                    case Level::warning:
                        return &Logger::warning;
                    case Level::error:
                        return &Logger::error;
                    case Level::fatal:
                        return &Logger::fatal;
                    default:
                        return &Logger::none;
                }
            };
        };
        
        template<typename Arg, typename... Args> class LogLinesAdapter : public Adapter<Arg, Args...>{
        public:
            LogLinesAdapter(const Logger &logger, Level level) : Adapter<Arg,Args...>(logger, get_func(level)){};
        
        private:
            static typename Adapter<Arg,Args...>::Function get_func(Level level){
                switch(level){
                    case Level::debug:
                        return &Logger::debug_lines;
                    case Level::info:
                        return &Logger::info_lines;
                    case Level::warning:
                        return &Logger::warning_lines;
                    case Level::error:
                        return &Logger::error_lines;
                    case Level::fatal:
                        return &Logger::fatal_lines;
                    default:
                        return &Logger::none_lines;
                }
            };
        };

        ///
        /// A shorthand method to easily configure logger outputs
        /// See LogFactory::configure for more details
        /// \param id the id of the output
        /// \param stream a pointer to an output stream
        /// \param owned indicates whether the stream should be destroyed after the factory closes
        /// \return true if a new logger output was configured, false if an existing one was configured
        ///
        bool configure_logger(const Id &id, Level level, std::ostream *stream, bool owned = false);

        ///
        /// A shorthand method to easily create loggers
        /// See LogFactory::create_logger for more details
        /// \param id the id of the output to connect to
        /// \return a new logger connected to the output
        ///
        Logger create_logger(const Id &id);

    }



}

#endif	/* LOG_H */

