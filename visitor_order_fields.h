#include "visitor.h"

class order_fields : public visitor
{
public:
	static void visit_static(ast ast_tree, table& result_table);	

	virtual void visit(select_clause* ast_node);
	virtual void visit(from_clause* ast_node);
	virtual void visit(where_clause* ast_node);
	virtual void visit(group_by_clause* ast_node);

	virtual void visit(field_leaf* ast_node);
};
