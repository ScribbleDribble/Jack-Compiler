//
// Created by mke on 10/08/2020.
//

#ifndef JACK_COMPILER_SYMBOL_TABLE_H
#define JACK_COMPILER_SYMBOL_TABLE_H

#include <iostream>
#include <map>

typedef struct IdentifierData {
    // int/string/bool/etc
    std::string type;
    // field/static/argument/var
    std::string kind;
    // scope number
    int index;
}IdentifierData;

class Symbol_Table {
private:
    std::map<std::string, IdentifierData*> class_scope;
    std::map<std::string, IdentifierData*> subroutine_scope;

    int static_count = 0;
    int field_count = 0;
    int var_count = 0;
    int arg_count = 0;

public:

    Symbol_Table() = default;

    void start_subroutine();
    void set_data(const std::string& name, const std::string& type, const std::string& kind);
    int get_static_count() const;
    int get_field_count() const;
    int get_arg_count() const;
    int get_var_count() const;
    std::string get_kind(const std::string& identifier);
    std::string get_type(const std::string& identifier);
    int get_index(const std::string& identifier);
    void free_class();
    void free_subroutine();

};


#endif //JACK_COMPILER_SYMBOL_TABLE_H
