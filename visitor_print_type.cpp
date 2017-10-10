#include <stdio.h>
#include "visitor_print_type.h"
#include "ast.h"

void print_ast_type::visit_static(ast ast_tree)
{
	std::cout << "\nPrinting ast with types...";
	print_ast_type vis;
	vis.visitor::visit(ast_tree);
	std::cout << "\n";
}
void print_ast_type::visit(select_subject* ast_node)
{	
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast_type::visit(select_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast_type::visit(from_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast_type::visit(from_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast_type::visit(where_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast_type::visit(where_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast_type::visit(order_by_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast_type::visit(order_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}
void print_ast_type::visit(group_by_subject* ast_node)
{
	std::cout << "\n";
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void print_ast_type::visit(group_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children()){
		std::cout << "\n\t";
		e->accept(this);
	}
}

void print_ast_type::visit(field_leaf* ast_node)
{
	ast_node->print();
	switch(ast_node->get_type()){
		case t_UNINIT:
		std::cout << "(Un-initalized)";
		break;
		case t_INT:
		std::cout << "(Integer)";
		break;
		case t_CSTRING:
		std::cout << "(String)";
		break;
		case t_DOUBLE:
		std::cout << "(Double)";
		break;
		case t_BOOL:
		std::cout << "(Boolean)";
		break;
	}
}
void print_ast_type::visit(node_leaf* ast_node)
{
	if(ast_node != NULL)
		ast_node->print();
}
void print_ast_type::visit(expression_node_leaf* ast_node)
{
	ast_node->print();
	switch(ast_node->get_type()){
		case t_UNINIT:
		std::cout << "(Un-initalized)";
		break;
		case t_INT:
		std::cout << "(Integer)";
		break;
		case t_CSTRING:
		std::cout << "(String)";
		break;
		case t_DOUBLE:
		std::cout << "(Double)";
		break;
		case t_BOOL:
		std::cout << "(Boolean)";
		break;
	}
}
void print_ast_type::visit(expression_node_branch* ast_node)
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