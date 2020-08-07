//
// Created by mke on 31/07/2020.
//

#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#endif //COMPILER_TOKENIZER_H

#include "iostream"
#include <fstream>
#include "queue"
#include "map"
#include "string"
#include <memory>

class Tokenizer{

private:
    std::ifstream& file_stream;
    std::queue<std::string> token_queue;
    std::map<char, int> symbol_map =  {
            {'{', 1}, {'}', 2}, {'(', 3},
            {')', 4}, {'[', 5}, {']', 6},
            {',', 7}, {'=', 8}, {';', 9},
            {'<',10}, {'>', 11}, {'+', 12},
            {'-', 13}, {'%', 14}, {'/', 15},
            {'.', 16}, {'*', 17}, {'|', 18},
    };

    std::map<std::string, int> keyword_map = {
            {"class", 1}, {"constructor", 2}, {"method", 3},
            {"function", 3}, {"int", 4}, {"boolean", 5},
            {"void", 6}, {"static", 7}, {"field", 8}, {"let", 9},
            {"if", 10}, {"else", 11}, {"while", 12},
            {"return", 13}, {"true", 19}, {"false", 15},
            {"char", 16}, {"this", 17}, {"null", 18},
            {"do", 14}
    };

    std::map<std::string, int> type_map = { {"int", 1}, {"boolean", 1}, {"char", 1} };
    std::map<std::string, int> op_map = { {"+", 1}, {"-", 1}, {"*", 1}, {"/", 1},
            {"|", 1}, {"&", 1}, {"<", 1}, {">", 1}, {"=", 1}
    };

    static bool is_singleline_comment(char character, const std::string& token);
    static bool is_multiline_comment(char character, const std::string& token);
    static bool is_multiline_end(char character, const std::string& token);
    static bool is_string(char character, const std::string& meta_string, const std::string& token);
    std::streampos save_file_position();
    bool tokenize_symbol(char character, std::string& token, const std::string& meta_string );
    void get_tokens();

    std::string cur_token;


public:

    Tokenizer(std::ifstream& file_stream): file_stream(file_stream) {
        get_tokens();
    }

    enum class SyntacticType {SYMBOL, KEYWORD, STRING_CONST, INT_CONST, IDENTIFIER};

    bool contains_more();

    std::string advance();
    std::string get_current_token();
    std::string type(const std::string& token);
    static std::string keyword_type(const std::string& token);

    static void symbol() {
        /**
         * returns symbol of current token. Only call if type() == SYMBOL
         */
    }

    static void identifier() {
        /**
         * returns the identifier of the current token. Only call if type() == IDENTIFIER
         */
    }

    static void int_value() {
        /**
         * returns integer value of the current token. Only call if type() == INT_CONST
         */
    }

    // '_' prefix is attached to every string in program text to separate it from identifiers
    std::string string_val(const std::string& token);

    bool type_exists(const std::string& type_token) {
        return type_map.count(type_token) > 0;
    }

    void add_type(const std::string& user_def_type) {
        // adds user defined types as a recognizable type
        type_map[user_def_type] = 1;
    }

    int get_keyword_val(const std::string& token);

    bool op_exists(const std::string& token) {
        return op_map.count(token) > 0;
    }
};