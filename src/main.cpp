#include <iostream>
#include <sstream>
#include <unordered_map>

#include "Name.h"
#include "CLI.h"
#include "FileSystem.h"
#include "Log.h"
#include "Module.h"
#include "Parser.h"
#include "Resource.h"
#include "Script.h"

using namespace Game;
using namespace Game::CLI;
using namespace std;

static Log::Logger logger = Log::create_logger("default");

static const unordered_map<string, Log::Level> log_level_parameters{
    {"info", Log::Level::info},
    {"debug", Log::Level::debug},
    {"warning", Log::Level::warning}, 
    {"error", Log::Level::error}, 
    {"fatal", Log::Level::fatal}, 
    {"none", Log::Level::none}
};

Call parse_arguments(int arg_count, const char **args) {
    ArgumentParser parser{
        ArgumentDefinition{"module ID", "module", 'm', "default", false},
        ArgumentDefinition{"language ID", "language", 'l', "en", false},
        ArgumentDefinition{"data path", "data", 'd', "", false},
        ArgumentDefinition{"logger verbosity", "verbosity", 'v', "info", false}
    };
    return parser.parse(arg_count, args);
};

Log::Level get_log_level(const Arguments &args){
    auto found = args.find("logger verbosity");
    if(found == args.end()){
        return Log::Level::none;
    }else{
        auto found_level =  log_level_parameters.find(found->second);
        if(found_level == log_level_parameters.end()){
            return Log::Level::none;
        }else{
            return found_level->second;
        }
    }
};

int main(int arg_count, const char **args) {
    cout << "starting application" << endl;
    
    Call call = parse_arguments(arg_count, args);
    
    Log::Level level = get_log_level(call.arguments);
    
    Log::configure_logger("default", level, &std::cout);
    
    logger.debug("initializing with the following parameters:").debug_lines(call);    
    
    ApplicationSystemGuard<FileSystem> file_system_guard(call);
    ApplicationSystemGuard<ModuleSystem> module_system_guard(call.arguments["module ID"], call.arguments["language ID"]);
    ApplicationSystemGuard<ScriptSystem> script_system_guard;
    ApplicationSystemGuard<ResourceSystem> resource_system;
    return 0;
}
