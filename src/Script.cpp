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

ObjectError::ObjectError(const std::string& message) : ScriptError(message) {
}

DuplicateObjectError::DuplicateObjectError(const std::string& id) : ObjectError(string{"duplicate object id: "}
+id) {
}

NoSuchObjectError::NoSuchObjectError(const std::string& id) : ObjectError(string{"unknown object id: "}
+id) {
}

NullHandleError::NullHandleError() : ObjectError("null handle dereference") {
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
    }
}

IgnoreWriter::IgnoreWriter() {}

IgnoreWriter::~IgnoreWriter(){}

void IgnoreWriter::write(const char* output){}

void IgnoreWriter::flush(){};

StandardWriter::StandardWriter(){}

StandardWriter::StandardWriter(const StandardWriter &){}

StandardWriter &StandardWriter::operator=(const StandardWriter &){
    return *this;
};

StandardWriter::~StandardWriter() {}

void StandardWriter::write(const char *output){
    ApplicationSystem<ScriptSystem>::instance().current_script().standard_output().write(output);
};

void StandardWriter::flush(){
    ApplicationSystem<ScriptSystem>::instance().current_script().standard_output().flush();
};

ErrorWriter::ErrorWriter(){}

ErrorWriter::ErrorWriter(const ErrorWriter &){}

ErrorWriter &ErrorWriter::operator=(const ErrorWriter &){
    return *this;
};

ErrorWriter::~ErrorWriter() {}

void ErrorWriter::write(const char *output){
    ApplicationSystem<ScriptSystem>::instance().current_script().error_output().write(output);
};

void ErrorWriter::flush(){
    ApplicationSystem<ScriptSystem>::instance().current_script().error_output().flush();
};

static const boost::python::str redirect_logger_code{"import sys\nimport GameUtils\nsys.stdout=GameUtils.StandardWriter()\nsys.stderr=GameUtils.ErrorWriter()"};

BOOST_PYTHON_MODULE(GameUtils) {
    using namespace boost::python;
    class_<StandardWriter>("StandardWriter")
        .def("write", &StandardWriter::write);
    class_<ErrorWriter>("ErrorWriter")
        .def("write", &ErrorWriter::write);
}

Runnable::Runnable() : code_(), standard_output_(new IgnoreWriter{}), error_output_(new IgnoreWriter{}) {
}

Runnable::~Runnable() {
    delete error_output_;
    delete standard_output_;
}

void Runnable::error_output(Writer* writer) {
    delete error_output_;
    if(writer){
        error_output_ = writer;
    }else{
        error_output_ = new IgnoreWriter{};
    }
}

Writer &Runnable::error_output(){
    return *error_output_;
};

Writer &Runnable::standard_output(){
    return *standard_output_;
}

void Runnable::standard_output(Writer* writer) {
    delete standard_output_;
    if(writer){
        standard_output_ = writer;
    }else{
        error_output_ = new IgnoreWriter{};
    }
}

void Runnable::load(std::istream& input) {
    streampos begin = input.tellg();
    input.seekg(0, ios::end);
    streamsize size = input.tellg() - begin;
    if (size > 0) {
        char *buffer = new char[size];
        input.seekg(begin);
        input.read(buffer, size);
        code_ = str{buffer, static_cast<size_t>(size)};
        delete[] buffer;
    }
}

void Runnable::run() {
    ApplicationSystem<ScriptSystem>::instance().run(this);
    standard_output_->flush();
    error_output_->flush();
}
    
const ApplicationId ScriptSystem::id{"script"};

ScriptSystem::ScriptSystem() : current_script_(), run_mutex_(){}

ScriptSystem::~ScriptSystem(){};

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
    try {
        exec(redirect_logger_code, main_namespace, main_namespace);
        exec(current_script_->code_, main_namespace, main_namespace);
    } catch (boost::python::error_already_set const &) {
        PyErr_Print();
    }
    Py_Finalize();
    current_script_ = nullptr;
}

Runnable &ScriptSystem::current_script(){
    return *current_script_;
};