#include "parser.h"
void Parser::query_tokenizer() {
    if (tokenizer.contains_more()) {
        tokenizer.advance();
    }
}

void Parser::parse_class() {
    // skip class declaration
    tokenizer.advance();
    tokenizer.advance();

    out_file << "<class>" << std::endl;

    check_token_type_x_after_y("IDENTIFIER", "class keyword");
    out_file << "\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
    tokenizer.add_type(tokenizer.get_current_token());
    class_name = tokenizer.get_current_token();
    query_tokenizer();


    if (tokenizer.get_current_token() == "{") {
        out_file << "\t<symbol>" +tokenizer.get_current_token() + "</symbol>" << std::endl;
        query_tokenizer();
    }

    else {
        std::cerr << "Compiler Error: expected { instead of the token '" +
        tokenizer.get_current_token() <<"'" <<std::endl;
    }


    while(tokenizer.get_current_token() == "static" || tokenizer.get_current_token() == "field"){
        parse_class_var_dec();
        query_tokenizer();
    }

    while (tokenizer.get_current_token() == "constructor"  || tokenizer.get_current_token() == "function" ||
            tokenizer.get_current_token() == "method") {
        const std::string subroutine_type = tokenizer.get_current_token();
        query_tokenizer();
        parse_subroutine(subroutine_type);
    }

    out_file << "\n</class>" << std::endl;
}


void Parser::parse_class_var_dec() {
    out_file << "\t<class_var_dec>" << std::endl;
    out_file << "\t\t<keyword>" + tokenizer.get_current_token() + "</keyword>" << std::endl;
    tokenizer.advance();

    if (tokenizer.type_exists(tokenizer.get_current_token())) {
        out_file << "\t\t<type>" + tokenizer.get_current_token() + "</type>" << std::endl;
        query_tokenizer();
    }

    else {
        std::cerr << "Compiler Error: the token '" + tokenizer.get_current_token() + "' is not a valid type.";
        exit(0);
    }


    if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
        out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
        query_tokenizer();
    }

    else {
        std::cerr << "Compiler Error: expected an identifier." << std::endl;
        exit(0);
    }

    while (tokenizer.get_current_token() == ",") {
        query_tokenizer();
        if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
            out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
            query_tokenizer();
        }
    }

    check_expected_x_after_y(";", "identifier");
    out_file << "\t\t<symbol>;</symbol>";

    out_file << "\n\t</class_var_dec>" << std::endl;
}

void Parser::parse_subroutine(const std::string& subroutine_type) {
    out_file << "\t<subroutine>" << std::endl;

    out_file << "\t\t<keyword>" + subroutine_type + "</keyword>" << std::endl;

    if (subroutine_type == "constructor") {
        if (tokenizer.get_current_token() != class_name) {
            std::cerr << "Invalid constructor identifier" << std::endl;
            exit(0);
        }
        query_tokenizer();
    }

    else if (tokenizer.type_exists(tokenizer.get_current_token()) || tokenizer.get_current_token() == "void") {

        out_file << "\t\t<type>" + tokenizer.get_current_token() + "</type>" << std::endl;
        query_tokenizer();

        if (tokenizer.type(tokenizer.get_current_token()) == "IDENTIFIER") {
            query_tokenizer();
        }
        else {
            std::cerr << "Compiler Error: Expected identifier after subroutine type definition" << std::endl;
        }

    }
    else {
        std::cerr << "Compiler Error: '" + tokenizer.get_current_token() + "' not a valid type for a subroutine"
        << std::endl;
    }


    check_expected_x_after_y("(", "identifier");
    out_file << "\t\t<symbol>(</symbol>" << std::endl;
    query_tokenizer();

    parse_param_list();
    check_expected_x_after_y(")", "parameter list");
    out_file << "\t\t<symbol>)</symbol>" << std::endl;

    query_tokenizer();
    parse_subroutine_body();
}

void Parser::parse_param_list() {
    out_file << "\t\t<param_list>" << std::endl;

    int param_counter = 0;

    while(tokenizer.type_exists(tokenizer.get_current_token()) || param_counter > 0) {

        if (param_counter > 0) {
            check_expected_x_after_y(",", "identifier");
            out_file <<"\t\t\t<symbol>" + tokenizer.get_current_token() + "</symbol>" << std::endl;
            query_tokenizer();
        }

        out_file << "\t\t\t<type>" + tokenizer.get_current_token() + "</type>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", "type");
        out_file << "\t\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
        query_tokenizer();
        param_counter++;

        if (tokenizer.get_current_token() == ")") {
            break;
        }
    }

    out_file << "\t\t</param_list>" << std::endl;
}

void Parser::parse_subroutine_body() {
    if (tokenizer.get_current_token() != "{") {
        std::cerr << "Expected { after parameter list" << std::endl;
        exit(0);
    }
    out_file << "\t\t<symbol>{</symbol>" << std::endl;
    query_tokenizer();

    parse_var_dec();
    parse_statements();

}

void Parser::parse_var_dec() {

    int variable_count = 0;
    while(tokenizer.get_current_token() == "var" || variable_count > 0) {
        out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
        query_tokenizer();

        check_type_exists();
        out_file << "\t\t<type>" + tokenizer.get_current_token() + "</type>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", "type");
        out_file << "\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
        query_tokenizer();

        variable_count++;
        if (tokenizer.get_current_token() == ";")
        {
            out_file << "\t\t<symbol>;</symbol>" << std::endl;
            break;
        }

        if (tokenizer.get_current_token() != ",") {
            std::cerr << "Expected either a , or ;" << std::endl;
        }

        query_tokenizer();
    }


}

void Parser::parse_statements() {
    out_file << "\t\t<statements>" << std::endl;

    int keyword = tokenizer.get_keyword_val(tokenizer.get_current_token());

    while (keyword >= LET && keyword <= DO) {
        query_tokenizer();

        switch (keyword) {
            case LET:
                parse_let_statement();
                break;

            case IF:
                parse_if_statement();
                break;

            case WHILE:
                parse_while_statement();
                break;

            case RETURN:
                parse_return_statement();
                break;

            case DO:
                parse_do_statement();
                break;

            default:
                std::cerr << "Expected a statement instead of " + tokenizer.get_current_token() << std::endl;
                exit(0);
        }

    }

    out_file << "\t\t</statements>" << std::endl;
}

void Parser::parse_let_statement() {
    out_file << "\t\t<let_statement>" << std::endl;
    check_token_type_x_after_y("IDENTIFIER", "let statement");
    out_file << "\t\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
    query_tokenizer();

    if (tokenizer.get_current_token() == "[") {
        out_file << "\t\t\t<symbol>[</symbol>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("INT_CONST", "symbol");
        out_file << "\t\t\t<int>" + tokenizer.get_current_token() + "</int>" << std::endl;
        query_tokenizer();

        check_expected_x_after_y("SYMBOL", "integer");
        out_file << "\t\t\t<symbol>" + tokenizer.get_current_token() + "</symbol>" << std::endl;
        query_tokenizer();
    }

    if (tokenizer.get_current_token() != "=") {
        std::cerr << "Compiler Error: Expected an = instead of " + tokenizer.get_current_token();
        exit(0);
    }

    out_file << "\t\t\t<symbol>=</symbol>" << std::endl;
    query_tokenizer();

    parse_expression();
    out_file << "\t\t\t</let_statement>" << std::endl;
}

void Parser::parse_if_statement() {

}

void Parser::parse_while_statement() {

}

void Parser::parse_return_statement() {

}

void Parser::parse_do_statement() {

}

void Parser::parse_expression() {
//    check_token_type_x_after_y("INT_CONST", "=");
//    out_file << "\t\t\t<int>" + tokenizer.get_current_token() + "</int>" << std::endl;
//    query_tokenizer();

    check_op_exists(tokenizer.get_current_token());
    out_file << "\t\t\t<symbol>" + tokenizer.get_current_token() + "</symbol>" << std::endl;
    query_tokenizer();

//    check_token_type_x_after_y("IDENTIFIER", "after operator symbol");
//    out_file << "\t\t\t<"
}

void Parser::check_expected_x_after_y(const std::string& expected, const std::string& syntactic_element) {
    // only to be called once the token to be compared has already been advanced to the front of the queue
    if (tokenizer.get_current_token() != expected) {
        std::cerr << "Compiler Error: expected " + expected + " after " + syntactic_element;
        exit(0);
    }
}

void Parser::check_token_type_x_after_y(const std::string& expected_token_type, const std::string& syntactic_element) {
    // syntactic token types
    if (tokenizer.type(tokenizer.get_current_token()) != expected_token_type) {
        std::cerr << "Compiler Error: expected " + expected_token_type + " after " + syntactic_element;
        exit(0);
    }
}

void Parser::check_type_exists() {
    // types defined by language
    if (!tokenizer.type_exists(tokenizer.get_current_token())) {
        std::cerr << "Compiler Error: '" + tokenizer.get_current_token() + "' is not a valid type";
        exit(0);
    }
}

void Parser::check_op_exists(const std::string& op_token) {
    if (!tokenizer.op_exists()) {
        std::cerr << "Compiler Error: expected a valid operator but instead got" + tokenizer.get_current_token();
        exit(0);
    }
}

