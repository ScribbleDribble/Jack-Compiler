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

class Tokenizer {

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

public:

    Tokenizer(std::ifstream& file_stream): file_stream(file_stream) {
        get_tokens();
    }


    static bool is_singleline_comment(char character, const std::string& token);
    static bool is_multiline_comment(char character, const std::string& token);
    static bool is_multiline_end(char character, const std::string& token);
    static bool is_string(char character, const std::string& meta_string, const std::string& token);
    std::streampos save_file_position();
    bool tokenize_symbol(char character, std::string& token, const std::string& meta_string );
    void get_tokens();
    void comment_test(bool single_flag, bool multi_flag);

    bool contains_more() {

    }

    void advance() {
        /**
         * advances onto the next token - only to be used when contains_more is true
         */
    }

    void type() {
    }

    void keyword_type() {
        /**
         * Returns keyword of the current token. Only called if type() == KEYWORD
         */
    }

    void symbol() {
        /**
         * returns symbol of current token. Only call if type() == SYMBOL
         */
    }

    void identifier() {
        /**
         * returns the identifier of the current token. Only call if type() == IDENTIFIER
         */
    }

    void int_value() {
        /**
         * returns integer value of the current token. Only call if type() == INT_CONST
         */
    }


};