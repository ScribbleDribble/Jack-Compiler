#include "parser.h"
#include <iostream>
#include <cstdlib>
#include "vm_writer.h"

int main(int argc, char** argv) {

    std::ifstream file;

    if (argc < 2) {
        std::cerr << "Usage: jcomp <your_file.jack> (<your_file.jack>)*" << std::endl;
        exit(0);
    }

    std::string exe_rel_path = argv[0];
    Tokenizer tokenizer(file);

    for (int i = 1; i < argc; i++) {
        std::string file_path =  exe_rel_path.substr(0, exe_rel_path.length() - 5) + argv[i];
        file.open(file_path, std::ifstream::in);
        tokenizer.get_tokens();
        file.clear();
    }


    std::ofstream vm_file;
    vm_file.open("/home/mke/projects/c++/jack_compiler/compiler/intermediate.vm");
    VM_Writer vm_writer(vm_file);

    std::ofstream out_file;
    out_file.open("/home/mke/projects/c++/jack_compiler/compiler/syntax_struct.xml");

    Parser parser(out_file, tokenizer, vm_writer);

    file.close();
    out_file.close();

    return 0;
}

