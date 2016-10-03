///
/// \file Defines basic types to manage game object life cycle as well as time and space units
///

#ifndef GAME_OBJECT_H
#define	GAME_OBJECT_H

#include "Metrics.h"

#include <string>
#include <chrono>

namespace Game{
    
    ///
    /// A scalar type to use for distance measurements
    ///
    using Coordinate = double;
    
    ///
    /// A two dimensional vector to model the position on a plane
    ///
    using Position = Vector2<Coordinate>;
    
    ///
    /// A type for unique identifiers of game objects
    ///
    using ObjectId = std::string;
    
    ///
    /// A clock type (c++11)
    ///
    using Clock = std::chrono::high_resolution_clock;
    
    ///
    /// A time point type (based on Clock)
    ///
    using TimePoint = Clock::time_point;
    
    ///
    /// A duration type (based on Clock)
    ///
    using Duration = Clock::duration;
    
    ///
    /// A context object used to handle all non-local object life cycle responsibilities 
    ///
    class ObjectContext{};
    
    ///
    /// A context object used to handle all non-local persistence responsibilities 
    ///
    class PersistenceUnit{};
    
    ///
    /// A base type for all complex game objects
    ///
    class Object{
    public:
    
        ///
        /// Destroys this object
        /// Resource deallocation may happen in dispose())
        ///
        virtual ~Object();

        ///
        /// Initializes this object
        /// Allocation of all managed or shared resources should happen here
        /// implementations should override do_initialize to modify standard behavior
        /// \param context the context for this object
        ///
        void initialize(ObjectContext &context);
        
        ///
        /// Saves this object into the persistence unit
        ///
        ///
        void save(ObjectContext &context, PersistenceUnit &unit);
        
        void load(ObjectContext &context, PersistenceUnit &unit);
        
        void dispose(ObjectContext &context);
        
        const ObjectId &id() const;
        
    protected:

        Object();
        
        void id(const ObjectId &id);
        
        void do_initialize(ObjectContext &context);
        
        void do_save(ObjectContext &context, PersistenceUnit &unit);
        
        void do_load(ObjectContext &context, PersistenceUnit &unit);
        
        void do_dispose(ObjectContext &context);
        
    private:
        ObjectId id_;
    };
    
    class MapObject{
    public:
        
        virtual ~MapObject();
       
        const Position &position() const;
        
        void position(const Position &position);
    
    protected:
        MapObject();
        
    private:
        Position position_;
    };
    
}

#endif	/* OBJECT_H */

