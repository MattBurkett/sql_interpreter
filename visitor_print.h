#include "visitor.h"
#include "ast.h"

class print_ast : public visitor
{
public:
	static void visit_static(ast ast_tree)
	{
		print_ast vis;
		vis.visitor::visit(ast_tree);
		std::cout << "\n";
	}

	virtual void visit(field_leaf* ast_node)
	{
		ast_node->print();
	}
	virtual void visit(node_leaf* ast_node)
	{
		if(ast_node != NULL)
			ast_node->print();
	}
	virtual void visit(expression_node_leaf* ast_node)
	{
		ast_node->print();
	}
	virtual void visit(expression_node_branch* ast_node);
};
