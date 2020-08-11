//
// Created by mke on 11/08/2020.
//

#include "vm_writer.h"

void VM_Writer::write_push(const std::string& segment, int index) {
    out_file << "push " + segment  + " " << index;
}

void VM_Writer::write_pop(const std::string &segment, int index) {
    out_file << "pop " + segment + " " << index;
}

void VM_Writer::write_arithmetic(const std::string &command) {
    out_file << command;
}

void VM_Writer::write_label(const std::string &label) {

}

void VM_Writer::write_goto(const std::string &label) {

}

void VM_Writer::write_if(const std::string &label) {

}

void VM_Writer::write_call(const std::string &name, int n_args) {

}

void VM_Writer::write_function(const std::string &name, int n_locals) {

}

void VM_Writer::write_return() {

}

