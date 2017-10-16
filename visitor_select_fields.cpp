#include <algorithm>
#include "ast.h"
#include "visitor.h"
#include "visitor_select_fields.h"
#include "tables.h"

table select_fields::visit_static(ast ast_tree, table& result_table)
{
	select_fields vis;
	vis.result_table = &result_table;
	vis.visitor::visit(ast_tree);
	vis.result_table->set_columns(vis.header);
	
	return *(vis.result_table);
}

void select_fields::visit(from_clause* ast_node){}
void select_fields::visit(where_predicate* ast_node){}
void select_fields::visit(order_by_clause* ast_node){}
void select_fields::visit(group_by_clause* ast_node){}

void select_fields::visit(field_leaf* ast_node)
{
	table::field column;
	column.type = ast_node->get_type();
	column.name = ast_node->get_literal();
	header.push_back(column);
}
