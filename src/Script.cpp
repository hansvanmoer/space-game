#include "Script.h"
#include "Name.h"
#include "Log.h"

#include <boost/python.hpp>
#include <python3.4m/pythonrun.h>

using namespace std;
using namespace Game;
using namespace Game::Script;
using namespace boost::python;

static Log::Logger logger = Log::create_logger("default");

ScriptError::ScriptError(const std::string& message) : runtime_error(message) {
}

Writer::Writer(){}

Writer::~Writer(){
}

LoggerWriter::LoggerWriter(const Log::Logger& logger, Log::Level level) : logger_(logger, level), buffer_(), dirty_(){}

LoggerWriter::~LoggerWriter(){
    flush();
}

void LoggerWriter::write(const char *output){
    buffer_ << output;
    dirty_ = true;
}

void LoggerWriter::flush(){
    if(dirty_){
        logger_(buffer_.str());
        buffer_.str("");
        buffer_.clear();
        dirty_ = false;
    }
}

Run::Run(const std::string& context_id) : context_id_(context_id){
}

Run::~Run(){}

const std::string& Run::context_id() const {
    return context_id_;
}

RunCode::RunCode(const std::string& context_id, const std::string& code) : Run(context_id), code_(code){
}

void RunCode::execute(boost::python::object globals, boost::python::object locals) const {
    boost::python::exec(code_, globals, locals);
}

RunFile::RunFile(const std::string& context_id, const boost::filesystem::path& file) : Run(context_id), file_(file){}

void RunFile::execute(boost::python::object globals, boost::python::object locals) const{
    boost::python::exec_file(file_.c_str(), globals, locals);
};

const ApplicationId ScriptSystem::id{"script"};

ScriptSystem::ScriptSystem() : run_mutex_(), running_(), main_module_(), main_dict_(), current_module_(), after_start_(){
    
}

void ScriptSystem::initialize_module(const string &module_id, void (*init_function)(void)){
    lock_guard<mutex> guard{run_mutex_};
    if(running_){
        throw ScriptError{string{"can't import module '"}+module_id+"' while the interpreter is running"};
    }else{
        PyImport_AppendInittab(module_id.c_str(), init_function);
    }
}

bool ScriptSystem::start(){
    lock_guard<mutex> guard{run_mutex_};
    if(!running_){
        Py_Initialize();
        main_module_ = import("__main__");
        main_dict_ = main_module_.attr("__dict__");
        current_module_ = main_module_;
        
        for(Run *run : after_start_){
            run->execute(current_module_, current_module_);
        }
        for(Run *run : after_start_){
            delete run;
        }
        after_start_.clear();
        return true;
    }else{
        return false;
    }
};

void ScriptSystem::run(const string &code){
    lock_guard<mutex> guard{run_mutex_};
    if(running_){
        boost::python::str buffer{code};
        exec(buffer, current_module_, current_module_);
    }else{
        throw ScriptError{"can't run code: interpreter not running"};
    }
};

void ScriptSystem::run(const boost::filesystem::path &script_path){
    lock_guard<mutex> guard{run_mutex_};
    if(running_){
        boost::python::str buffer{script_path.c_str()};
        exec_file(buffer, current_module_, current_module_);
    }else{
        throw ScriptError{"can't run code: interpreter not running"};
    }
};

void ScriptSystem::run_after_start(const std::string& code, const std::string& context_id) {
    after_start_.push_back(new RunCode{context_id, code});
}

void ScriptSystem::run_after_start(const boost::filesystem::path& script_path, const std::string& context_id){
    after_start_.push_back(new RunFile{context_id, script_path});
}

void ScriptSystem::force_stop(){
    Py_Finalize();
}

bool ScriptSystem::stop(){
    lock_guard<mutex> guard{run_mutex_};
    if(running_){
        running_ = false;
        Py_Finalize();
        return true;
    }else{
        return false;
    }
}

ScriptSystem::~ScriptSystem(){
    for(Run *run : after_start_){
        delete run;
    }
    try{
        force_stop();
    }catch(std::exception &e){
        logger.error("unable to close script system:",e.what());
    }catch(...){
        logger.error("unable to close script system: unknown error");
    }
};



/*
void ScriptSystem::run(Runnable *script){
   lock_guard<mutex> run_lock{run_mutex_};
   do_run(script);
}

bool ScriptSystem::try_run(Runnable *script){
    if(run_mutex_.try_lock()){
        lock_guard<mutex> run_lock{run_mutex_, adopt_lock};
        do_run(script);
        return true;
    }else{
        return false;
    }
}

void ScriptSystem::do_run(Runnable *script){
    using namespace boost::python;
    current_script_ = script;
    PyImport_AppendInittab("GameUtils", &initGameUtils);
    current_script_->before_initialize();
    Py_Initialize();
    object main_module = import("__main__");
    object main_namespace = main_module.attr("__dict__");
    current_script_->before_run(main_namespace);
    try {
        exec(redirect_logger_code, main_namespace, main_namespace);
        object result = exec(current_script_->code_, main_namespace, main_namespace);
        current_script_->before_finalize(result);
    } catch (boost::python::error_already_set const &) {
        PyErr_Print();
    } catch(std::exception &e){
        logger.error("an error occurred while running script", e.what());
    }catch(...){
        logger.error("an unknown error occurred while running script");
    }
    Py_Finalize();
    current_script_ = nullptr;
}

Runnable &ScriptSystem::current_script(){
    return *current_script_;
};
 * 
 * */