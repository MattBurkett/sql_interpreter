#include <stdio.h>
#include "visitor_order_fields.h"

order_fields::order_fields()
{
	checking_if_sort = false;
	is_ascending = true;
}
void order_fields::visit_static(ast ast_tree, table& result_table)
{
	order_fields vis;
	vis.result_table = &result_table;
	vis.visitor::visit(ast_tree);
}
void order_fields::visit(field_leaf* ast_node)
{
	if(checking_if_sort){
		is_ascending = true;
		return;
	}
	// std::cout << ast_node->get_literal() << "." << ast_node->get_table() << "\n";
	// std::cout << "sort " << ast_node->get_literal() << (is_ascending ? std::string(" in Ascending order\n") : std::string(" in Descending order\n"));
	
	this->result_table->sort(ast_node->get_literal(), is_ascending);
	return;
}
void order_fields::visit(node_leaf* ast_node)
{
	if(checking_if_sort){
		if(ast_node->get_token() == TOK_DESC)
			is_ascending = false;
		else
			is_ascending = true;
		return;
	}
}
void order_fields::visit(order_by_predicate* ast_node)
{
	auto children = ast_node->get_children();
	for(auto ittr = children.end() - 1; ittr >= children.begin(); ittr--){
		if(ittr+1 != children.end()){
			checking_if_sort = true;
			ittr[1]->accept(this);
			checking_if_sort = false;
		}
		(*ittr)->accept(this);
	}
}

void order_fields::visit(select_clause* ast_node){}
void order_fields::visit(from_clause* ast_node){}
void order_fields::visit(where_clause* ast_node){}
void order_fields::visit(group_by_clause* ast_node){}
