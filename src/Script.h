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
#include "ThreadPool.h"

#include <string>
#include <stdexcept>
#include <sstream>
#include <future>
#include <unordered_map>
#include <functional>

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
    
    class ScriptCallResult{
    public:
        ScriptCallResult(std::future<boost::python::object> &&future);
        
        ScriptCallResult(ScriptCallResult &&result);
        
        ScriptCallResult &operator=(ScriptCallResult &&result);
        
        template<typename T> T get(){
            return static_cast<T>(boost::python::extract<T>(future_.get()));
        };
        
    private:
        std::future<boost::python::object> future_;
        
        ScriptCallResult(const ScriptCallResult &) = delete;
        ScriptCallResult &operator=(const ScriptCallResult &) = delete;
    };
    
    class ScriptSystem{
    public:
        
        static const ApplicationId id;
        
        ScriptSystem(std::size_t executor_thread_count = 2);
        
        void run(const ScriptContext &context, const Script &script);
        
        ~ScriptSystem();
        
        ScriptWriter &writer();
        
        template<typename Callable> auto evaluate_in_module(const std::string &module_name, Callable callable) -> decltype(callable(boost::python::object{})){
            using namespace std;
            using namespace boost::python;
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
        
        struct NamedFunction{
            std::string module_name;
            std::string name;
            boost::python::object function;
        };
        
        template<typename... Args> static boost::python::object call_named_function(const NamedFunction &named_function, Args... args){
            using namespace std;
            using namespace boost::python;
            
            GILGuard guard;
            try{
                return named_function.function(args...);
            }catch(error_already_set &e){
                PyErr_Print();
                throw ScriptError{string{"an error has occurred while executing function "}+named_function.module_name+"::"+named_function.name};
            }catch(exception &e){
                throw ScriptError{string{"an error has occurred while executing function "}+named_function.module_name+"::"+named_function.name+string{" : "}+e.what()};
            }catch(...){
                throw ScriptError{string{"an unknown error has occurred while executing function "}+named_function.module_name+"::"+named_function.name};
            }
        };
    public:
        
        template<typename... Args> auto bind(const std::string &module_name, const std::string &function_name, Args ... args) -> decltype(std::bind(&call_named_function<Args...>, NamedFunction{}, std::forward<Args>(args) ...)){
            using namespace std;
            using namespace boost::python;
            NamedFunction named_function{module_name, function_name, main_module_.attr("__dict__")[module_name].attr("__dict__")[function_name]};
            
            return std::bind(&call_named_function<Args...>, named_function, args...);
        };
        
        template<typename Function> ScriptCallResult submit_call(Function function){
            using namespace std;
            using namespace boost::python;
            packaged_task<object ()> task{function};
            ScriptCallResult result{task.get_future()};
            executors_.submit(move(task));
            return move(result);
        };
        
        /*
        template<typename Result, typename... Args> std::future<Result> submit_call(const std::string &module_name, const std::string &function_name, Args && ... args){
            using namespace std;
            using namespace boost::python;
            object func{main_module_.attr("__dict__")[module_name].attr("__dict__")[function_name]};
            
            packaged_task<Result ()> task{bind(&ScriptSystem::execute_call<Result,Args...>, module_name, function_name, func, forward<Args>(args)...)};
            //future<Result> result{task.get_future()};
            executors_.submit(move(task));
            //return result;
        };*/
        
    private:
        ScriptWriter writer_;
        FixedThreadPool executors_;
        boost::python::object main_module_;
        PyThreadState *main_thread_state_;
        
        class GILGuard{
        public:
            GILGuard();
            
            ~GILGuard();
        private:
            PyGILState_STATE gstate_;
        };
    };

    class ScriptWriterHandle{
    public:
        void write(const std::string &message);

        void flush();
    };
    
}

#endif	/* SCRIPT_H */

