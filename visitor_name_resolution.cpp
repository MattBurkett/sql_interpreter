#include "tables.h"
#include "visitor.h"
#include "ast.h"
#include "visitor_name_resolution.h"
#include "lexemes.h"

void name_resolution::visit(field_leaf* ast_node)
{
	std::vector<std::string> possible_tables;
	if(ast_node->get_table() == ""){
		std::cout << "Missing table name: " << ast_node->get_literal() << "\n";
		for(table t : sql_tables.get_tables())
			;
	}
}

void name_resolution::visit(expression_node_leaf* ast_node)
{
	std::vector<std::string> possible_tables;
	if(ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_table() == ""){
		std::cout << "Missing table name: " << ast_node->get_literal() << "\n";
		for(table t : sql_tables.get_tables())
			;
	}
}

void name_resolution::resolve(select_predicate* select_predicate_node, from_predicate* from_predicate_node, tables sql_tables)
{
	if(dynamic_cast<node_leaf *>(select_predicate_node->get_children().front())->get_token() != TOK_ASTERICK)
		return;
	auto tables_in_query = from_predicate_node->get_children();
	for(auto e : tables_in_query)
		;
}