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
    std::string current_token;
    std::string class_name;

public:

    Parser(std::ofstream& out_file, Tokenizer& tokenizer): out_file(out_file), tokenizer(tokenizer)
    {
        parse_class();
    }

    void parse_class();
    void parse_class_var_dec();
    void parse_subroutine();
    void parse_param_list();
    void parse_var_dec();



    void parse_statements();
    void parse_let_statement();
    void parse_if_statement();
    void parse_while_statement();
    void parse_do_statement();
    void parse_return_statement();

    void parse_expression();
    void parse_term();
    void parse_expression_list();


};