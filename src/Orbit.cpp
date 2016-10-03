#include "Orbit.h"

#include <algorithm>
#include <cmath>

using namespace Game;
using namespace std;

OrbitalObject::OrbitalObject(MapObject* object)  : orbit_(), object_(object){
}

MapObject* OrbitalObject::object() const {
    return object_;
}

void OrbitalObject::update(Duration duration){}


Orbit* OrbitalObject::orbit() const {
    return orbit_;
}

GravityWell::GravityWell(MapObject* object) : OrbitalObject(object), radius(), orbits_(){
}

const std::vector<Orbit*> &GravityWell::orbits() const {
    return orbits_;
}

void GravityWell::update(Duration current) {
    for(Orbit *orbit : orbits_){
        orbit->update(current);
    }
}


GravityWell::~GravityWell() {}

Orbit::Orbit() : parent_(), child_(){
}

OrbitalObject* Orbit::child() const {
    return child_;
}

GravityWell* Orbit::parent() const {
    return parent_;
}

void Orbit::update(Duration current) {
    child_->object()->position(calculate_position(current));
    child_->update(current);
}

void Orbit::detach() {
    Game::detach(this);
}

Orbit::~Orbit() {
}

void Game::attach(GravityWell* parent, OrbitalObject* child, Orbit* orbit) {
    orbit->parent_ = parent;
    orbit->child_ = child;
    parent->orbits_.push_back(orbit);
    child->orbit_ = orbit;
}

void Game::detach(Orbit *orbit){
    vector<Orbit *> &orbits = orbit->parent_->orbits_;
    orbits.erase(find(orbits.begin(), orbits.end(), orbit));
    orbit->child_->orbit_ = nullptr;
    orbit->parent_ = nullptr;
    orbit->child_ = nullptr;
}

StaticOrbit::StaticOrbit(const Position& relative_position) : relative_position_(){
}

Position StaticOrbit::calculate_position(Duration current) {
    return parent_->object()->position() + relative_position_;
}

CircularOrbit::CircularOrbit(Coordinate radius, Duration period, Coordinate phase) : radius_(radius), period_(period), phase_(phase){
}

Position CircularOrbit::calculate_position(Duration current) {
    using namespace std::chrono;
    duration<double> current_duration = current % period_;
    double theta = phase_ + (pi() * current / period_);
    Position parent_pos = parent_->object()->position();
    return Position{parent_pos.x+cos(theta)*radius_, parent_pos.y+sin(theta)*radius_};
}

