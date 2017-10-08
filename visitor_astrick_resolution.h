#include "visitor.h"
#include "tables.h"

class astrick_resolution : public visitor
{
private:
	tables sql_tables;
	bool parent_select_predicate;
	bool child_astrick;

	astrick_resolution(tables sql_tables);
	std::string resolve(std::string column);

public:
	static void visit_static(ast ast_tree, tables sql_tables);

	virtual void visit(node_leaf* ast_node);
	virtual void visit(select_predicate* ast_node);
};
