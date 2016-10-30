#include <iostream>
#include <sstream>

#include "Name.h"

#include "CLI.h"

#include "FileSystem.h"

using namespace Game;
using namespace Game::CLI;
using namespace std;


Call parse_arguments(int arg_count, const char **args) {
    ArgumentParser parser{
        ArgumentDefinition{"module ID", "module", 'm', "default", false},
        ArgumentDefinition{"language ID", "language", 'l', "en", false},
        ArgumentDefinition{"data path", "data", 'd', "", false},
    };
    return parser.parse(arg_count, args);
};

int main(int arg_count, const char **args) {
    
    cout << "starting application" << endl;
    Call call = parse_arguments(arg_count, args);
    
    cout <<  call << endl;
    ApplicationSystemGuard<FileSystem> file_system_guard(call);
    
    
    return 0;
}
