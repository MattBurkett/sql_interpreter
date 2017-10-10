#include "visitor.h"

class node_leaf;

class type_check : public visitor
{
private:
	Type prev_func_type;	// this is used in place of a return variable for leafs
				// 	to avoid re-implementing the super visitor's visit
public:
	static void visit_static(ast ast_tree);

	virtual void visit(expression_node_branch* ast_node);
	virtual void visit(field_leaf* ast_node);
	virtual void visit(expression_node_leaf* ast_node);


};
