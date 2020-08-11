cmake_minimum_required(VERSION 3.1)

project(jack_compiler)

add_executable(
        jcomp
        src/analyzer.cpp src/parser.h src/parser.cpp src/tokenizer.h src/tokenizer.cpp
        src/symbol_table.cpp src/symbol_table.h src/vm_writer.cpp src/vm_writer.h)
