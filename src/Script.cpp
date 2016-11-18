#include "Script.h"
#include "Log.h"

#include <utility>

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

void BufferedScript::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str code_str{code_};
    exec(code_str, globals, locals);
}

ScriptFile::ScriptFile(const std::string& name, const boost::filesystem::path& path) : Script(name), path_(path){
}

ScriptFile::ScriptFile(const boost::filesystem::path& path) : ScriptFile(path.filename().string(), path){
}

void ScriptFile::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str path_str{path_.string()};
    exec_file(path_str, globals, locals);
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

BOOST_PYTHON_MODULE(Game){
    using namespace boost::python;
    class_<ScriptWriterHandle>("ScriptWriter")
            .def("write", &ScriptWriterHandle::write)
            .def("flush", &ScriptWriterHandle::flush);
}


const ApplicationId ScriptSystem::id{"script"};

ScriptSystem::ScriptSystem() : writer_(){
    Py_Initialize();
    initGame();
    
    run(BufferedScript{"init_script_system",
            "import Game\n"
            "import sys\n"
            "sys.stdout=Game.ScriptWriter()\n"
    });
}

ScriptSystem::~ScriptSystem(){
    Py_Finalize();
}

ScriptWriter &ScriptSystem::writer(){
    return writer_;
}

void ScriptSystem::run(const Script& script) {
    using namespace boost::python;
    try{
        object main_module = import("__main__");
        object main_namespace = main_module.attr("__dict__");
        object locals;
        script.execute(main_namespace, locals);
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