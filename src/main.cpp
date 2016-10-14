#include <iostream>

#include "Metrics.h"
#include "String.h"

using namespace Game;
using namespace std;

int main(int arg_cunt, const char **args) {
    
    Transform2<double> t;
    string s{" \tDit is een test\t "};
    trim_whitespace(s);
    cout << "\"" << s << "\"";
    return 0;
}
