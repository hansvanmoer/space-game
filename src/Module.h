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

    ///
    /// \class a singleton containing the current module and several utility functions for module related resources
    ///
    class ModuleSystem{
    public:
        
        ///
        /// the module subsystem id
        ///
        static const ApplicationId id;
        
        ///
        /// Creates a new module system and loads the descriptor
        /// \param module_id the id of the module to load
        /// \param language_id the user's preferred language (may be overridden when it is not supported by the module)
        ///
        ModuleSystem(const ModuleId &module_id, const LanguageId &language_id);
        
        ///
        /// Localizes the supplied path
        /// If the localized path does not point to a valid file or folder this method is a no-op
        /// e.g. names/ship.names -> #MODULES_PATH#/#MODULE_ID#/default/names/#LANGUAGE_ID#/ship.names
        /// \param resource_path the path to localize
        /// \return true if the path was localized, false otherwise
        ///
        bool localize_path(boost::filesystem::path &resource_path) const;
        
        ///
        /// Normalizes the supplied path
        /// If the normalized path does not point to a valid file or folder this method is a no-op
        /// e.g. names/ship.names -> #MODULES_PATH#/#MODULE_ID#/default/names/ship.names
        /// \param resource_path the path to normalize
        /// \return true if the path was normalized, false otherwise
        ///
        bool normalize_path(boost::filesystem::path &resource_path) const;
        
    private:
        LanguageId language_id_;
        boost::filesystem::path path_;
        ModuleDescriptor descriptor_;
        
        void load_module_descriptor(const ModuleId &id);
        
        boost::filesystem::path create_localized_path(const boost::filesystem::path &path, const LanguageId &language_id) const;
    };
    
}

#endif	/* MODULE_H */

