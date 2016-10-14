#include <iostream>
#include <sstream>

#include "Name.h"

using namespace Game;
using namespace std;

int main(int arg_cunt, const char **args) {
    string s{"# Dit is een comment\n #Dit ook\nNaam1\n Naam\t"};
    istringstream buffer{s};
    BufferedStringPool pool;
    pool.load(buffer);
    pool.reset();
    while(pool.has_more()){
        cout << "\"" << pool.next() << "\"" << endl;
    }
    
    return 0;
}
