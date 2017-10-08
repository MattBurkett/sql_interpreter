#include "tables.h"
#include "visitor.h"
#include "ast.h"
#include "visitor_type_resolution.h"
#include "lexemes.h"
#include <string.h>

type_resolution::type_resolution(tables sql_tables)
{
	this->sql_tables = sql_tables;
}

void type_resolution::visit_static(ast ast_tree, tables sql_tables)
{
	type_resolution vis(sql_tables);
	vis.visitor::visit(ast_tree);
}

void type_resolution::visit(field_leaf* ast_node)
{
	for(auto column : sql_tables.get_query_table().get_columns())
		if( ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_literal() == column.first ){
			ast_node->set_type(column.second);
			break;
		}
}

void type_resolution::visit(expression_node_leaf* ast_node)
{
	for(auto column : sql_tables.get_query_table().get_columns())
		if( ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_literal() == column.first ){
			ast_node->set_type(column.second);
			break;
		}
}
