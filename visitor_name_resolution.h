#include "visitor.h"
#include "ast.h"
#include "tables.h"

class name_resolution : public visitor
{
private:
	tables sql_tables;

	name_resolution(tables sql_tables)
	{
		this->sql_tables = sql_tables;
	}
public:
	static void visit_static(ast ast_tree, tables sql_tables)
	{
		name_resolution vis(sql_tables);
		vis.visitor::visit(ast_tree);
	}

	virtual void visit(field_leaf* ast_node);
	virtual void visit(expression_node_leaf* ast_node);

	virtual void resolve(select_predicate* select_predicate_node, from_predicate* from_predicate_node, tables sql_tables);
};
