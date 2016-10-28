#include <iostream>
#include <sstream>

#include "Name.h"

#include "CLI.h"

using namespace Game;
using namespace Game::CLI;
using namespace std;


Call parse_arguments(int arg_count, const char **args) {
    ArgumentParser parser{
        ArgumentDefinition{"module", "module", 'm', "default", false},
        ArgumentDefinition{"language", "language", 'l', "en", false}
    };
    return parser.parse(arg_count, args);
};

int main(int arg_count, const char **args) {
    
    cout << "starting application" << endl;
    Call call = parse_arguments(arg_count, args);
    
    cout <<  call << endl;
    
    return 0;
}
