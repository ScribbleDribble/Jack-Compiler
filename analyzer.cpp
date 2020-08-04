#include "tokenizer.h"
#include <iostream>


int main() {

    std::ifstream file;
    file.open("/home/mke/projects/c++/jack_compiler/compiler/example.jack", std::ifstream::in);

    Tokenizer tokenizer(file);

    return 0;
}

