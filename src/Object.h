///
/// \file Defines basic types to manage game object life cycle as well as time and space units
///

#ifndef GAME_OBJECT_H
#define	GAME_OBJECT_H

#include "Metrics.h"

#include <string>
#include <chrono>

namespace Game {

    ///
    /// a scalar type to use for distance measurements
    ///
    using Coordinate = double;

    ///
    /// a two dimensional vector to model the position on a plane
    ///
    using Position = Vector2<Coordinate>;

    ///
    /// a type for unique identifiers of game objects
    ///
    using ObjectId = std::string;

    ///
    /// a clock type (c++11)
    ///
    using Clock = std::chrono::high_resolution_clock;

    ///
    /// a time point type (based on Clock)
    ///
    using TimePoint = Clock::time_point;

    ///
    /// a duration type (based on Clock)
    ///
    using Duration = Clock::duration;

    ///
    /// a context object used to handle all non-local object life cycle responsibilities 
    ///

    class ObjectContext {
    };

    ///
    /// a context object used to handle all non-local persistence responsibilities 
    ///

    class PersistenceUnit {
    };

    ///
    /// a base type for all complex game objects
    ///

    class Object {
    public:

        ///
        /// destroys this object
        /// resource deallocation may happen in dispose())
        ///
        virtual ~Object();

        ///
        /// initializes this object
        /// allocation of all managed or shared resources should happen here
        /// implementations should override do_initialize to modify standard behavior
        /// \param context the context for this object
        ///
        void initialize(ObjectContext &context);

        ///
        /// saves this object into the persistence unit
        /// implementations should override do_save to modify standard behavior
        /// \param context the context for this object
        /// \param unit the persistence unit to save the object to
        ///
        void save(ObjectContext &context, PersistenceUnit &unit);

        ///
        /// loads this object from the persistence unit
        /// implementations should override do_load to modify standard behavior
        /// \param context the context for this object
        /// \param unit the persistence unit to load the object from
        ///
        void load(ObjectContext &context, PersistenceUnit &unit);
        
        ///
        /// disposes this object
        /// all relations of this object should be destroyed after this method returns
        /// implementations should override do_dispose to modify standard behavior
        /// \param context the context for this object
        ///
        void dispose(ObjectContext &context);

        ///
        /// \return the object id
        ///
        const ObjectId &id() const;

        ///
        /// \param o the other object
        /// \return true if the objects have an identical id, false otherwise
        ///
        bool operator==(const Object &o) const;
        
        ///
        /// \param o the other object
        /// \return false if the objects have an identical id, true otherwise
        ///
        bool operator!=(const Object &o) const;
        
        
    protected:

        ///
        /// constructs a new object
        ///
        Object();

        ///
        /// sets the object id
        /// \param id the object id, should be unique
        ///
        void id(const ObjectId &id);
        
        void do_initialize(ObjectContext &context);

        void do_save(ObjectContext &context, PersistenceUnit &unit);

        void do_load(ObjectContext &context, PersistenceUnit &unit);

        void do_dispose(ObjectContext &context);

    private:
        ObjectId id_;
    };

    ///
    /// an object represented on the map
    ///
    class MapObject {
    public:

        virtual ~MapObject();

        ///
        /// \return the object's position on the map
        ///
        const Position &position() const;

        ///
        /// \param position the object's new position on the map
        ///
        void position(const Position &position);

    protected:
        
        ///
        /// creates a new map object
        ///
        MapObject();

    private:
        Position position_;
    };

}

#endif	/* OBJECT_H */

