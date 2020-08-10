//
// Created by mke on 10/08/2020.
//

#include "symbol_table.h"
void Symbol_Table::start_subroutine() {
    subroutine_scope.clear();
    arg_count = 0;
    var_count = 0;
}

void Symbol_Table::set_data(const std::string& name, const std::string& type, const std::string& kind) {
    IdentifierData *identifier_data;
    identifier_data->type = name;
    identifier_data->kind = kind;
    if (kind == "ARG" || kind == "VAR") {
        subroutine_scope[name] = identifier_data;
        if (type == "ARG") {
            arg_count++;
        }
        else {
            var_count++;
        }
    }
    else {
        class_scope[name] = identifier_data;
        if (type == "FIELD") {
            field_count++;
        }
        else {
            static_count++;
        }
    }
};

int Symbol_Table::get_static_count() const {
    return static_count;
}

int Symbol_Table::get_field_count() const {
    return field_count;
}

int Symbol_Table::get_arg_count() const {
    return arg_count;
}

int Symbol_Table::get_var_count() const {
    return var_count;
}

std::string Symbol_Table::get_kind(const std::string& identifier) {
    if (class_scope.count(identifier) > 0) {
        return class_scope[identifier]->kind;
    }
    return subroutine_scope[identifier]->kind;
}

std::string Symbol_Table::get_type(const std::string& identifier) {
    if (class_scope.count(identifier) > 0) {
        return class_scope[identifier]->type;
    }
    return subroutine_scope[identifier]->type;
}

int Symbol_Table::get_index(const std::string& identifier) {
    if (class_scope.count(identifier) > 0) {
        return class_scope[identifier]->index;
    }
    return subroutine_scope[identifier]->index;
}

