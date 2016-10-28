#include <iostream>
#include <sstream>

#include "Name.h"

using namespace Game;
using namespace std;

int main(int arg_cunt, const char **args) {
    string s{"# Dit is een comment\n #Dit ook\nNaam1\n Naam\t\nNaam 3\nNaam 4"};
    istringstream buffer{s};
    BufferedStringPool pool;
    pool.load(buffer);
    for (int i = 0; i < 8; ++i) {
        pool.reset();
        while (pool.has_more()) {
            cout << "\"" << pool.next() << "\"" << endl;
        }
    }

    return 0;
}
