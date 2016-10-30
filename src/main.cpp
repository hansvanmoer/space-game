#include <iostream>
#include <sstream>

#include "Name.h"

#include "CLI.h"

#include "FileSystem.h"
#include "Log.h"

using namespace Game;
using namespace Game::CLI;
using namespace std;

static Log::Logger logger = Log::create_logger("default");

Call parse_arguments(int arg_count, const char **args) {
    ArgumentParser parser{
        ArgumentDefinition{"module ID", "module", 'm', "default", false},
        ArgumentDefinition{"language ID", "language", 'l', "en", false},
        ArgumentDefinition{"data path", "data", 'd', "", false},
    };
    return parser.parse(arg_count, args);
};

int main(int arg_count, const char **args) {
        
    Log::configure_logger("default", Log::Level::DEBUG, &std::cout);
    logger.info("starting application");
    
    Call call = parse_arguments(arg_count, args);
    logger.debug("initializing with the following parameters:").info_lines(call);
    ApplicationSystemGuard<FileSystem> file_system_guard(call);
    
    
    return 0;
}
