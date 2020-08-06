#include "parser.h"

void Parser::parse_class() {
    // skip class declaration
    tokenizer.advance();
    tokenizer.advance();

    out_file << "<class>" << std::endl;

    if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
        out_file << "\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
        class_name = tokenizer.get_current_token();
    }

    else {
        std::cerr << "Compiler Error: Token '" + tokenizer.get_current_token() +
        "' is not a suitable class name." << std::endl;
    }

    tokenizer.advance();
    if (tokenizer.get_current_token() == "{") {
        out_file << "\t<symbol>" +tokenizer.get_current_token() + "</symbol>" << std::endl;
        tokenizer.advance();
    }

    else {
        std::cerr << "Compiler Error: Expected { instead of the token '" +
        tokenizer.get_current_token() <<"'" <<std::endl;
    }


    while(tokenizer.get_current_token() == "static" || tokenizer.get_current_token() == "field"){
        parse_class_var_dec();
        tokenizer.advance();
    }

    tokenizer.advance();
    if (tokenizer.type(tokenizer.get_current_token()) == "KEYWORD") {
//        parse_subroutine();
    }

    out_file << "</class>" << std::endl;
}


void Parser::parse_class_var_dec() {
    out_file << "\t<class_var_dec>" << std::endl;
    out_file << "\t\t<keyword>" + tokenizer.get_current_token() + "</keyword>" << std::endl;
    tokenizer.advance();

    if (tokenizer.type_exists(tokenizer.get_current_token()) || tokenizer.get_current_token() == class_name) {
        out_file << "\t\t<type>" + tokenizer.get_current_token() + "</type>" << std::endl;
        if (tokenizer.contains_more()) {
            tokenizer.advance();
        }
    }

    else {
        std::cerr << "Compiler Error: The token '" + tokenizer.get_current_token() + "' is not a valid type.";
        exit(0);
    }


    if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
        out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;

        if (tokenizer.contains_more()) {
            tokenizer.advance();
        }
    }

    else {
        std::cerr << "Compiler Error: Expected an identifier." << std::endl;
        exit(0);
    }

    while (tokenizer.get_current_token() == ",") {
        if (tokenizer.contains_more()) {
            tokenizer.advance();
        }
        if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
            out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
            if (tokenizer.contains_more()) {
                tokenizer.advance();
            }
        }
    }

    out_file << "\t</class_var_dec>" << std::endl;
}