#include "tables.h"
#include "visitor.h"
#include "ast.h"
#include "visitor_name_resolution.h"
#include "lexemes.h"
#include <string.h>

name_resolution::name_resolution(tables sql_tables)
{
	this->sql_tables = sql_tables;
}

void name_resolution::visit_static(ast ast_tree, tables sql_tables)
{
	name_resolution vis(sql_tables);
	vis.visitor::visit(ast_tree);
}

void name_resolution::visit(field_leaf* ast_node)
{
	std::vector<std::string> possible_tables;
	if(ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_table() == "")
		resolve(ast_node);
}

void name_resolution::visit(expression_node_leaf* ast_node)
{
	std::vector<std::string> possible_tables;
	if(ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_table() == "")
		resolve(ast_node);

}

void name_resolution::resolve(field_leaf* ast_node)
{
	std::string unknown_column = ast_node->get_literal();
	std::string matching_table = "";
	table query_table = sql_tables.get_query_table();

	for(auto column : query_table.get_columns())
		if(unknown_column == column.first)
			if(matching_table == "")
				matching_table = query_table.get_name_useable();
			else{
				std::cout << "\tAmbiguous column: " << unknown_column 
					<< "\n\tFirst two conflicts..."
					<< "\n\t\tTable 1: " << matching_table
					<< "\n\t\tTable 2: " << query_table.get_name() << "\n";
				throw("Ambiguous column");
			}

	if(matching_table == ""){
		std::cout << "\tUnresolved column: " << unknown_column << "\n";
		throw("Unresolved column");
	}

	ast_node->add_table(matching_table);

}