#include "parser.h"

void Parser::parse_class() {
    std::string token = tokenizer.advance();
    out_file << "<" + token +">";
    parse_class_var_dec();

}

void Parser::parse_class_var_dec() {
    std::string token = tokenizer.advance();
    out_file << "<" + token + ">";
}