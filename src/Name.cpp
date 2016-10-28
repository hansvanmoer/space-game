#include "Name.h"

#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

#include <boost/python.hpp>

#include "String.h"

using namespace std;
using namespace Game;
using namespace Game::Script;

using namespace boost::python;

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

StringPoolHandle::StringPoolHandle(){}

StringPoolHandle::StringPoolHandle(BufferedStringPool* pool) : BasicHandle<BufferedStringPool>(pool){
}

std::string StringPoolHandle::next() {
    BufferedStringPool *pool = object();
    if(!pool->has_more()){
        pool->reset();
    }
    return pool->next();
}

void StringPoolHandle::add(const std::string& value) {
    object()->add(value);
}

StringPoolHandle StringPoolManager::create_empty(const Id& id) {
    BufferedStringPool *pool = new BufferedStringPool();
    try{
        return add(id, pool);
    }catch(...){
        throw;
    }
}

StringPoolHandle StringPoolManager::create_from_file(const Id& id, const std::string path) {
    std::ifstream input{path.c_str()};
    BufferedStringPool *pool = new BufferedStringPool();
    try{
        pool->load(input);
        return add(id, pool);
    }catch(...){
        throw;
    }
}

BOOST_PYTHON_MODULE(namegenerator)
{
    class_<StringPoolManager>("StringPoolManager")
            .def("create_empty", &StringPoolManager::create_empty)
            .def("create_from_file", &StringPoolManager::create_from_file);
}