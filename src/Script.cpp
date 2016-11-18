#include "Script.h"
#include "Log.h"

#include "Name.h"

#include <utility>

using namespace Game;
using namespace std;

static Log::Logger logger = Log::LoggerFactory::instance().create_logger("default");

ScriptError::ScriptError(const string& message) : runtime_error(message){};

Script::Script(const string& name, const string &module_id) : name_(name), module_id_(module_id){
}

const string& Script::name() const {
    return name_;
}

const string &Script::module_id() const{
    return module_id_;
}

BufferedScript::BufferedScript(const string& name, const string &module_id, const string& code) : Script(name, module_id), code_(code){
}

BufferedScript::BufferedScript(const string& name, const string &module_id, string &&code) : Script(name, module_id), code_(move(code)){}

boost::python::object BufferedScript::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str code_str{code_};
    return exec(code_str, globals, locals);
}

ScriptFile::ScriptFile(const string& name, const string &module_id, const boost::filesystem::path& path) : Script(name, module_id), path_(path){
}

ScriptFile::ScriptFile(const string &module_id, const boost::filesystem::path& path) : ScriptFile(path.filename().string(), module_id, path){
}

boost::python::object ScriptFile::execute(boost::python::object globals, boost::python::object locals) const {
    using namespace boost::python;
    str path_str{path_.string()};
    return exec_file(path_str, globals, locals);
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

BOOST_PYTHON_MODULE(NameGeneratorExt){}


const ApplicationId ScriptSystem::id{"script"};

ScriptSystem::ScriptSystem() : writer_(), mutex_(), extensions_(){
    Py_Initialize();
    initGameUtils();
    initGameUtilsExt();
    initNameGenerator();
    initNameGeneratorExt();
    run(BufferedScript{"init_script_system", "GameUtilsExt",
            "import GameUtils\n"
            "import sys\n"
            "sys.stdout=GameUtils.ScriptWriter()\n"
    });
}

ScriptSystem::~ScriptSystem(){
    Py_Finalize();
}

ScriptWriter &ScriptSystem::writer(){
    return writer_;
}

boost::python::object ScriptSystem::extension(const string &module_id){
    return extensions_[module_id];
}

void ScriptSystem::run(const Script& script) {
    lock_guard<mutex> lock{mutex_};
    using namespace boost::python;
    try{
        object main_module = import("__main__");
        object main_namespace = main_module.attr("__dict__");
        object extension_module = import(script.module_id().c_str());
        main_namespace["extensions"] = extension_module;
        object result = script.execute(main_namespace, main_namespace);
        extensions_[script.module_id()] = extension_module;
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