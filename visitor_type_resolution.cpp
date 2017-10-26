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
	if( ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_type() == t_UNINIT ){
		for(auto column : sql_tables.get_query_table().get_columns())
			if( ast_node->get_token() == TOK_IDENTIFIER && ast_node->get_literal() == column.first ){
				ast_node->set_type(column.second);
				break;
			}
	}
	else if( ast_node->get_token() == TOK_INTEGER )
		ast_node->set_type(t_INT);
	else if( ast_node->get_token() == TOK_DECIMAL )
		ast_node->set_type(t_DOUBLE);
	else if( ast_node->get_token() == TOK_BOOL )
		ast_node->set_type(t_BOOL);
	else if( ast_node->get_token() == TOK_STRING )
		ast_node->set_type(t_CSTRING);
	
}

void type_resolution::visit(expression_node_leaf* ast_node)
{
	field_leaf* field_ast_node = ast_node;
	visit(field_ast_node);
	return;
}
