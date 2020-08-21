//
// Created by mke on 11/08/2020.
//

#ifndef JACK_COMPILER_VM_WRITER_H
#define JACK_COMPILER_VM_WRITER_H
#include <iostream>
#include <fstream>
#include <map>

class VM_Writer {
private:
    std::ofstream& out_file;
    std::map<std::string, std::string> op_map = { {"+", "add"}, {"-", "sub"},
                                                  {"*", "call Math.multiply 2"}, {"/", "call Math.divide 2"},
                                                  {">", "gt"}, {"<", "lt"}, {"&", "and"},
                                                  {"=", "eq"}, {"|", "or"}, {"~", "not"}};
    std::string translate_operator(const std::string& command);

public:

    VM_Writer(std::ofstream& out_file): out_file(out_file) {

    }

    void write_push(const std::string& segment, int index);
    void write_pop(const std::string& segment, int index);
    void write_arithmetic(const std::string& command);
    void write_label(int label_number);
    void write_goto(int label_number);
    void write_if(int label_number);
    void write_call(const std::string& name, int n_args);
    void write_function(const std::string& name, int n_locals);
    void write_return();

    void operator <<(const std::string& s)  {
        out_file << s << std::endl;
    }
};


#endif //JACK_COMPILER_VM_WRITER_H
