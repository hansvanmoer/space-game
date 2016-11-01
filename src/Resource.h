///
/// \file contains basic types and functions for resource loading
///

#ifndef GAME_RESOURCE_H
#define	GAME_RESOURCE_H

#include <string>
#include <stdexcept>
#include <fstream>

#include "Application.h"
#include "Module.h"

namespace Game{
    
    ///
    /// \typedef a resource ID type
    ///
    using ResourceId = std::string;
    
    ///
    /// \class a basic error thrown when a resource could not be loaded
    ///
    class ResourceError : public std::runtime_error{
    public:
        
        ///
        /// Creates a new resource error
        /// \param id the resource id
        /// \param message the message for this error
        ///
        ResourceError(const ResourceId &id, const std::string &message);
    
        ResourceId id() const;
        
    private:
        ResourceId id_;
    };
    
    ///
    /// \class an error thrown when a resource could not be found
    ///
    class ResourceNotFoundError : public ResourceError{
    public:
        ResourceNotFoundError(const ResourceId &id);
    };
    
    class ResourceSystem{
    public:
        
        static const ApplicationId id;
        
        ResourceSystem();
        
        void shutdown();
        
        void open_string_pool(const ResourceId &id, std::ifstream &input);
        
    private:
        
        void load_names(const ModuleSystem &module);
                
    };
    
    
}

#endif	/* RESOURCE_H */

