#include "visitor.h"
#include "tables.h"

class type_resolution : public visitor
{
private:
	tables sql_tables;

	type_resolution(tables sql_tables);

public:
	static void visit_static(ast ast_tree, tables sql_tables);

	virtual void visit(field_leaf* ast_node);
	virtual void visit(expression_node_leaf* ast_node);
};
