//
// Created by mke on 11/08/2020.
//

#include "vm_writer.h"

void VM_Writer::write_push(const std::string& segment, int index) {
    out_file << "push " + segment  + " " << index << std::endl;
}

void VM_Writer::write_pop(const std::string &segment, int index) {
    out_file << "pop " + segment + " " << index << std::endl;
}

void VM_Writer::write_arithmetic(const std::string &command) {
    out_file << translate_operator(command) <<
    std::endl;
}

void VM_Writer::write_label(int label_number) {
    out_file <<"label L" + std::to_string(label_number) << std::endl;
}

void VM_Writer::write_goto(int label_number) {
    out_file << "goto L" + std::to_string(label_number) << std::endl;
}

void VM_Writer::write_if(int label_number) {
    out_file << "if-goto L" + std::to_string(label_number) << std::endl;
}

void VM_Writer::write_call(const std::string &name, int n_args) {
    out_file << "call " + name +  " " + std::to_string(n_args)  << std::endl;
}

void VM_Writer::write_function(const std::string &name, int n_locals) {
    out_file << "function " + name + " " + std::to_string(n_locals) << std::endl;
}

void VM_Writer::write_return() {
    out_file << "return" << std::endl;
}

std::string VM_Writer::translate_operator(const std::string &command) {
    if (op_map.count(command) > 0) {
        return op_map[command];
    }
    return command;
}


