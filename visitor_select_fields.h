#include "ast.h"
#include "tables.h"

class select_fields : visitor
{
private:
	table *result_table;
	std::vector<table::field> header;
public:
	static table visit_static(ast ast_tree, table& result_table);	

	virtual void visit(from_clause* ast_node);
	virtual void visit(where_predicate* ast_node);
	virtual void visit(order_by_clause* ast_node);
	virtual void visit(group_by_clause* ast_node);

	virtual void visit(field_leaf* ast_node);
};
