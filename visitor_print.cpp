#include <stdio.h>
#include "visitor_print.h"
#include "ast.h"

void print_ast::visit_static(ast ast_tree)
{
	std::cout << "\nPrinting ast...";
	print_ast vis;
	vis.visitor::visit(ast_tree);
	std::cout << "\n";
}
void print_ast::visit(select_subject* ast_node)
{	
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast::visit(select_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast::visit(from_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast::visit(from_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast::visit(where_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast::visit(where_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast::visit(order_by_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast::visit(order_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast::visit(group_by_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast::visit(group_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}

void print_ast::visit(field_leaf* ast_node)
{
	ast_node->print();
}
void print_ast::visit(node_leaf* ast_node)
{
	if(ast_node != NULL)
		ast_node->print();
}
void print_ast::visit(expression_node_leaf* ast_node)
{
	ast_node->print();
}
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