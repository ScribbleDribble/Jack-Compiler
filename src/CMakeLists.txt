cmake_minimum_required(VERSION 3.1)

project(jack_compiler)

add_executable(
        jcomp
        analyzer.cpp parser.h parser.cpp tokenizer.h tokenizer.cpp
)
