#include "visitor.h"
#include "ast.h"

void visitor::visit(ast ast_tree)
{
	ast_tree.get_root()->accept(this);
}

void visitor::visit(node_branch* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}
void visitor::visit(node_leaf* ast_node)
{}

void visitor::visit(node* ast_node)
{}

void visitor::visit(field_leaf* ast_node)
{}

void visitor::visit(expression_node_branch* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(expression_node_leaf* ast_node)
{}

void visitor::visit(select_subject* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(select_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(select_clause* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(from_subject* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(from_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(from_clause* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(where_subject* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(where_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(where_clause* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(order_by_subject* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(order_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(order_by_clause* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(group_by_subject* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(group_by_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(group_by_clause* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}

void visitor::visit(select_statement* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
}