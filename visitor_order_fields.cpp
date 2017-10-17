#include <stdio.h>
#include "visitor_order_fields.h"

void order_fields::visit_static(ast ast_tree, table& result_table)
{
	order_fields vis;
	vis.visitor::visit(ast_tree);
}
void order_fields::visit(field_leaf* ast_node)
{
	std::cout << ast_node->get_literal() << "." << ast_node->get_table() << "\n";
	return;
}

void order_fields::visit(select_clause* ast_node){}
void order_fields::visit(from_clause* ast_node){}
void order_fields::visit(where_clause* ast_node){}
void order_fields::visit(group_by_clause* ast_node){}
