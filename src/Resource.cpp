#include "Resource.h"
#include "Script.h"
#include "Log.h"
#include "Name.h"

#include <memory>

#include <boost/filesystem.hpp>

using namespace Game;
using namespace std;

static Log::Logger logger = Log::create_logger("default");

ResourceError::ResourceError(const ResourceId& id, const std::string& message) : runtime_error{message}, id_(id){
}

ResourceId ResourceError::id() const {
    return id_;
}


ResourceNotFoundError::ResourceNotFoundError(const ResourceId& id) : ResourceError{id, string{"unable to find resource: "}+id}{
}

const ApplicationId ResourceSystem::id{"resource"};

ResourceSystem::ResourceSystem() {}

void ResourceSystem::load_resources(){
    const ModuleSystem &module{ApplicationSystem<ModuleSystem>::instance()};
    load_names(module);
}

void ResourceSystem::shutdown() {

}

void ResourceSystem::load_names(const ModuleSystem &module) {
    namespace fs = boost::filesystem;
    
    fs::path generators_path{"names/generators.py"};
    if(!module.normalize_path(generators_path)){
        throw ResourceNotFoundError{"name_generator_system"};
    }
    ApplicationSystem<ScriptSystem>::instance().run(ScriptContext{"NameGeneratorExt", {"NameGenerator"}}, ScriptFile{generators_path});
}

void ResourceSystem::open_string_pool(const ResourceId& id, std::ifstream& input) {
    namespace fs = boost::filesystem;
    fs::path path{string{"names/strings/"} + id + ".pool"};
    if(ApplicationSystem<ModuleSystem>::instance().localize_path(path)){
        input.open(path.c_str());
    }else{
        throw ResourceNotFoundError{id};
    }
}
