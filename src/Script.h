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

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

namespace Game {

    class ScriptError : public std::runtime_error{
    public:
        ScriptError(const std::string &message);
    };
    
    class Script{
    public:
        virtual void execute(boost::python::object globals, boost::python::object locals) const = 0;
        
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
        
        void execute(boost::python::object globals, boost::python::object locals) const;
        
    private:
        std::string code_;
    };
    
    class ScriptFile : public Script{
    
        ScriptFile(const std::string &name, const boost::filesystem::path &path);
        
        ScriptFile(const boost::filesystem::path &path);
        
        void execute(boost::python::object globals, boost::python::object locals) const;
        
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
        
    private:
        ScriptWriter writer_;
    };

    class ScriptWriterHandle{
    public:
        void write(const std::string &message);

        void flush();
    };
    
}

#endif	/* SCRIPT_H */

