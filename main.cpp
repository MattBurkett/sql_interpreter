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
#include "visitor_execute.h"
#include "visitor_select_fields.h"
#include "visitor_order_fields.h"

int main(int argc, char* argv[])
{
	const char database_dir[] = "databases/";
	int parse_number = 1;
	
	tables sql_tables(database_dir);
	lexer sql_lexer = lexer(argv[1]);

	while(sql_lexer.load_statement()){
		try{
			auto tokens = sql_lexer.tokenize();
			parser sql_parser = parser(tokens);
			ast sql_ast = sql_parser.parse();		

			/* Visitors */
			table_resolution::visit_static(sql_ast, sql_tables);
			astrick_resolution::visit_static(sql_ast, sql_tables);
			name_resolution::visit_static(sql_ast, sql_tables);
			type_resolution::visit_static(sql_ast, sql_tables);
			type_check::visit_static(sql_ast);
			table result_table = execute::visit_static(sql_ast, sql_tables);
			select_fields::visit_static(sql_ast, result_table);
			order_fields::visit_static(sql_ast, result_table);

			result_table.print();	
		}
		catch(const char* e){
			std::cout << parse_number << ". " << std::string(e) << "\n";
		}
		catch(const std::string e){
			std::cout << parse_number << ". " << e << "\n";
		}
		parse_number++;
	}


};