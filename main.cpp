#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "tables.h"
#include "visitor_print.h"
#include "visitor_print_type.h"
#include "visitor_table_resolution.h"
#include "visitor_astrick_resolution.h"
#include "visitor_name_resolution.h"
#include "visitor_type_resolution.h"
#include "visitor_type_check.h"

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
		try{
			auto tokens = sql_lexer.tokenize();
			sql_parser = parser(tokens);
			sql_ast = sql_parser.parse();
			
			//print_ast::visit_static(sql_ast);
			print_ast_type::visit_static(sql_ast);			

			table_resolution::visit_static(sql_ast, sql_tables);
			astrick_resolution::visit_static(sql_ast, sql_tables);
			name_resolution::visit_static(sql_ast, sql_tables);
			type_resolution::visit_static(sql_ast, sql_tables);
			type_check::visit_static(sql_ast);
			//execute_query

			//print_ast::visit_static(sql_ast);
			print_ast_type::visit_static(sql_ast);	
		}
		catch(const char* e){
			std::cout << parse_number++ << ". " << std::string(e) << "\n";
		}
		catch(const std::string e){
			std::cout << parse_number++ << ". " << e << "\n";
		}
	}


};