//
// Created by mke on 11/08/2020.
//

#ifndef JACK_COMPILER_VM_WRITER_H
#define JACK_COMPILER_VM_WRITER_H
#include <iostream>
#include <fstream>


class VM_Writer {
private:
    std::ofstream& out_file;
public:

    VM_Writer(std::ofstream& out_file): out_file(out_file) {

    }

    void write_push(const std::string& segment, int index);
    void write_pop(const std::string& segment, int index);
    void write_arithmetic(const std::string& command);
    void write_label(const std::string& label);
    void write_goto(const std::string& label);
    void write_if(const std::string& label);
    void write_call(const std::string& name, int n_args);
    void write_function(const std::string& name, int n_locals);
    void write_return();
};


#endif //JACK_COMPILER_VM_WRITER_H
