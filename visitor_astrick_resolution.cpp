#include "tables.h"
#include "visitor.h"
#include "ast.h"
#include "visitor_astrick_resolution.h"
#include "lexemes.h"
#include <string.h>

astrick_resolution::astrick_resolution(tables sql_tables)
{
	parent_select_predicate = false;
	child_astrick = false;
	this->sql_tables = sql_tables;
}

void astrick_resolution::visit_static(ast ast_tree, tables sql_tables)
{
	astrick_resolution vis(sql_tables);
	vis.visitor::visit(ast_tree);
}

void astrick_resolution::visit(select_predicate* ast_node)
{
	parent_select_predicate = true;
	for(auto e : ast_node->get_children())
		e->accept(this);
	if(child_astrick){
		ast_node->remove_child(0);
		for( auto column : sql_tables.get_query_table().get_columns() )
			ast_node->add_child( new field_leaf(std::pair<std::string, token_id>(column.first, TOK_IDENTIFIER), sql_tables.get_query_table().get_name_useable()) );
	}
	parent_select_predicate = false;
}

void astrick_resolution::visit(node_leaf* ast_node)
{
	if(parent_select_predicate && ast_node->get_token() == TOK_ASTERICK)
		child_astrick = true;
}
