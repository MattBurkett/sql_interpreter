#include "visitor.h"
#include "tables.h"

class execute : public visitor
{
private:
	bool prev_node;
	enum {UNKNOWN, SELECT, FROM, WHERE, ORDER, GROUP} current_state;
	std::vector<table::element> current_row;
	std::vector<expression_node_leaf*> children_leafs;
public:
	static void visit_static(ast ast_tree, tables sql_tables);	

	virtual void visit(select_clause* ast_node);
	virtual void visit(from_clause* ast_node);
	virtual void visit(where_predicate* ast_node);
	virtual void visit(order_by_clause* ast_node);
	virtual void visit(group_by_clause* ast_node);

	virtual void visit(field_leaf* ast_node);
	// virtual void visit(node_leaf* ast_node);
	virtual void visit(expression_node_leaf* ast_node);
	virtual void visit(expression_node_branch* ast_node);
};
