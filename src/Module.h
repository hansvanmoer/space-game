///
/// \file contains all types and functions related to module loading and management
///

#ifndef MODULE_H
#define	MODULE_H

#include "Application.h"

#include <string>
#include <set>

namespace Game{
    
    struct Version{
        unsigned int major;
        unsigned int minor;
        unsigned int bugfix;
        
        Version();
        
        Version(unsigned int major, unsigned int minor, unsigned int bugfix);
        
        bool operator==(const Version &version) const;
        
        bool operator!=(const Version &version) const;
        
        bool operator<(const Version &version) const;
        
        bool operator<=(const Version &version) const;
        
        bool operator>(const Version &version) const;
        
        bool operator>=(const Version &version) const;
    };
    
    using ModuleId = std::string;
    
    using LanguageId = std::string;
    
    struct ModuleDescriptor{
        ModuleId id;
        Version version;
        LanguageId default_language_id;
        std::set<LanguageId> supported_language_ids;
        
        ModuleDescriptor();
    };
    
    class ModuleSystem{
    public:
        
        ModuleSystem(const ModuleId &module_id);
        
        ~ModuleSystem();
        
        void shutdown();
        
    };
    
}

#endif	/* MODULE_H */

