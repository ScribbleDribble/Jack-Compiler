#include "parser.h"
void Parser::query_tokenizer() {
    if (tokenizer.contains_more()) {
        tokenizer.advance();
        cur_token =  tokenizer.get_current_token();
    }
}

void Parser::parse_class() {
    // skip class declaration
    query_tokenizer();
    query_tokenizer();

    out_file << "<class>" << std::endl;

    check_token_type_x_after_y("IDENTIFIER", "class keyword");
    out_file << "\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
    tokenizer.add_type(tokenizer.get_current_token());
    class_name = cur_token;
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

    while (cur_token == "constructor"  || cur_token == "function" || cur_token == "method") {
        const std::string subroutine_type = tokenizer.get_current_token();
        query_tokenizer();
        parse_subroutine(subroutine_type);
    }

    out_file << "</class>" << std::endl;
}


void Parser::parse_class_var_dec() {
    out_file << "\t<class_var_dec>" << std::endl;
    out_file << "\t\t<keyword>" + tokenizer.get_current_token() + "</keyword>" << std::endl;
    query_tokenizer();

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
    out_file << "\t\t<symbol>;</symbol>" << std::endl;

    out_file << "\t</class_var_dec>" << std::endl;
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
            out_file << "\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
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

    out_file << "\t</subroutine>" << std::endl;
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

    check_expected_x_after_y("}", "statement(s)");
    out_file << "\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

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

    int keyword = tokenizer.get_keyword_val(cur_token);

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
        keyword = tokenizer.get_keyword_val(cur_token);
    }
    out_file << "\t\t</statements>" << std::endl;
}

void Parser::parse_let_statement() {
    out_file << "\t\t\t<let_statement>" << std::endl;
    check_token_type_x_after_y("IDENTIFIER", "let statement");
    out_file << "\t\t\t\t<identifier>" + tokenizer.get_current_token() + "</identifier>" << std::endl;
    query_tokenizer();

    if (tokenizer.get_current_token() == "[") {
        out_file << "\t\t\t\t<symbol>[</symbol>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("INT_CONST", "symbol");
        out_file << "\t\t\t\t<int>" + cur_token + "</int>" << std::endl;
        query_tokenizer();

        check_expected_x_after_y("]", "integer");
        out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
        query_tokenizer();
        check_expected_x_after_y("=", "]");
    }
    else {
        check_expected_x_after_y("=", "identifier");
    }

    out_file << "\t\t\t\t<symbol>=</symbol>" << std::endl;
    query_tokenizer();

    parse_expression();

    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    out_file << "\t\t\t</let_statement>" << std::endl;
    query_tokenizer();
}

void Parser::parse_if_statement() {

}

void Parser::parse_while_statement() {

}

void Parser::parse_return_statement() {
    out_file << "\t\t\t<return_statement>" << std::endl;
    if (cur_token != ";") {
        parse_expression();
    }
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    out_file << "\t\t\t</return_statement>" << std::endl;
}

void Parser::parse_do_statement() {
    // TODO test
    out_file << "\t\t\t<do_statement>" << std::endl;
    parse_subroutine_call();

    check_expected_x_after_y(";", "subroutine call");
    out_file << "<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();
}

void Parser::parse_expression() {
    out_file << "\t\t\t\t<expression>" << std::endl;
    parse_term();

    while (tokenizer.op_exists(cur_token)) {
        out_file << "\t\t\t\t\t<symbol>" + cur_token << "</symbol>" << std::endl;
        query_tokenizer();
        parse_term();
    }

    out_file << "\t\t\t\t</expression>" << std::endl;
//    query_tokenizer();
}

void Parser::parse_term() {

    std::string cur_token_type = tokenizer.type(cur_token);

    if (cur_token_type == "IDENTIFIER") {
        out_file << "\t\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        query_tokenizer();
        if (is_array())
        {
            out_file << "\t\t\t\t\t<symbol>[</symbol>" << std::endl;
            parse_expression();
            out_file << "\t\t\t\t\t<symbol>]</symbol>" << std::endl;
        }
        parse_subroutine_call();
    }
    else if (cur_token_type == "STRING_CONST" || cur_token_type == "INT_CONST" ||
                                                                            keyword_constant_map.count(cur_token) > 0) {

        out_file << "\t\t\t\t\t<" + cur_token_type +  ">" +
                    cur_token + "</" + cur_token_type + ">" << std::endl;
        query_tokenizer();
    }

    else if(cur_token == "(") {
        out_file << "\t\t\t\t\t<symbol>(</symbol>" << std::endl;
        query_tokenizer();
        parse_expression();

        check_expected_x_after_y(")", "expression");
        out_file << "\t\t\t\t\t<symbol>)</symbol>" << std::endl;
        query_tokenizer();
    }

    else if (cur_token == "-") {
        out_file << "\t\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
        query_tokenizer();
        parse_term();
    }
}

void Parser::parse_subroutine_call() {
    // only to be called once we know Tokenizer::type() == "IDENTIFIER"
    if (tokenizer.get_current_token() == ".") {
        out_file << "\t\t\t\t\t<symbol>.</symbol>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", ".");
        out_file << "\t\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        query_tokenizer();

        check_expected_x_after_y("(", ".");
        out_file << "\t\t\t\t\t<symbol>(</symbol>" << std::endl;
        query_tokenizer();

        if (cur_token != ")") {
            parse_expression_list();
        }
        else {
            out_file << "\t\t\t\t\t<symbol>)</symbol>" << std::endl;
            query_tokenizer();
        }
    }
    else if (cur_token == "(") {
        query_tokenizer();
        check_expected_x_after_y(")", "(");
        out_file << "\t\t\t\t\t<symbol>(</symbol>\n\t\t\t\t\t<symbol>)</symbol>" << std::endl;
        query_tokenizer();
    }
}

void Parser::parse_expression_list() {
    out_file << "\t\t\t\t\t<expression_list>" << std::endl;
    parse_expression();

    while (cur_token == ",") {
        out_file << "\t\t\t\t\t<symbol>,</symbol>" << std::endl;
        query_tokenizer();

        parse_expression();
    }

    check_expected_x_after_y(")", "identifier");
    out_file << "\t\t\t\t\t<symbol>)</symbol>" << std::endl;
    out_file << "\t\t\t\t</expression_list>" << std::endl;
    query_tokenizer();
}

void Parser::check_expected_x_after_y(const std::string& expected, const std::string& syntactic_element) {
    // only to be called once the token to be compared has already been advanced to the front of the queue
    // compares x to the current token and displays error message mentioning x was expected after y
    if (tokenizer.get_current_token() != expected) {
        std::cerr << "Compiler Error: expected " + expected + " after " + syntactic_element;
        exit(0);
    }
}

void Parser::check_token_type_x_after_y(const std::string& expected_token_type, const std::string& syntactic_element) {
    // syntactic token types
    if (tokenizer.type(cur_token) != expected_token_type) {
        std::cerr << "Compiler Error: expected " + expected_token_type + " after " + syntactic_element;
        exit(0);
    }
}

void Parser::check_type_exists() {
    // types defined by language + user defined types
    if (!tokenizer.type_exists(cur_token)) {
        std::cerr << "Compiler Error: '" + cur_token + "' is not a valid type";
        exit(0);
    }
}

void Parser::check_op_exists(const std::string& op_token) {
    if (!tokenizer.op_exists(op_token)) {
        std::cerr << "Compiler Error: expected a valid operator but instead got " + cur_token;
        exit(0);
    }
}

bool Parser::is_array() {
    // only to be called once we know Tokenizer::type() == "IDENTIFIER"
    if (tokenizer.get_current_token() != "[") {
        return false;
    }
    query_tokenizer();

    check_expected_x_after_y("]", "[");
    return true;
}


// TODO replace all tokenizer.get_current_token() for cur_token