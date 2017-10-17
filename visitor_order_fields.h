#include "visitor.h"

class order_fields : public visitor
{
private:
	bool checking_if_sort;
	bool is_ascending;

	table* result_table;
public:
	order_fields();
	static void visit_static(ast ast_tree, table& result_table);	

	virtual void visit(select_clause* ast_node);
	virtual void visit(from_clause* ast_node);
	virtual void visit(where_clause* ast_node);
	virtual void visit(group_by_clause* ast_node);
	virtual void visit(order_by_predicate* ast_node);

	virtual void visit(field_leaf* ast_node);
	virtual void visit(node_leaf* ast_node);
};
