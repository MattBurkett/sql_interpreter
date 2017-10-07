#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "tables.h"
#include "visitor_print.h"
#include "visitor_name_resolution.h"

int main(int argc, char* argv[])
{
	const char database_dir[] = "databases/";
	lexer sql_lexer;
	parser sql_parser;
	ast sql_ast;
	tables sql_tables(database_dir);
	int parse_number = 1;

	if(argc == 1)
		sql_lexer = lexer();
	else if(argc >= 2)
		sql_lexer = lexer(argv[1]);

	while(sql_lexer.load_statement()){
		try
		{
			auto tokens = sql_lexer.tokenize();
			sql_parser = parser(tokens);
			sql_ast = sql_parser.parse();
			
			print_ast::visit_static(sql_ast);
			name_resolution::visit_static(sql_ast, sql_tables);

			#ifdef DEBUG_MODE
			std::cout << parse_number++ << ". successful lex and parse\n";
			sql_ast.print(); std::cout << "\n";
			#endif
		}
		catch(const char* e)
		{
			std::cout << parse_number++ << ". " << std::string(e) << "\n";
		}
		catch(const std::string e)
		{
			std::cout << parse_number++ << ". " << e << "\n";
		}
	}


};