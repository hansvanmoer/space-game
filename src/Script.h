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

#include <string>
#include <stdexcept>
#include <sstream>
#include <mutex>
#include <unordered_map>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

namespace Game {

    class ScriptError : public std::runtime_error{
    public:
        ScriptError(const std::string &message);
    };
    
    class Script{
    public:
        virtual boost::python::object execute(boost::python::object globals, boost::python::object locals) const = 0;
        
        const std::string &name() const;
        
    protected:
        Script(const std::string &name);
    
    private:
        std::string name_;
    };
    
    class BufferedScript : public Script{
    public:
        
        BufferedScript(const std::string &name, const std::string &code);
        
        BufferedScript(const std::string &name, std::string &&code);
        
        boost::python::object execute(boost::python::object globals, boost::python::object locals) const;
        
    private:
        std::string code_;
    };
    
    class ScriptFile : public Script{
    public:
        ScriptFile(const std::string &name, const boost::filesystem::path &path);
        
        ScriptFile(const boost::filesystem::path &path);
        
        boost::python::object execute(boost::python::object globals, boost::python::object locals) const;
                
    private:
        boost::filesystem::path path_;
    };
    
    class ScriptWriter{
    public:
        ScriptWriter();
        
        ~ScriptWriter();
        
        void write(const std::string &message);

        void flush();
        
    private:
        std::ostringstream buffer_;
        bool dirty_;
        
        ScriptWriter(const ScriptWriter &) = delete;
        ScriptWriter &operator=(const ScriptWriter &) = delete;
    };
    
    class ScriptContext{
    public:
        
        ScriptContext(const std::string &module_name);
        
        ScriptContext(const std::string &module_name, const std::vector<std::string> &available_modules);
        
        ScriptContext(const std::string &module_name, std::vector<std::string> &&available_modules);
        
        const std::string &module_name() const;
        
        const std::vector<std::string> &available_modules() const;
        
    private:
        std::string module_name_;
        std::vector<std::string> available_modules_;
    };
    
    class ScriptSystem{
    public:
        
        static const ApplicationId id;
        
        ScriptSystem();
        
        void run(const ScriptContext &context, const Script &script);
        
        ~ScriptSystem();
        
        ScriptWriter &writer();
        
        template<typename Callable> auto evaluate_in_module(const std::string &module_name, Callable callable) -> decltype(callable(boost::python::object{})){
            using namespace std;
            using namespace boost::python;
            lock_guard<mutex> lock{mutex_};
            try{
                return callable(main_module_.attr("__dict__")[module_name].attr("__dict__"));
            }catch(error_already_set &e){
                PyErr_Print();
                throw ScriptError{string{"an error has occurred while executing script in module "}+module_name};
            }catch(exception &e){
                throw ScriptError{string{"an error has occurred while executing script in module "}+module_name+string{" : "}+e.what()};
            }catch(...){
                throw ScriptError{string{"an unknown error has occurred while executing script in module "}+module_name};
            }
        };
        
        template<typename Result, typename... Args> Result evaluate_function(const std::string &module_name, const std::string &function_name, Args && ... args){
            using namespace std;
            using namespace boost::python;
            lock_guard<mutex> lock{mutex_};
            try{
                return static_cast<Result>(extract<Result>(main_module_.attr("__dict__")[module_name].attr("__dict__")[function_name](move<Args>(args)...)));
            }catch(error_already_set &e){
                PyErr_Print();
                throw ScriptError{string{"an error has occurred while executing function "}+module_name+"::"+function_name};
            }catch(exception &e){
                throw ScriptError{string{"an error has occurred while executing function "}+module_name+"::"+function_name+string{" : "}+e.what()};
            }catch(...){
                throw ScriptError{string{"an unknown error has occurred while executing function "}+module_name+"::"+function_name};
            }
        };

    private:
        ScriptWriter writer_;
        std::mutex mutex_;
        boost::python::object main_module_;
    };

    class ScriptWriterHandle{
    public:
        void write(const std::string &message);

        void flush();
    };
    
}

#endif	/* SCRIPT_H */

