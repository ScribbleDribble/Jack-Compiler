cmake_minimum_required(VERSION 3.1)

project(jack_compiler)

add_executable(
        jcomp
        analyzer.h analyzer.cpp compilation_engine.h compilation_engine.cpp tokenizer.h tokenizer.cpp
)
