//
// Created by mke on 10/08/2020.
//

#ifndef JACK_COMPILER_SYMBOL_TABLE_H
#define JACK_COMPILER_SYMBOL_TABLE_H

#include <iostream>
#include <map>

typedef struct IdentifierData {
    // int/string/bool/etc - /return type if function
    std::string type;
    // field/static/argument/var - /method/function
    std::string kind;
    // scope number
    int index;
}IdentifierData;

typedef struct FunctionIdentifierData : IdentifierData{
    std::string type;
    std::string kind;
    int index;
    int n_args;

}FunctionIdentifierData;

class Symbol_Table {
private:
    std::map<std::string, IdentifierData*> class_scope;
    std::map<std::string, IdentifierData*> subroutine_scope;
    // constructors are not to be stored
    std::map<std::string, FunctionIdentifierData*> functions;

    void free_subroutine();

    int static_count = 0;
    int field_count = 0;
    int var_count = 0;
    int arg_count = 0;
    int method_count = 0;
    int function_count = 0;
    int temp_n_args = 0;
    int label_count = 0;
public:

    Symbol_Table() = default;

    void start_subroutine();
    void set_data(const std::string& name, const std::string& type, const std::string& kind);
    void set_function_data(const std::string& name, const std::string& kind, int n_args);
    int get_static_count() const;
    int get_field_count() const;
    int get_arg_count() const;
    int get_var_count() const;
    int get_label_count() const;
    void increment_label_count();
    std::string get_kind(const std::string& identifier);
    std::string get_type(const std::string& identifier);
    int get_index(const std::string& identifier);
    void free_class();
    // temporarily hold data on how many arguments a subroutine has
    void set_temp_arg_count(int n_args);
    int get_temp_arg_count();
    int get_arg_count(const std::string& identifier);

    bool is_declared(const std::string& identifier);
    bool is_func(const std::string& identifier);
};


#endif //JACK_COMPILER_SYMBOL_TABLE_H
