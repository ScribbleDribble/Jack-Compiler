#include "parser.h"
#include <iostream>
#include <cstdlib>
#include "vm_writer.h"
#include <regex>

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
        std::cout << "Compiling file: " + file_path << std::endl;
        file.open(file_path, std::ifstream::in);
        tokenizer.get_tokens();
        file.clear();
    }

    std::regex regexp("[A-Z]*[a-z]*.jack");
    std::smatch smatch;
    std::string rel_path = argv[1];
    std::regex_search(rel_path, smatch, regexp);

    std::string vm_file_name;
    for (std::string str : smatch) {
        vm_file_name = str.substr(0, str.size() - 4) + "vm";
    }

    std::ofstream vm_file;
    vm_file.open("/home/mke/projects/c++/jack_compiler/compiler/" + vm_file_name);
    VM_Writer vm_writer(vm_file);

    std::ofstream out_file;
    out_file.open("/home/mke/projects/c++/jack_compiler/compiler/syntax_struct.xml");

    Parser parser(out_file, tokenizer, vm_writer);

    file.close();
    out_file.close();

    return 0;
}

