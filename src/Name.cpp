#include "Name.h"

#include <random>
#include <algorithm>
#include <chrono>

#include "String.h"

using namespace std;
using namespace Game;

StringPool::StringPool() {
}

StringPool::~StringPool() {
}

BufferedStringPool::BufferedStringPool() : pool_(), next_(pool_.end()) {
}

BufferedStringPool::~BufferedStringPool() {
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

