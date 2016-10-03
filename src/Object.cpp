#include "Object.h"

using namespace Game;
using namespace std;

Object::Object() : id_(){}

Object::~Object(){}

const ObjectId &Object::id() const{
    return id_;
}

void Object::id(const ObjectId& id){
    id_ = id;
}

void Object::initialize(ObjectContext& context){
    do_initialize(context);
}

void Object::load(ObjectContext& context, PersistenceUnit& unit) {
    do_load(context, unit);
}

void Object::save(ObjectContext& context, PersistenceUnit& unit){
    do_save(context, unit);
}

void Object::dispose(ObjectContext& context) {
    do_dispose(context);
}

void Object::do_initialize(ObjectContext& context){
    
}

void Object::do_load(ObjectContext& context, PersistenceUnit& unit) {

}

void Object::do_save(ObjectContext& context, PersistenceUnit& unit) {

}

void Object::do_dispose(ObjectContext& context) {

}

bool Object::operator==(const Object& o) const {
    return id_ == o.id_;
}


bool Object::operator!=(const Object& o) const {
    return id_ != o.id_;
}


MapObject::MapObject() : position_(){
}

MapObject::~MapObject() {

}

const Position& MapObject::position() const {
    return position_;
}

void MapObject::position(const Position& position) {
    position_ = position;
}


