#include "tokenizer.h"

void Tokenizer::get_tokens() {
    std::string token;
    // holds strings within text
    std::string meta_string;
    char character;
    char comment_data[256];
    int space_ascii = 32;

    while (file_stream.get(character)) {

        if (character == '\n' || character == '\t')
            continue;

       if (symbol_map.count(character) > 0) {
            if (tokenize_symbol(character, token, meta_string)) {
                continue;
            }
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

            else if (static_cast<int>(character) == space_ascii) {
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
            token_queue.push("0"+meta_string);
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
    return token[0] == '/' && (character == '/' || token[1] == '/');
}

bool Tokenizer::is_multiline_comment(char character, const std::string& token) {
    return (token.size() >= 2 && (token[0] == '/' && token[1] == '*')) || token[0] == '/' && character == '*';
}

bool Tokenizer::tokenize_symbol(char character, std::string& token, const std::string& meta_string) {
    char next_char;
    std::streampos cur_pos = save_file_position();
    file_stream.get(next_char);

    if (character == '/' && next_char == '/' || character == '/' && next_char == '*') {
        file_stream.seekg(cur_pos);
        return false;
    }
    file_stream.seekg(cur_pos);

    if (!is_multiline_comment(character, token) &&
            !is_string(character, meta_string, token) && !is_singleline_comment(character, token))  {

        if (!token.empty()){
            token_queue.push(token);
            token = "";
        }
        token_queue.push(std::string() += character);
        return true;
    }
    return false;
}

std::streampos Tokenizer::save_file_position() {
    return file_stream.tellg();
}

bool Tokenizer::is_multiline_end(char character, const std::string& token) {
    return token[token.size() - 1] == '*' && character == '/';
}
bool Tokenizer::is_string(char character, const std::string& meta_string, const std::string& token) {
    return token.empty() && ((meta_string.empty() && character == '"') || !meta_string.empty());
}

std::string Tokenizer::type(const std::string& token) {
    if (symbol_map.count(token[0]) > 0) {
        return "SYMBOL";
    }

    else if (keyword_map.count(token) > 0) {
        return "KEYWORD";
    }

    else if (token[0] == '_') {
        return "STRING_CONST";
    }

    try {
        int val = std::stoi(token);
        return "INT_CONST";
    }
    catch (std::exception& e) {
        return "IDENTIFIER";
    }
}

std::string Tokenizer::keyword_type(const std::string& token) {
    /**
     * Returns keyword of the current token. Only called if type() == KEYWORD
     */
    std::string keyword;
    for(char letter : token) {
        int ascii_val = static_cast<int>(letter) - 32;
        char character = static_cast<char>(ascii_val);
        keyword += character;
    }

    return keyword;
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

std::string Tokenizer::string_val(const std::string &token) {

}

bool Tokenizer::contains_more() {
    return !token_queue.empty();
}

std::string Tokenizer::advance() {
    cur_token = token_queue.front();
    token_queue.pop();
    return cur_token;
}

std::string Tokenizer::get_current_token() {
    return cur_token;
}

int Tokenizer::get_keyword_val(const std::string& token) {
    return keyword_map[token];
}



