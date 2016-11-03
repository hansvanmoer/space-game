/* 
 * File:   Script.h
 * Author: hans
 *
 * Created on October 14, 2016, 7:46 PM
 */

#ifndef GAME_SCRIPT_H
#define	GAME_SCRIPT_H

#include "Application.h"
#include "Log.h"

#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <memory>
#include <vector>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

namespace Game {

    namespace Script {

        class ScriptError : public std::runtime_error{
        public:
            ScriptError(const std::string &message);
        };
        
        class Writer{
        public:
            
            virtual ~Writer();
            
            virtual void write(const char *output) = 0;
            
            virtual void flush() = 0;
        protected:
            Writer();
        private:
            Writer(const Writer &) = delete;
            Writer &operator=(const Writer &) = delete;
        };
                        
        class LoggerWriter : public Writer{
        public:
            
            LoggerWriter(const Log::Logger &logger, Log::Level level = Log::Level::debug);
            
            ~LoggerWriter();
            
            void write(const char *output);
            
            void flush();
            
        private:
            Log::LogLinesAdapter<std::string> logger_;
            std::ostringstream buffer_;
            bool dirty_;
        };
        
        class Run{
        public:
            ~Run();
            
            virtual void execute(boost::python::object globals, boost::python::object locals) const = 0;
            
            const std::string &context_id() const;
            
        protected:
            Run(const std::string &context_id);
            
        private:
            std::string context_id_;
        };
        
        class RunCode : public Run{
        public:
            RunCode(const std::string &context_id, const std::string &code);
                        
            void execute(boost::python::object globals, boost::python::object locals) const;
            
        private:
            boost::python::str code_;
        };

        class RunFile : public Run{
        public:
            RunFile(const std::string &context_id, const boost::filesystem::path &file);
            
            void execute(boost::python::object globals, boost::python::object locals) const;
            
        private:
            boost::filesystem::path file_;
        };

        
        class ScriptSystem{
        private:
            std::mutex run_mutex_;
            bool running_;
            
            boost::python::object main_module_;
            boost::python::object main_dict_;
            boost::python::object current_module_;
            
            std::vector<Run *> after_start_;
            
            void force_stop();
            
            ScriptSystem(const ScriptSystem &) = delete;
            ScriptSystem &operator=(const ScriptSystem &) = delete;

        public:

            static const ApplicationId id;
            
            ScriptSystem();
            
             ~ScriptSystem();
             
             void set_context(const std::string &python_module_id);
             
             void reset_context();
             
             void initialize_module(const std::string &module_id, void (*init_function)(void));
             
             bool start();
             
             void run(const std::string &code);
             
             void run(const boost::filesystem::path &script_path);

             void run_after_start(const std::string &code, const std::string &context_id = std::string{});
             
             void run_after_start(const boost::filesystem::path &script_path, const std::string &context_id = std::string{});
             
             bool stop();
             
             template<typename Runnable> auto run(Runnable &&runnable) -> decltype(runnable(current_module_)){
                 std::lock_guard<std::mutex> guard{run_mutex_};
                 return run(current_module_);
             };

        };

    }
    
    using ScriptSystem = Script::ScriptSystem;

}

#endif	/* SCRIPT_H */

