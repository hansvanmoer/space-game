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

StringPoolError::StringPoolError(const string &message) : runtime_error(message){}

StringPool::StringPool() {
}

StringPool::~StringPool() {
}

BufferedStringPool::BufferedStringPool(bool randomized, bool exhaustible) : pool_(), next_(pool_.end()), randomized_(randomized), exhaustible_(exhaustible) {
}

BufferedStringPool::~BufferedStringPool() {
}

std::string BufferedStringPool::next(){
    if(next_ == pool_.end()){
        if(exhaustible_){
            throw StringPoolError{"string pool is empty"};
        }else{
            reset();
        }
    }
    return *next_++;
}

std::string BufferedStringPool::peek(){
    if(next_ == pool_.end()){
        if(exhaustible_){
            throw StringPoolError{"string pool is empty"};
        }else{
            reset();
        }
    }
    return *next_;
}

bool BufferedStringPool::has_more() const{
    return !exhaustible_ || (next_ != pool_.end());
}

void BufferedStringPool::reset() {
    if (!pool_.empty()) {
        if(randomized_){
            unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(pool_.begin(), pool_.end(), default_random_engine(seed));
        }
        next_ = pool_.begin();
    }
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

void BufferedStringPool::load_from_file(const ResourceId& id) {
    ifstream input;
    ApplicationSystem<ResourceSystem>::instance().open_string_pool(id, input);
    load(input);
    reset();
}

BOOST_PYTHON_MODULE(NameGenerator)
{
    class_<BufferedStringPool>("StringPool", init<bool, bool>())
            .def("load_from_file", &BufferedStringPool::load_from_file)
            .def("add", &BufferedStringPool::add)
            .def("next", &BufferedStringPool::next)
            .def("peek", &BufferedStringPool::peek)
            .def("has_more", &BufferedStringPool::has_more)
            .def("reset", &BufferedStringPool::reset);
}

void Script::NameScript::before_initialize(){
    PyImport_AppendInittab( "NameGenerator", &initNameGenerator);
}