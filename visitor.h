#pragma once

#include <typeinfo>
#include "ast.h"
#include "tables.h"
#include "lexemes.h"

class node_leaf;

class visitor
{
private:
public:
	virtual void visit(ast ast_tree);
	virtual void visit(node_branch* ast_node);
	virtual void visit(node_leaf* ast_node);
	virtual void visit(node* ast_node);
	virtual void visit(field_leaf* ast_node);
	virtual void visit(expression_node_branch* ast_node);
	virtual void visit(expression_node_leaf* ast_node);
	virtual void visit(select_subject* ast_node);
	virtual void visit(select_predicate* ast_node);
	virtual void visit(select_clause* ast_node);
	virtual void visit(from_subject* ast_node);
	virtual void visit(from_predicate* ast_node);
	virtual void visit(from_clause* ast_node);
	virtual void visit(where_subject* ast_node);
	virtual void visit(where_predicate* ast_node);
	virtual void visit(where_clause* ast_node);
	virtual void visit(order_by_subject* ast_node);
	virtual void visit(order_by_predicate* ast_node);
	virtual void visit(order_by_clause* ast_node);
	virtual void visit(group_by_subject* ast_node);
	virtual void visit(group_by_predicate* ast_node);
	virtual void visit(group_by_clause* ast_node);
	virtual void visit(select_statement* ast_node);
};
