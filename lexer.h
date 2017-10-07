#pragma once

#include <string>
#include <vector>
#include "lexemes.h"
#define STATEMENT_BUFFER_SIZE 1024


class lexer{
private:
	std::string 	statement_file_name;
	char 		statement_buffer[STATEMENT_BUFFER_SIZE];
	char	       *statement_top;
	bool 		file_input;

	bool match();
	bool match_lexeme(std::pair <const char*, const token_id> lexeme);
	void strip_white_space(char *&ptr);
public:
	lexer();
	lexer(std::string file_name);
	~lexer();

	std::vector<std::pair<std::string, token_id>> tokenize();
	bool load_statement();

};