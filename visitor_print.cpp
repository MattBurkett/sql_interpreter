#include "visitor_print.h"
#include "ast.h"

void print_ast::visit(expression_node_branch* ast_node)
{
	auto ast_node_children = ast_node->get_children();

	if(ast_node->is_unary_exp()){
		ast_node->print();
		std::cout << "(";
		if(ast_node_children[0] != NULL) 
			ast_node_children[0]->accept(this);
		std::cout << ")";
	}
	else{
		std::cout << "(";
		if(ast_node_children[0] != NULL) 
			ast_node_children[0]->accept(this);
		std::cout << ") ";
		ast_node->print();
		std::cout << " (";
		if(ast_node_children[1] != NULL) 
			ast_node_children[1]->accept(this);
		std::cout << ")";
	}
}