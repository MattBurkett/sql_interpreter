#include "visitor.h"

class node_leaf;

class type_check : public visitor
{
private:
public:
	static void visit_static(ast ast_tree);
	virtual void visit(ast ast_tree);

	virtual void visit(expression_node_branch* ast_node);
	virtual void visit(select_predicate* ast_node);
	virtual void visit(from_predicate* ast_node);
	virtual void visit(where_predicate* ast_node);
	virtual void visit(order_by_predicate* ast_node);
	virtual void visit(group_by_predicate* ast_node);
};
