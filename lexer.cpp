#include "lexer.h"
#include "lexemes.h"
#include <string.h>
#include <fstream>
#include <iostream>

lexer::lexer(char* file_name)
{
	if(file_input = (file_name != NULL))
		statement_file_name = std::string(file_name);
	else
		statement_file_name = std::string("");
	
}

lexer::~lexer()
{
	
}

bool lexer::load_statement()
{
	if(file_input){
		static unsigned long position = 0;
		std::fstream statement;
		statement.open(statement_file_name, std::fstream::in);
		statement.seekg(position);
	
		int i;
		for(i = 0; statement.peek() != ';'; i++){
			if(!statement.good())
				return false;
			statement_buffer[i] = statement.get();
			position++;
		}
		statement_buffer[i++] = ';';
		statement_buffer[i++] = '\n';
		statement_buffer[i++] = '\0';
		position += 3;
		statement_top = &statement_buffer[0];
		strip_white_space(statement_top);
		return true;
	}
	else{
		int offset = 0;
		while(true){
			std::cout << "> ";
			fgets(statement_buffer + offset, STATEMENT_BUFFER_SIZE - offset, stdin);
			offset = strlen(statement_buffer);
			if(strstr(statement_buffer, ";"))
				break;
		}
		statement_buffer[offset] = '\0';
		statement_top = &statement_buffer[0];
		strip_white_space(statement_top);
		return true;
	}
}

void lexer::strip_white_space(char *&ptr)
{
	while(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
		ptr++;
}

static unsigned char* generate_accept_array(const char *&ptr)
{
	static unsigned char accept_array[256];
	int inital_state;
	ptr++;

	if(ptr[0] == '^'){
		inital_state = 1;
		ptr++;
	}
	else{
		inital_state = 0;
	}

	for(int i = 0; i < 256; i++)
		accept_array[i] = inital_state;

	while(*ptr != ']' && *ptr != '\0'){
		if(ptr[1] == '-'){
			for(int i = ptr[0]; i <= ptr[2]; i++)
				accept_array[i] ^= 1;
			ptr += 3;
		}
		else{
			accept_array[ptr[0]] ^= 1;
			ptr += 1;
		}
	}
	if(*ptr == ']')
		ptr++;

	return accept_array;
}

static std::vector<std::pair<std::string, token_id>> tokens;

std::vector<std::pair<std::string, token_id>> lexer::tokenize()
{
	tokens.erase(tokens.begin(), tokens.end());

	while(*statement_top != '\0'){
		if(match() == false){
			std::cout << "invalid lex: " + std::string(statement_top, 8) + "...\n";
			exit(-1);
		}
		strip_white_space(statement_top);
	}
	return tokens;
}

static std::pair<std::string, token_id> token;
bool lexer::match()
{
	for(auto lexeme : lexemes){
		if(match_lexeme(lexeme) == true){
			tokens.push_back(token);
			return true;
		}
	}

	return false;
}

bool lexer::match_lexeme(std::pair <const char*, const token_id> lexeme)
{
	const char *lexeme_ptr = lexeme.first;
	char* statement_top_ptr = statement_top;
	char  buffer[2];
	const unsigned char *accept_array = NULL;

	while(*lexeme_ptr != '\0' && *statement_top_ptr != '\0'){
		switch(*lexeme_ptr){
		case '\\':
			lexeme_ptr++;
		default:
			if(*lexeme_ptr != *statement_top_ptr)
				return false;
			lexeme_ptr++;
			statement_top_ptr++;
			continue;
		case '*':
		case '+':
			if(lexeme_ptr[-1] == ']')
				while(accept_array[*(statement_top_ptr++)]);
			else
				while(lexeme_ptr[-1] == *(statement_top_ptr++));
			statement_top_ptr--;
			lexeme_ptr++;
			continue;

		case '[':
			accept_array = generate_accept_array(lexeme_ptr);
			
			if(!accept_array[*statement_top_ptr])
				if(*lexeme_ptr != '*')
					return false;
				else
					continue;
			statement_top_ptr++;
			continue;
		case '.':
			lexeme_ptr++;
			statement_top_ptr++;
			continue;
		}
	}

	token.first = std::string(statement_top, --statement_top_ptr);
	token.second = lexeme.second;
	statement_top = statement_top_ptr;
	return true;
}