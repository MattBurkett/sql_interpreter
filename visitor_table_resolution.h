#include "visitor.h"
#include "tables.h"

class table_resolution : public visitor
{
private:
	tables *sql_tables;
	bool parent_from_predicate;

	table_resolution(tables& sql_tables);
	std::string resolve(std::string column);

public:
	static void visit_static(ast ast_tree, tables& sql_tables);

	virtual void visit(from_predicate* ast_node);
	virtual void visit(node_leaf* ast_node);
};
