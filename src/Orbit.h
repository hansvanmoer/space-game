/* 
 * File:   StarSystem.h
 * Author: hans
 *
 * Created on September 30, 2016, 1:33 PM
 */

#ifndef GAME_ORBIT_H
#define	GAME_ORBIT_H

#include "Object.h"

#include <vector>

namespace Game {

    struct Orbit;

    class GravityWell;

    class OrbitalObject;

    ///
    /// Attaches the child to the parent's gravity well using the specified orbit
    /// The orbit will be added to the gravity well's orbit list and the child's orbit will be set
    /// \param parent the gravity well
    /// \param child the satellite
    /// \param orbit the orbit
    ///
    void attach(GravityWell *parent, OrbitalObject *satellite, Orbit *orbit);

    ///
    /// Detaches the gravity well and satellite
    /// Removes the orbit from the gravity well's orbit list and unsets the child orbit
    /// The orbit is not deleted
    /// \param orbit the orbit
    ///
    void detach(Orbit *orbit);

    ///
    /// A type representing an object in orbit around another object
    ///
    class OrbitalObject {
    public:
        
        ///
        /// creates a new orbital object
        /// \param object the object visible on the map
        ///
        OrbitalObject(MapObject *object);

        ///
        /// \return the object visible on the map
        ///
        MapObject *object() const;
        
        ///
        /// updates the orbital object and its child orbits
        /// \param current the elapsed time since game start
        ///
        virtual void update(Duration current);
        
        ///
        /// \return the current orbit of this orbital object
        ///
        Orbit *orbit() const;
    private:
        Orbit *orbit_;
        MapObject * const object_;

        friend void attach(GravityWell *parent, OrbitalObject *child, Orbit *orbit);
        friend void detach(Orbit *orbit);
    };

    ///
    /// represents an orbital object with a gravity well and satellites
    ///
    class GravityWell : public OrbitalObject {
    public:
        
        ///
        /// creates a new gravity well
        ///
        GravityWell(MapObject *object);

        ///
        /// updates the orbital object and its child orbits
        /// \param current the elapsed time since game start
        ///
        void update(Duration duration);

        ///
        /// \return the list of satellite orbits
        ///
        const std::vector<Orbit *> &orbits() const;

        ///
        /// destroys this gravity well
        /// warning does not destroy orbits!
        ///
        virtual ~GravityWell();

        ///
        /// the radius of this gravity well
        ///
        Coordinate radius;

    private:
        std::vector<Orbit *> orbits_;

        friend void attach(GravityWell *parent, OrbitalObject *child, Orbit *orbit);
        friend void detach(Orbit *orbit);
    };

    ///
    /// represents an orbit
    ///
    struct Orbit {
        
        ///
        /// \return the parent of this orbit
        ///
        GravityWell *parent() const;

        ///
        /// \return the child (satellite) of this orbit
        ///
        OrbitalObject *child() const;

        ///
        /// updates the position of the satellite
        /// \param current the elapsed time since game start
        ///
        void update(Duration current);

        ///
        /// detaches this orbit (see Game::detach())
        ///
        void detach();
        
        ///
        /// destroys this orbit
        /// does not detach the orbit before destruction
        ///
        virtual ~Orbit();
    protected:
        
        ///
        /// creates a new orbit
        ///
        Orbit();
        
        ///
        /// the orbit parent
        ///
        GravityWell *parent_;
        
        ///
        /// the child or satellite
        ///
        OrbitalObject *child_;
        
        ///
        /// should be implemented to calculate the child's position for a given elapsed time
        /// \return the child position
        ///
        virtual Position calculate_position(Duration current) = 0;
        
    private:
        friend void attach(GravityWell *parent, OrbitalObject *child, Orbit *orbit);
        friend void detach(Orbit *orbit);
    };
    
    ///
    /// represents a static orbit where the child's position does not change over time relative to the parent's position
    ///
    class StaticOrbit : public Orbit {
    public:
        
        ///
        /// creates a new static orbit with the specified relative position
        /// \param relative_position the position of the child relative to the parent's
        ///
        StaticOrbit(const Position &relative_position);

    protected:
        
        Position calculate_position(Duration current);

    private:
        const Position relative_position_;
    };

    ///
    /// represents a circular orbit (I know, heresy, should be elliptic)
    ///
    class CircularOrbit : public Orbit {
    public:
        ///
        /// creates a new circular orbit
        /// \param the orbit's radius
        /// \param period the period
        /// \param phase the orbit's phase (angle at start)
        ///
        CircularOrbit(Coordinate radius, Duration period, Coordinate phase);

    protected:

        Position calculate_position(Duration duration);

    private:
        Coordinate radius_;
        Duration period_;
        Coordinate phase_;
    };

}

#endif	/* STARSYSTEM_H */

