#include "tokenizer.h"

void Tokenizer::get_tokens() {
    std::string token;

    // holds strings within text
    std::string meta_string;
    char character;
    char comment_data[256];
    int space_ascii = 32;

    while (file_stream.get(character)) {

        if (character == '\n')
            continue;

        else if (tokenize_symbol(character, token, meta_string)) {
            continue;
        }
        if (!is_string(character, meta_string, token)) {
            if (is_singleline_comment(character, token)) {
                file_stream.getline(comment_data, 256);
                token = "";
                continue;
            }
            else if (is_multiline_comment(character, token)) {
                if (!is_multiline_end(character, token)) {
                    token += character;
                    continue;
                }
                token = "";
            }
            else if ((int)character == space_ascii) {
                if (!token.empty()) {
                    token_queue.push(token);
                }
                token = "";
            }
            else {
                token += character;
            }
        }
        else if ((!meta_string.empty() && character == '"')) {
            token_queue.push(meta_string);
            meta_string = "";
        }
        else {
            if (character == '"')
            {
                // dont store quotation marks in string but include the next character
                file_stream.get(character);
                meta_string += character;
            }
            else {
                meta_string += character;
            }
        }
    }

    file_stream.close();
}

bool Tokenizer::is_singleline_comment(char character, const std::string& token) {
    return token[0] == '/' && character == '/';
}

bool Tokenizer::is_multiline_comment(char character, const std::string& token) {
    if (token.size() >= 2)
        return token[0] == '/' && token[1] == '*';

    return false;
}

bool Tokenizer::tokenize_symbol(char character, std::string& token, const std::string& meta_string) {

    if (symbol_map.count(character) > 0 && !is_multiline_comment(character, token) &&
            !is_string(character, meta_string, token)) {

        if (!token.empty()){
            token_queue.push(token);
            token = "";
        }
        token_queue.push(std::string() += character);
        return true;
    }
    return false;
}

bool Tokenizer::is_multiline_end(char character, const std::string& token) {
    return token[token.size() - 1] == '*' && character == '/';
}
bool Tokenizer::is_string(char character, const std::string& meta_string, const std::string& token) {
    return token.empty() && ((meta_string.empty() && character == '"') || !meta_string.empty());
}

