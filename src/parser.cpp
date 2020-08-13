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

    check_expected_x_after_y("}", "class definition");
    out_file << "</class>" << std::endl;
    query_tokenizer();

    while (cur_token == "function") {
        query_tokenizer();
        symbol_table.start_subroutine();
        symbol_table.free_subroutine();
        parse_subroutine("function");
    }

    symbol_table.free_subroutine();
    symbol_table.free_class();
}


void Parser::parse_class_var_dec() {
    std::string type, kind, name;

    out_file << "\t<class_var_dec>" << std::endl;
    out_file << "\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
    kind = Tokenizer::to_upper(cur_token);
    query_tokenizer();


    if (tokenizer.type_exists(cur_token)) {
        out_file << "\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        type = cur_token;
        query_tokenizer();
    }
    else {
        std::cerr << "Compiler Error: the token '" + cur_token + "' is not a valid type.";
        exit(0);
    }

    if (tokenizer.type(cur_token) == "IDENTIFIER") {
        out_file << "\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        name = cur_token;
        symbol_table.set_data(name, type, kind);
        query_tokenizer();
    }
    else {
        std::cerr << "Compiler Error: expected an identifier." << std::endl;
        exit(0);
    }

    while (cur_token == ",") {
        query_tokenizer();
        if (tokenizer.type(cur_token) == "IDENTIFIER") {
            out_file << "\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
            symbol_table.set_data(cur_token, type, kind);
            query_tokenizer();
        }
    }

    check_expected_x_after_y(";", "identifier");
    out_file << "\t\t<symbol>;</symbol>" << std::endl;

    out_file << "\t</class_var_dec>" << std::endl;
}

void Parser::parse_subroutine(const std::string& subroutine_type) {
    out_file << "\t<subroutine>" << std::endl;

    std::string name;

    out_file << "\t\t<keyword>" + subroutine_type + "</keyword>" << std::endl;

    if (subroutine_type == "constructor") {
        if (tokenizer.get_current_token() != class_name) {
            std::cerr << "Invalid constructor identifier" << std::endl;
            exit(0);
        }

        query_tokenizer();

        check_expected_x_after_y("new", class_name);
        out_file << "\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        query_tokenizer();
    }
    else if (tokenizer.type_exists(tokenizer.get_current_token()) || tokenizer.get_current_token() == "void") {

        out_file << "\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", "subroutine definition");
        out_file << "\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        name = cur_token;
        query_tokenizer();
    }
    else {
        std::cerr << "Compiler Error: '" + cur_token + "' not a valid type for a subroutine"
        << std::endl;
    }
    check_expected_x_after_y("(", "identifier");
    out_file << "\t\t<symbol>(</symbol>" << std::endl;
    query_tokenizer();

    if (subroutine_type == "constructor") {
        parse_param_list();
        vm_writer.write_function(class_name+"."+name, symbol_table.get_temp_arg_count());
    }
    else if (subroutine_type == "function") {
        parse_param_list();
        symbol_table.set_function_data(name, "function");
        vm_writer.write_function(name, symbol_table.get_temp_arg_count());
    }
    else {
        parse_param_list();
        symbol_table.set_function_data(name, "method");
        vm_writer.write_function(class_name + "." + name, symbol_table.get_temp_arg_count() + 1);
    }


    check_expected_x_after_y(")", "parameter list");
    out_file << "\t\t<symbol>)</symbol>" << std::endl;

    query_tokenizer();
    parse_subroutine_body();

    out_file << "\t</subroutine>" << std::endl;
}

void Parser::parse_param_list() {
    out_file << "\t\t<param_list>" << std::endl;
    std::string name, type, kind = "ARG";

    vm_writer.write_push("argument", 0);
    vm_writer.write_pop("this", 0);

    int param_counter = 0;
    while(tokenizer.type_exists(cur_token) || param_counter > 0) {

        if (param_counter > 0) {
            check_expected_x_after_y(",", "identifier");
            out_file <<"\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
            query_tokenizer();
        }

        out_file << "\t\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        type = cur_token;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", "type");
        out_file << "\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        symbol_table.set_data(cur_token, type, kind);
        query_tokenizer();
        param_counter++;

        if (tokenizer.get_current_token() == ")") {
            break;
        }
    }

    out_file << "\t\t</param_list>" << std::endl;
    symbol_table.set_temp_arg_count(param_counter);
}

void Parser::parse_subroutine_body() {
    check_expected_x_after_y("{", "parameter list");

    out_file << "\t\t<symbol>{</symbol>" << std::endl;
    query_tokenizer();

   while (cur_token == "var") {
        parse_var_dec();
    }
    parse_statements();

    check_expected_x_after_y("}", "statement(s)");
    out_file << "\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

}

void Parser::parse_var_dec() {
    out_file << "\t\t<varDec>" << std::endl;

    std::string name, type, kind = "VAR";

    int variable_count = 0;
    while(cur_token == "var" || variable_count > 0) {
        out_file << "\t\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        query_tokenizer();

        check_type_exists();
        out_file << "\t\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        type = cur_token;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", "type");
        out_file << "\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        name = cur_token;
        symbol_table.set_data(name, type, kind);
        query_tokenizer();

        variable_count++;
        if (cur_token == ";")
        {
            out_file << "\t\t\t<symbol>;</symbol>" << std::endl;
            break;
        }
        if (cur_token != ",") {
            std::cerr << "Expected either a , or ;" << std::endl;
        }
        query_tokenizer();
    }
    check_expected_x_after_y(";", "variable declaration");
    query_tokenizer();

    out_file << "\t\t</varDec>" << std::endl;
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
    out_file << "\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
    std::vector<std::string> expr_tokens = tokenizer.get_line();
    std::vector<std::string>expression = get_expression(expr_tokens);

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
    out_file << "\t\t\t<if_statement>" << std::endl;

    check_expected_x_after_y("(", "if statement");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    parse_expression();

    check_expected_x_after_y(")", "expression");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    check_expected_x_after_y("{", ")");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    parse_statements();

    check_expected_x_after_y("}", "statements");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    if (cur_token == "else") {
        out_file << "\t\t\t<else_statement>" << std::endl;
        query_tokenizer();

        check_expected_x_after_y("{", "else statement");
        out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;

        parse_statements();

        check_expected_x_after_y("}", "statements");
        out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    }
    out_file << "\t\t\t</if_statement>" << std::endl;
}

void Parser::parse_while_statement() {
    out_file << "\t\t\t<while_statement>" << std::endl;

    check_expected_x_after_y("(", "while statement");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    parse_expression();

    check_expected_x_after_y(")", "expression");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    check_expected_x_after_y("{", ")");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    parse_statements();
    expr_builder.clear();

    check_expected_x_after_y("}", "statements");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    out_file << "\t\t\t</while_statement>" << std::endl;
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

    check_token_type_x_after_y("IDENTIFIER", "do statement");
    out_file << "\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
    query_tokenizer();

    parse_subroutine_call();

    check_expected_x_after_y(";", "subroutine call");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    out_file << "\t\t\t</do_statement>" << std::endl;

}

void Parser::parse_expression() {
    out_file << "\t\t\t\t<expression>" << std::endl;
    parse_term();

    while (tokenizer.op_exists(cur_token)) {
        out_file << "\t\t\t\t\t<symbol>" + cur_token << "</symbol>" << std::endl;
        expr_builder.push_back(cur_token);
        query_tokenizer();
        parse_term();
    }

    out_file << "\t\t\t\t</expression>" << std::endl;
}

void Parser::parse_term() {

    std::string cur_token_type = tokenizer.type(cur_token);
    if (cur_token_type == "IDENTIFIER") {
        out_file << "\t\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;

        if (symbol_table.is_declared(cur_token))
        {
            expr_builder.push_back(cur_token);
        }

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
        if (cur_token_type == "INT_CONST") {
            expr_builder.push_back(cur_token);
        }
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
    else if (cur_token == "-" || cur_token == "~") {
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
        out_file << "\t\t\t\t\t<symbol>(</symbol>" << std::endl;
        query_tokenizer();

        if (cur_token != ")") {
            parse_expression_list();
        }
        else {
            check_expected_x_after_y(")", "(");
            out_file << "\t\t\t\t\t<symbol>)</symbol>" << std::endl;
            query_tokenizer();
        }
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

std::vector<std::string> Parser::get_expression(std::vector<std::string>& tokens) {
    std::vector<std::string> expr;
    for (std::string& token : tokens) {
        if (token == "="){
            continue;
        }
        else if (symbol_table.is_declared(token) || tokenizer.op_exists(token) ||
                    tokenizer.type(token) == "INT_CONST" || tokenizer.type(token) == "STRING_CONST") {
            expr.push_back(token);
        }
    }
    return expr;
}

//void Parser::expression_code_gen(const std::string& expression) {


// TODO replace all tokenizer.get_current_token() for cur_token
// TODO replace all cerrs with exceptions