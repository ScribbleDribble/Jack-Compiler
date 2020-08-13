//
// Created by mke on 31/07/2020.
//

#ifndef PARSER_H
#define PARSER_H

#endif //PARSER_H

#include <string>
#include <iostream>
#include "tokenizer.h"
#include "symbol_table.h"
#include <map>
#include "vm_writer.h"

class Parser {

private:
    std::ofstream& out_file;
    Tokenizer& tokenizer;
    std::string cur_token;
    std::string class_name;
    Symbol_Table symbol_table;
    VM_Writer& vm_writer;
    std::vector<std::string> expr_builder;
public:

    Parser(std::ofstream& out_file, Tokenizer& tokenizer, VM_Writer& vm_writer): out_file(out_file),
            tokenizer(tokenizer), vm_writer(vm_writer)
    {
        query_tokenizer();
        parse_class();
    }

    enum statements { LET = 9, IF = 10, ELSE = 11, WHILE = 12, RETURN = 13, DO = 14};
    std::map<std::string, int> keyword_constant_map = { {"true", 1}, {"false", 1}, {"null", 1},
                                                        {"this", 1} };

    void parse_class();
    void parse_class_var_dec();
    void parse_subroutine(const std::string& subroutine_type);
    void parse_param_list();
    void parse_subroutine_body();
    void parse_var_dec();



    void parse_statements();
    void parse_let_statement();
    void parse_if_statement();
    void parse_while_statement();
    void parse_do_statement();
    void parse_return_statement();

    void parse_expression();
    void parse_term();
    void parse_subroutine_call();
    void parse_expression_list();

    void query_tokenizer();

    // generates compiler errors
    void check_expected_x_after_y(const std::string& expected, const std::string& syntactic_element);
    void check_token_type_x_after_y(const std::string& expected_token_type, const std::string& syntactic_element);
    void check_type_exists();
    void check_op_exists(const std::string& op_token);



    // term detection helper functions
    bool is_array();

    // vm writer auxiliary functions

    std::vector<std::string> get_expression(std::vector<std::string>& tokens);

};