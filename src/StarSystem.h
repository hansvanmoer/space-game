/* 
 * File:   StarSystem.h
 * Author: hans
 *
 * Created on October 3, 2016, 9:11 PM
 */

#ifndef GAME_STAR_SYSTEM_H
#define	GAME_STAR_SYSTEM_H

namespace Game{
    
    class StarSystem : public Object, MapObject{
        
    private:
        std::string name_;
        GravityWell *gravity_well_;
    };
    
    
}

#endif	/* STARSYSTEM_H */

