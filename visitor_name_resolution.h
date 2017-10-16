#include "visitor.h"
#include "tables.h"

class name_resolution : public visitor
{
private:
	tables sql_tables;

	name_resolution(tables sql_tables);
	void resolve(field_leaf* ast_node);

public:
	static void visit_static(ast ast_tree, tables sql_tables);

	virtual void visit(field_leaf* ast_node);
	virtual void visit(expression_node_leaf* ast_node);
};
