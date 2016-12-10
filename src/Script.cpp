#include "Script.h"
#include "Log.h"

#include "Name.h"

#include <utility>
#include <python2.7/ceval.h>

using namespace Game;
using namespace std;

static Log::Logger logger = Log::LoggerFactory::instance().create_logger("default");

ScriptError::ScriptError(const string& message) : runtime_error(message){};

Script::Script(const string& name) : name_(name){
}

const string& Script::name() const {
    return name_;
}

BufferedScript::BufferedScript(const string& name, const string& code) : Script(name), code_(code){
}

BufferedScript::BufferedScript(const string& name, string &&code) : Script(name), code_(move(code)){}

boost::python::object BufferedScript::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str code_str{code_};
    return exec(code_str, globals, locals);
}

ScriptFile::ScriptFile(const string& name, const boost::filesystem::path& path) : Script(name), path_(path){
}

ScriptFile::ScriptFile(const boost::filesystem::path& path) : ScriptFile(path.filename().string(), path){
}

boost::python::object ScriptFile::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str path_str{path_.string()};
    return exec_file(path_str, globals, locals);
}

ScriptContext::ScriptContext(const string &module_name) : module_name_(module_name), available_modules_(){};

ScriptContext::ScriptContext(const string& module_name, const vector<string>& available_modules) : module_name_(module_name), available_modules_(available_modules){}

ScriptContext::ScriptContext(const string& module_name, vector<string> && available_modules) : module_name_(module_name), available_modules_(move(available_modules)){
}

const std::string& ScriptContext::module_name() const {
    return module_name_;
}

const std::vector<std::string>& ScriptContext::available_modules() const {
    return available_modules_;
}

ScriptWriter::ScriptWriter() : buffer_(), dirty_(){
}

void ScriptWriter::write(const string& message) {
    buffer_ << message;
    dirty_ = true;
}

void ScriptWriter::flush() {
    if(dirty_){
        Log::Logger script_logger = Log::LoggerFactory::instance().create_logger("default");
        script_logger.info_lines(buffer_.str());
        buffer_.str("");
        buffer_.clear();
        dirty_ = false;
    }
}

ScriptWriter::~ScriptWriter(){
    flush();
};

void ScriptWriterHandle::write(const string &message){
    ApplicationSystem<ScriptSystem>::instance().writer().write(message);
}

void ScriptWriterHandle::flush(){
    ApplicationSystem<ScriptSystem>::instance().writer().flush();
}

BOOST_PYTHON_MODULE(GameUtils){
    using namespace boost::python;
    class_<ScriptWriterHandle>("ScriptWriter")
            .def("write", &ScriptWriterHandle::write)
            .def("flush", &ScriptWriterHandle::flush);
}

BOOST_PYTHON_MODULE(GameUtilsExt){}

BOOST_PYTHON_MODULE(NameGenerator){
    using namespace boost::python;
    
    class_<BufferedStringPool>("StringPool", init<bool,bool>())
        .def("add", &BufferedStringPool::add)
        .def("next", &BufferedStringPool::next)
        .def("peek", &BufferedStringPool::peek)
        .def("has_more", &BufferedStringPool::has_more)
        .def("reset", &BufferedStringPool::reset)
        .def("load_from_file", &BufferedStringPool::load_from_file);
}

BOOST_PYTHON_MODULE(NameGeneratorExt){
}

ScriptCallResult::ScriptCallResult(std::future<boost::python::object> &&result_future) : future_(forward<future<boost::python::object>>(result_future)){};


ScriptCallResult::ScriptCallResult(ScriptCallResult &&result) : future_(){
    swap(future_, result.future_);
};

ScriptCallResult &ScriptCallResult::operator=(ScriptCallResult &&result){
    swap(future_, result.future_);
    return *this;
};

ScriptSystem::GILGuard::GILGuard(){
    gstate_ = PyGILState_Ensure();
}

ScriptSystem::GILGuard::~GILGuard() {
    PyGILState_Release(gstate_);
}


const ApplicationId ScriptSystem::id{"script"};

ScriptSystem::ScriptSystem(size_t executor_thread_count) : writer_(), executors_(executor_thread_count){
    using namespace boost::python;
    
    Py_Initialize();
    initGameUtils();
    initGameUtilsExt();
    initNameGenerator();
    initNameGeneratorExt();
    main_module_ = import("__main__");
    
    run(ScriptContext{"GameUtilsExt"}, BufferedScript{"init_script_system",
            "import GameUtils\n"
            "import sys\n"
            "sys.stdout=GameUtils.ScriptWriter()\n"
    });
    
    //Acquiring interpreter lock and starting threading
    PyEval_InitThreads();
    main_thread_state_ = PyThreadState_Get();
    
    //Release interpreter lock
    PyEval_ReleaseLock();
    
    executors_.start();
}

ScriptSystem::~ScriptSystem(){
    executors_.stop();
    
    // IMPORTANT: I need to acquire the GIL before attempting to finalize
    PyEval_AcquireLock();
    
    // Swap main thread state back in: otherwise the thread state of the last executed thread is used and Py_Finalize segfaults
    PyThreadState_Swap(main_thread_state_); 
    Py_Finalize();    
}

ScriptWriter &ScriptSystem::writer(){
    return writer_;
}

void ScriptSystem::run(const ScriptContext &context, const Script& script) {
    GILGuard guard;
    using namespace boost::python;
    try{
        object extension_module = import(context.module_name().c_str());
        object main_namespace = main_module_.attr("__dict__");
        object extension_namespace = extension_module.attr("__dict__");
        for(string dependency_name : context.available_modules()){
            if(!main_namespace.contains(dependency_name.c_str())){
                main_namespace[dependency_name.c_str()] = import(dependency_name.c_str());
            }
        }
        object result = script.execute(main_namespace, extension_namespace);
        main_namespace[context.module_name()] = extension_module;
    }catch(error_already_set &e){
        PyErr_Print();
        throw ScriptError{string{"an error has occurred while executing script "}+script.name()};
    }catch(exception &e){
        throw ScriptError{string{"an error has occurred while executing script "}+script.name()+string{":"}+e.what()};
    }catch(...){
        throw ScriptError{string{"an unknown error has occurred while executing script "}+script.name()};
    }
    writer_.flush();
}