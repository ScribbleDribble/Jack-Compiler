//
// Created by mke on 31/07/2020.
//

#ifndef PARSER_H
#define PARSER_H

#endif //PARSER_H

#include <string>
#include <iostream>
#include "tokenizer.h"

class Parser {

private:
    std::ofstream& out_file;
    Tokenizer& tokenizer;
public:

    Parser(std::ofstream& out_file, Tokenizer& tokenizer): out_file(out_file), tokenizer(tokenizer)
    {
        parse_class();
    }

    void parse_class();
    void parse_class_var_dec();
    void parse_type();
    void parse_subroutine_dec();
    void parse_param_list();
    void parse_subroutine_body();
    void parse_var_dec();
    void parse_class_name();
    void parse_subroutine_name();
    void parse_var_name();

    void parse_keyword();
    void parse_symbol();
    void parse_int_constant();
    void parse_string_constant();
    void parse_identifier();

    void parse_statements();
    void parse_statement();
    void parse_let_statement();
    void parse_if_statement();
    void parse_while_statement();
    void parse_do_statement();
    void parse_return_statement();

    void parse_expression();
    void parse_term();
    void parse_subroutine_call();
    void parse_expression_list();
    void parse_op();
    void parse_unary_op();
    void parse_keyword_constant();

};