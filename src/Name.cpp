#include "Name.h"

#include "String.h"
#include "Resource.h"
#include "Log.h"

#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

#include <boost/python.hpp>

using namespace std;
using namespace Game;
using namespace Game::Script;

using namespace boost::python;

static Log::Logger logger = Log::create_logger("default");

StringPool::StringPool() {
}

StringPool::~StringPool() {
}

BufferedStringPool::BufferedStringPool() : pool_(), next_(pool_.end()) {
}

BufferedStringPool::~BufferedStringPool() {
}

std::string BufferedStringPool::next(){
    return *next_++;
}

bool BufferedStringPool::has_more() {
    return next_ != pool_.end();
}

void BufferedStringPool::reset() {
    if (!pool_.empty()) {
        unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(pool_.begin(), pool_.end(), default_random_engine(seed));
        next_ = pool_.begin();
    }
}

void BufferedStringPool::add(const char* value, std::size_t length) {
    pool_.emplace_back(value, value+length);
}

void BufferedStringPool::add(const std::string& value){
    pool_.push_back(value);
}

std::size_t BufferedStringPool::load(std::istream& input) {
    size_t result;
    string line;
    while(input.good()){
        getline(input, line);
        trim_whitespace(line);
        if(!line.empty()){
            if(line.front() != '#'){
                add(line);
                ++result;
            }
        }
    }
    return result;
}

Script::StringPoolHandle::StringPoolHandle() : pool_(){};

void StringPoolHandle::add(const std::string& value) {
    pool_.add(value);
}

void StringPoolHandle::load_from_file(const ResourceId& id) {
    ifstream input;
    ApplicationSystem<ResourceSystem>::instance().open_string_pool(id, input);
    pool_.load(input);
}

void python_log(const char *data){
    logger.debug(data);
};

BOOST_PYTHON_MODULE(NameGenerator)
{
    class_<StringPoolHandle>("StringPool")
            .def("load_from_file", &StringPoolHandle::load_from_file)
            .def("add", &StringPoolHandle::add);
}

void Script::NameScript::before_initialize(){
    PyImport_AppendInittab( "NameGenerator", &initNameGenerator);
}