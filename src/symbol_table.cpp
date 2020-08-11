//
// Created by mke on 10/08/2020.
//

#include "symbol_table.h"
void Symbol_Table::start_subroutine() {
    free_subroutine();
    subroutine_scope.clear();
    arg_count = 0;
    var_count = 0;
}

void Symbol_Table::free_class() {
    for (auto & it : class_scope) {
        delete it.second;
    }
}

void Symbol_Table::free_subroutine() {
    for (auto& it : subroutine_scope) {
        delete it.second;
    }
}

void Symbol_Table::set_data(const std::string& name, const std::string& type, const std::string& kind) {
    auto *identifier_data = new IdentifierData;
    identifier_data->type = type;
    identifier_data->kind = kind;

    if (kind == "ARG" || kind == "VAR") {
        subroutine_scope[name] = identifier_data;
        if (kind == "ARG") {
            identifier_data->index = arg_count++;
        }
        else {
            identifier_data->index = var_count++;
        }
    }
    else {
        class_scope[name] = identifier_data;
        if (kind == "FIELD") {
            identifier_data->index = field_count++;
        }
        else {
            identifier_data->index = static_count++;
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

