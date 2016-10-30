#include "Module.h"

using namespace Game;
using namespace std;

Version::Version() : Version(1,0,0){}

Version::Version(unsigned int major_, unsigned int minor_, unsigned int bugfix_){
    major = major_;
    minor = minor_;
    bugfix = bugfix_;
}

bool Version::operator==(const Version& version) const {
    return major == version.major && minor == version.minor && bugfix == version.bugfix;
}

bool Version::operator!=(const Version& version) const {
    return !operator==(version);
}

bool Version::operator<(const Version& version) const {
    if (major < version.major) {
        return true;
    } else if (major == version.major) {
        if (minor < version.minor) {
            return true;
        } else if (minor == version.minor && bugfix < version.bugfix) {
            return true;
        }
    }
    return false;
}

bool Version::operator<=(const Version& version) const {
    if (major > version.major) {
        return false;
    } else if (major == version.major) {
        if (minor > version.minor) {
            return false;
        } else if (minor == version.minor && bugfix > version.bugfix) {
            return false;
        }
    }
    return true;
}

bool Version::operator>(const Version& version) const {
    return version.operator <(*this);
}

bool Version::operator>=(const Version& version) const {
    return version.operator <=(*this);
}

ModuleDescriptor::ModuleDescriptor() : id(), version(), default_language_id(), supported_language_ids(){
}

ModuleSystem::ModuleSystem(const ModuleId& module_id) {
    
}
