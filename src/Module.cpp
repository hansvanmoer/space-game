#include "Module.h"
#include "Log.h"
#include "FileSystem.h"
#include "Parser.h"

#include <fstream>
#include <iterator>

using namespace Game;
using namespace std;

static Log::Logger logger = Log::create_logger("default");

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

istream &Game::operator >>(std::istream& input, Version& version){
    string buffer;
    getline(input, buffer, '.');
    version.major = static_cast<unsigned int>(stoi(buffer));
    getline(input, buffer, '.');
    version.minor = static_cast<unsigned int>(stoi(buffer));
    input >> version.bugfix;
    return input;
}

ostream &Game::operator<<(std::ostream &output, const Version &version){
    return output << version.major << '.' << version.minor << '.' << version.bugfix;
}

ModuleDescriptor::ModuleDescriptor() : id(), version(), default_language_id(), supported_language_ids(){
}

ModuleDescriptor::ModuleDescriptor(const ModuleId& id_, const Property::Map& properties) : id(id_), version(), default_language_id(), supported_language_ids(){
    Parser::required_from_map(properties, "version", version);
    Parser::required_from_map(properties, "default_language_id", default_language_id);
    Parser::required_sequence_from_map(properties, "supported_language_ids", Parser::Sequence<string>{}, back_inserter(supported_language_ids));
    if(find(supported_language_ids.begin(), supported_language_ids.end(), default_language_id) == supported_language_ids.end()){
        supported_language_ids.push_back(default_language_id);
    }
    sort(supported_language_ids.begin(), supported_language_ids.end());
    supported_language_ids.erase(unique(supported_language_ids.begin(), supported_language_ids.end()), supported_language_ids.end());
}


ostream &Game::operator <<(ostream& output, const ModuleDescriptor& descriptor){
    output << "module id: " << descriptor.id << endl 
            << "version: " << descriptor.version << endl
            << "default language ID: " << descriptor.default_language_id << endl 
            << "supported language ID's:";
    for(string id : descriptor.supported_language_ids){
        output << ' ' << id;
    }
    output << endl;
    return output;
}

const ApplicationId ModuleSystem::id{"module"};


ModuleSystem::ModuleSystem(const ModuleId& module_id, const LanguageId &language_id) : descriptor_(){
    namespace fs = boost::filesystem;

    logger.info("loading module ", id);
    path_ = ApplicationSystem<FileSystem>::instance().modules_path() / module_id;
    logger.debug("loading module from path ", path_.string());
    if(!fs::is_directory(path_)){
        throw ApplicationError{id, "module folder not found: " + path_.string()};
    }
    load_module_descriptor(id);
    if(find(descriptor_.supported_language_ids.begin(), descriptor_.supported_language_ids.end(), language_id) == descriptor_.supported_language_ids.end()){
        logger.warning("module does not support language", language_id);
        language_id_ = descriptor_.default_language_id;
        logger.warning("language set to module default", language_id_);
    }else{
        language_id_ = language_id;
    }
}

void ModuleSystem::load_module_descriptor(const ModuleId &id) {
    namespace fs = boost::filesystem;
    
    ifstream input;
    fs::path descriptor_path = path_ / "module.properties";
    if(!fs::is_regular_file(descriptor_path)){
        throw ApplicationError{id, "unable to read module descriptor"};
    }
    input.open(descriptor_path.c_str());
    Property::Map props{Property::read_properties(input)};
    if(input.bad()){
        throw ApplicationError{id, "an error occurred while reading module descriptor"};
    }
    descriptor_ = ModuleDescriptor{id, props};
    logger.debug_lines("module descriptor: ", descriptor_);
}

bool ModuleSystem::normalize_path(boost::filesystem::path& resource_path) const {
    namespace fs = boost::filesystem;
    fs::path result = path_ / resource_path;
    if(fs::is_regular_file(result) || fs::is_directory(result)){
        resource_path = result;
        return true;
    }else{
        return false;
    }
}

boost::filesystem::path ModuleSystem::create_localized_path(const boost::filesystem::path& path, const LanguageId& language_id) const {
    namespace fs = boost::filesystem;
    fs::path parent = path.parent_path();
    return parent / fs::path{language_id} / path.filename();
}


bool ModuleSystem::localize_path(boost::filesystem::path& resource_path) const {
    namespace fs = boost::filesystem;
    fs::path language_path = create_localized_path(resource_path, language_id_);
    if(fs::is_regular_file(language_path) || fs::is_directory(language_path)){
        resource_path = language_path;
        return true;
    }else{
        language_path = create_localized_path(resource_path, descriptor_.default_language_id);
        if(fs::is_regular_file(language_path) || fs::is_directory(language_path)){
            resource_path = language_path;
            return true;
        }else{
            return false;
        }
    }
}
