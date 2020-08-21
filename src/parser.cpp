#include "parser.h"


std::string Parser::get_segment(const std::string& token) {
    if (!symbol_table.is_declared(token)) {
        if (tokenizer.type(token) == "INT_CONST"){
            return "constant";
        }
        return "none";
    }
    else {
        if (symbol_table.get_kind(token) == "FIELD" || symbol_table.get_kind(token) == "STATIC") {
            return "this";
        }
        else if (symbol_table.get_kind(token) == "ARG") {
            return "argument";
        }
        else {
            return "local";
        }
    }
}

void Parser::query_tokenizer() {
    if (tokenizer.contains_more()) {
        prev_token = cur_token;
        tokenizer.advance();
        cur_token =  tokenizer.get_current_token();
    }
}

void Parser::parse_class() {
    // skip class declaration
    query_tokenizer();

    out_file << "<class>" << std::endl;

    check_token_type_x_after_y("IDENTIFIER", "class keyword");
    out_file << "\t<identifier>" + cur_token + "</identifier>" << std::endl;
    tokenizer.add_type(cur_token);
    class_name = cur_token;
    query_tokenizer();

    if (tokenizer.get_current_token() == "{") {
        out_file << "\t<symbol>" + cur_token + "</symbol>" << std::endl;
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
        cur_subroutine_type = cur_token;
        query_tokenizer();
        parse_subroutine();
    }

    check_expected_x_after_y("}", "class definition");
    out_file << "</class>" << std::endl;
    query_tokenizer();

    while (cur_token == "function") {
        cur_subroutine_type = "function";
        query_tokenizer();
        symbol_table.start_subroutine();
        parse_subroutine();
    }

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

void Parser::parse_subroutine() {
    out_file << "\t<subroutine>" << std::endl;

    std::string name, return_type;
    out_file << "\t\t<keyword>" + cur_subroutine_type + "</keyword>" << std::endl;

    if (cur_subroutine_type == "constructor") {
        if (tokenizer.get_current_token() != class_name) {
            std::cerr << "Invalid constructor identifier" << std::endl;
            exit(0);
        }

        query_tokenizer();

        check_expected_x_after_y("new", class_name);
        out_file << "\t\t<keyword>" + cur_token + "</keyword>" << std::endl;
        query_tokenizer();
    }

    else if (tokenizer.type_exists(cur_token) || cur_token == "void") {
        return_type = cur_token;
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

    if (cur_subroutine_type == "constructor") {
        parse_param_list();
        vm_writer.write_function(class_name+"."+name, symbol_table.get_temp_arg_count());
    }
    else if (cur_subroutine_type == "function") {
        parse_param_list();
        int n_args = symbol_table.get_temp_arg_count();
        vm_writer.write_function(class_name + "." + name, n_args);
        symbol_table.set_function_data(name, "function", n_args);

    }
    else {
        parse_param_list();
        int n_args = symbol_table.get_temp_arg_count();
        symbol_table.set_function_data(name, "method", n_args);
        vm_writer.write_function(class_name + "." + name, n_args + 1);
        vm_writer.write_push("argument", 0);
        vm_writer.write_pop("this", 0);

    }


    check_expected_x_after_y(")", "parameter list");
    out_file << "\t\t<symbol>)</symbol>" << std::endl;

    query_tokenizer();
    parse_subroutine_body();

    // functions and void return type methods are specified to return 0;
    if (cur_subroutine_type == "function" || return_type == "void") {
        vm_writer.write_push("constant", 0);
    }
    vm_writer.write_return();
    out_file << "\t</subroutine>" << std::endl;
    symbol_table.start_subroutine();
}

void Parser::parse_param_list() {
    out_file << "\t\t<param_list>" << std::endl;
    std::string name, type, kind = "ARG";


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

    if (cur_token == ";")
    {
        out_file << "\t\t\t<symbol>;</symbol>" << std::endl;
        query_tokenizer();
        return;
    }
    if (cur_token != "," && cur_token != ";") {
        std::cerr << "Expected either a , or ;" << std::endl;
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
        }
        keyword = tokenizer.get_keyword_val(cur_token);
    }
    out_file << "\t\t</statements>" << std::endl;
}

void Parser::parse_let_statement() {
    out_file << "\t\t\t<let_statement>" << std::endl;
    std::string identifier = cur_token;
    check_token_type_x_after_y("IDENTIFIER", "let statement");
    out_file << "\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
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
    std::string segment = get_segment(identifier);
    if (segment == "none") {
        std::cerr << "Identifier '" + identifier  + "' was not declared" << std::endl;
        exit(0);
    }
    vm_writer.write_pop(get_segment(identifier), symbol_table.get_index(identifier));
    out_file << "\t\t\t</let_statement>" << std::endl;
    query_tokenizer();
}

void Parser::parse_if_statement() {
    out_file << "\t\t\t<if_statement>" << std::endl;

    int label_count = symbol_table.get_label_count();
    vm_writer.write_if(label_count);
    // reserve two labels for each potential if-else pair
    symbol_table.increment_label_count();
    symbol_table.increment_label_count();


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
        vm_writer.write_label(label_count);
        label_count++;
        query_tokenizer();

        check_expected_x_after_y("{", "else statement");
        out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
        query_tokenizer();

        if (tokenizer.get_keyword_val(cur_token) < LET && tokenizer.get_keyword_val(cur_token) > DO) {
            std::cerr << "Expected a statement after {" << std::endl;
            exit(0);
        }
        parse_statements();

        check_expected_x_after_y("}", "statements");
        out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
        vm_writer.write_goto(label_count);

        // increment for the next (potential) if statement
        symbol_table.increment_label_count();
        query_tokenizer();
    }

    vm_writer.write_label(label_count);
    out_file << "\t\t\t</if_statement>" << std::endl;
}

void Parser::parse_while_statement() {
    out_file << "\t\t\t<while_statement>" << std::endl;

    check_expected_x_after_y("(", "while statement");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    // start of loop label
    int label_count = symbol_table.get_label_count();
    vm_writer.write_label(label_count);
    symbol_table.increment_label_count();
    symbol_table.increment_label_count();

    parse_expression();
    vm_writer.write_if(label_count + 1);

    check_expected_x_after_y(")", "expression");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    check_expected_x_after_y("{", ")");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    parse_statements();
    vm_writer.write_goto(label_count);

    check_expected_x_after_y("}", "statements");
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

    vm_writer.write_label(label_count + 1);

    out_file << "\t\t\t</while_statement>" << std::endl;
}

void Parser::parse_return_statement() {
    out_file << "\t\t\t<return_statement>" << std::endl;
    if (cur_token != ";") {
        parse_expression();
    }
    out_file << "\t\t\t\t<symbol>" + cur_token + "</symbol>" << std::endl;
    query_tokenizer();

//    vm_writer.write_return();
    out_file << "\t\t\t</return_statement>" << std::endl;
}

void Parser::parse_do_statement() {
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
    if (!tokenizer.op_exists(cur_token)) {
        expression_writer(cur_token);
    }
    parse_term();

    while (tokenizer.op_exists(cur_token)) {
        out_file << "\t\t\t\t\t<symbol>" + cur_token << "</symbol>" << std::endl;
        std::string symbol = cur_token;

        query_tokenizer();
        expression_writer(cur_token);
        parse_term();
        expression_writer(symbol);
    }

    out_file << "\t\t\t\t</expression>" << std::endl;
}

void Parser::parse_term() {

    std::string cur_token_type = tokenizer.type(cur_token);
    if (cur_token_type == "IDENTIFIER") {
        out_file << "\t\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        std::string temp_identifier;

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
        }
        out_file << "\t\t\t\t\t<" + cur_token_type +  ">" +
                    cur_token + "</" + cur_token_type + ">" << std::endl;
//        vm_writer.write_push("constant", std::stoi(cur_token));
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
        std::string symbol = cur_token;

        query_tokenizer();
        // need to create an exception for int const values
        if (tokenizer.type(cur_token) == "INT_CONST") {
            vm_writer.write_push("constant", std::stoi(cur_token));
        }
        parse_term();
        if (symbol == "-") {
            vm_writer << "neg";
        }
        else {
            vm_writer.write_arithmetic("~");
        }
    }
}

void Parser::parse_subroutine_call() {
    // only to be called once we know Tokenizer::type() == "IDENTIFIER"

    if (tokenizer.get_current_token() == ".") {
        std::string type_name = prev_token;
        out_file << "\t\t\t\t\t<symbol>.</symbol>" << std::endl;
        query_tokenizer();

        check_token_type_x_after_y("IDENTIFIER", ".");
        out_file << "\t\t\t\t\t<identifier>" + cur_token + "</identifier>" << std::endl;
        std::string method_name = cur_token;
        query_tokenizer();

        check_expected_x_after_y("(", ".");
        out_file << "\t\t\t\t\t<symbol>(</symbol>" << std::endl;
        query_tokenizer();

        if (cur_token != ")") {
            parse_expression_list();
            vm_writer.write_call(type_name + "." + method_name, symbol_table.get_temp_arg_count());
        }
        else {
            out_file << "\t\t\t\t\t<symbol>)</symbol>" << std::endl;
            query_tokenizer();
        }
    }
    else if (cur_token == "(") {
        std::string method_name = prev_token;
        out_file << "\t\t\t\t\t<symbol>(</symbol>" << std::endl;
        query_tokenizer();

        if (cur_token != ")") {
            parse_expression_list();
            vm_writer.write_call(method_name, symbol_table.get_arg_count(method_name) + 1);
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
    // for recording arg counts of
    int arg_count = 0;
    parse_expression();
    arg_count++;

    while (cur_token == ",") {
        out_file << "\t\t\t\t\t<symbol>,</symbol>" << std::endl;
        query_tokenizer();

        parse_expression();
        arg_count++;
    }
    symbol_table.set_temp_arg_count(arg_count);
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

bool Parser::is_array() {
    // only to be called once we know Tokenizer::type() == "IDENTIFIER"
    if (tokenizer.get_current_token() != "[") {
        return false;
    }
    query_tokenizer();

    check_expected_x_after_y("]", "[");
    return true;
}

void Parser::expression_writer(const std::string& token) {

    if (tokenizer.op_exists(token)) {
        vm_writer.write_arithmetic(token);
    }
    else if ((symbol_table.is_declared(token) || tokenizer.type(token) == "INT_CONST") && !symbol_table.is_func(token)) {

        if (symbol_table.is_declared(token)) {
            if (symbol_table.get_kind(token) == "FIELD") {
                vm_writer.write_push("this", symbol_table.get_index(token));
            }
            else if (symbol_table.get_kind(token) == "STATIC") {
                vm_writer.write_push("static", symbol_table.get_index(token));
            }
            else if (symbol_table.get_kind(token) == "ARG") {
                if (cur_subroutine_type == "method") {
                    vm_writer.write_push("argument", symbol_table.get_index(token) + 1);
                }
                else {
                    vm_writer.write_push("argument", symbol_table.get_index(token));
                }
            }
            else if (symbol_table.get_kind(token) == "VAR") {
                vm_writer.write_push("local", symbol_table.get_index(token));
            }
        }
        else {
            vm_writer.write_push("constant", std::stoi(token));
        }
    }
}
