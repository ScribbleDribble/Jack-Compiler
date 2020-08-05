#include "parser.h"
#include <iostream>


int main() {

    std::ifstream file;
    file.open("/home/mke/projects/c++/jack_compiler/compiler/examples/example.jack", std::ifstream::in);
    Tokenizer tokenizer(file);

    std::ofstream out_file;

    out_file.open("/home/mke/projects/c++/jack_compiler/compiler/syntax_struct.xml");
    Parser parser(out_file, tokenizer);

    out_file.close();

    return 0;
}

