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

        const std::string &module_id() const;
        
    protected:
        Script(const std::string &name, const std::string &module_id);
    
    private:
        std::string name_;
        std::string module_id_;
    };
    
    class BufferedScript : public Script{
    public:
        
        BufferedScript(const std::string &name, const std::string &module_id, const std::string &code);
        
        BufferedScript(const std::string &name, const std::string &module_id, std::string &&code);
        
        boost::python::object execute(boost::python::object globals, boost::python::object locals) const;
        
    private:
        std::string code_;
    };
    
    class ScriptFile : public Script{
    public:
        ScriptFile(const std::string &name, const std::string &module_id, const boost::filesystem::path &path);
        
        ScriptFile(const std::string &module_id, const boost::filesystem::path &path);
        
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
    
    class ScriptSystem{
    public:
        
        static const ApplicationId id;
        
        ScriptSystem();
        
        void run(const Script &script);
        
        ~ScriptSystem();
        
        ScriptWriter &writer();
        
        boost::python::object extension(const std::string &module_id);
        
        template<typename Callable> auto call(const std::string &module_id, const std::string &function_name, Callable callable) -> decltype(callable(boost::python::object{})){
            using namespace std;
            using namespace boost::python;
            lock_guard<mutex> lock{mutex_};
            try{
                return callable(extensions_[module_id].attr("__dict__")[function_name]);
            }catch(error_already_set &e){
                PyErr_Print();
                throw ScriptError{string{"an error has occurred while executing function "}+module_id+"::"+function_name};
            }catch(exception &e){
                throw ScriptError{string{"an error has occurred while executing function "}+module_id+"::"+function_name+string{" : "}+e.what()};
            }catch(...){
                throw ScriptError{string{"an unknown error has occurred while executing function "}+module_id+"::"+function_name};
            }
        };

    private:
        ScriptWriter writer_;
        std::mutex mutex_;
        std::unordered_map<std::string, boost::python::object> extensions_;
    };

    class ScriptWriterHandle{
    public:
        void write(const std::string &message);

        void flush();
    };
    
}

#endif	/* SCRIPT_H */

