///
/// \file contains all types and functions related to module loading and management
///

#ifndef MODULE_H
#define	MODULE_H



#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>


#include "Application.h"
#include "property/Property.h"

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
    
    std::istream &operator>>(std::istream &input, Version &version);
    
    std::ostream &operator<<(std::ostream &output, const Version &version);
    
    using ModuleId = std::string;
    
    using LanguageId = std::string;
    
    struct ModuleDescriptor{
        ModuleId id;
        Version version;
        LanguageId default_language_id;
        std::vector<LanguageId> supported_language_ids;

        ///
        /// Creates an empty module descriptor
        ///
        ModuleDescriptor();
        
        ///
        /// Creates a module descriptor based on the supplied properties
        ///
        /// The following properties are used:
        /// version : the module version
        /// default_language_id : the default language ID
        /// supported_language_ids : a comma separated list of language ID's
        ///
        /// If the default language ID is not a supported language ID, it will be made one
        /// If the supported language ID sequence contains duplicates, they will be ignored
        ///
        /// \param id the module id
        /// \param properties the property map
        ///
        ModuleDescriptor(const ModuleId &id, const Property::Map &properties);
        
    };
    
    std::ostream &operator<<(std::ostream &output, const ModuleDescriptor &descriptor);
    
    class ModuleSystem{
    public:
        
        static const ApplicationId id;
        
        ModuleSystem(const ModuleId &module_id);
        
    private:
        boost::filesystem::path path_;
        ModuleDescriptor descriptor_;
        
        void load_module_descriptor(const ModuleId &id);
    };
    
}

#endif	/* MODULE_H */

