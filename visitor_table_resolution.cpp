#include "tables.h"
#include "visitor.h"
#include "ast.h"
#include "visitor_table_resolution.h"
#include <string.h>

table_resolution::table_resolution(tables& sql_tables)
{
	this->sql_tables = &sql_tables;
	parent_from_predicate = false;
}

void table_resolution::visit_static(ast ast_tree, tables& sql_tables)
{
	table_resolution vis(sql_tables);
	vis.visitor::visit(ast_tree);
}

void table_resolution::visit(from_predicate* ast_node)
{
	parent_from_predicate = true;
	for(auto e : ast_node->get_children())
		e->accept(this);
	parent_from_predicate = false;
	
}

void table_resolution::visit(node_leaf* ast_node)
{
	if(parent_from_predicate)
		if( !sql_tables->set_query_table(ast_node->get_literal()) ){
			std::cout << "\tUnresolved table name: " << ast_node->get_literal() << "\n";
			throw("Unresolved table");
		}

}