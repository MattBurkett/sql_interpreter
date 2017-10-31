#include <string>
#include <stdio.h>

#include "ast.h"
#include "tables.h"
#include "visitor_execute.h"

table execute::visit_static(ast ast_tree, tables sql_tables)
{
	execute vis;
	for ( auto row : sql_tables.get_query_table().get_rows()){
		vis.current_row = row;
		vis.children_leafs.clear();
		vis.visitor::visit(ast_tree);
	}
	return table(vis.query_rows, sql_tables.get_query_table().get_header());
}

void execute::visit(select_clause* ast_node) {}
void execute::visit(from_clause* ast_node) {}
void execute::visit(order_by_clause* ast_node) {}
void execute::visit(group_by_clause* ast_node) {}

void execute::visit(where_predicate* ast_node)
{
	for(auto e : ast_node->get_children())
		e->accept(this);
	if(*(bool*)children_leafs.back()->get_value() == true)
		query_rows.push_back(current_row);
	//std::cout << (prev_node?"true\n":"false\n");
}

void execute::visit(field_leaf* ast_node)
{
	expression_node_leaf* temp_node = NULL;

	switch(ast_node->get_type()){
	case t_INT:
		temp_node = new expression_node_leaf( std::make_pair(std::to_string(current_row[ast_node->get_table_x_index()].data.i), TOK_INTEGER) );
		break;
	case t_CSTRING:
		temp_node = new expression_node_leaf( std::make_pair(std::string("'") + current_row[ast_node->get_table_x_index()].data.s + "'", TOK_STRING) );
		break;
	case t_BOOL:
		temp_node = new expression_node_leaf( std::make_pair(current_row[ast_node->get_table_x_index()].data.b ? std::string("true") : std::string("false") , TOK_BOOL) );
		temp_node->set_type(t_BOOL);
		break;
	case t_DOUBLE:
		temp_node = new expression_node_leaf( std::make_pair(std::to_string(current_row[ast_node->get_table_x_index()].data.d), TOK_DECIMAL) );
		break;
	}
	temp_node->mark_temp();
	temp_node->set_type(ast_node->get_type());
	children_leafs.push_back(temp_node);
}

void execute::visit(expression_node_leaf* ast_node)
{
	if(ast_node->get_token() == TOK_IDENTIFIER)
		visit((field_leaf*)ast_node);
	else {
		children_leafs.push_back(ast_node);
	}
}

void execute::visit(expression_node_branch* ast_node)
{
	//delete previous temporary nodes and clear children vector
	for(auto e : ast_node->get_children())
		e->accept(this);

	int arithmetic_int;
	double arithmetic_double;

	bool boolean_value;
	expression_node_leaf* temp_node = NULL;
	auto ritter = children_leafs.rbegin();

	if( ast_node->is_unary_exp() ){
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_NOT:
			temp_node = new expression_node_leaf( std::make_pair( (*(bool*)ritter[0]->get_value())?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_BIT_NOT:
			arithmetic_int = ~*(int*)ritter[0]->get_value();
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
			break;
		}
		children_leafs.pop_back();
	}
	else{
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_AND:
			temp_node = new expression_node_leaf( std::make_pair( (*(bool*)ritter[1]->get_value() && *(bool*)ritter[0]->get_value())?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_BOOL_OR:
			temp_node = new expression_node_leaf( std::make_pair( (*(bool*)ritter[1]->get_value() || *(bool*)ritter[0]->get_value())?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_BIT_AND:
			arithmetic_int = (*(int*)ritter[1]->get_value()) & (*(int*)ritter[0]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
			temp_node->set_type(t_INT);
			break;
		case TOK_BIT_XOR:
			arithmetic_int = (*(int*)ritter[1]->get_value()) ^ (*(int*)ritter[0]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
			temp_node->set_type(t_INT);
			break;
		case TOK_BIT_OR:
			arithmetic_int = (*(int*)ritter[1]->get_value()) | (*(int*)ritter[0]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
			temp_node->set_type(t_INT);
			break;
		case TOK_MODULO:
			arithmetic_int = (*(int*)ritter[1]->get_value()) % (*(int*)ritter[0]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
			temp_node->set_type(t_INT);
			break;
		
		case TOK_DIVIDE:
			if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_INT){
				arithmetic_int = (*(int*)ritter[1]->get_value()) / (*(int*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
				temp_node->set_type(t_INT);
			}
			else{
				if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_INT)
					arithmetic_double = (*(double*)ritter[1]->get_value()) / (*(int*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(int*)ritter[1]->get_value()) / (*(double*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(double*)ritter[1]->get_value()) / (*(double*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_double), TOK_DECIMAL) );
				temp_node->set_type(t_DOUBLE);
			}
			break;
		case TOK_ASTERICK:
			if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_INT){
				arithmetic_int = (*(int*)ritter[1]->get_value()) * (*(int*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
				temp_node->set_type(t_INT);
			}
			else{
				if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_INT)
					arithmetic_double = (*(double*)ritter[1]->get_value()) * (*(int*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(int*)ritter[1]->get_value()) * (*(double*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(double*)ritter[1]->get_value()) * (*(double*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_double), TOK_DECIMAL) );
				temp_node->set_type(t_DOUBLE);
			}
			break;
		case TOK_PLUS:
			if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_INT){
				arithmetic_int = (*(int*)ritter[1]->get_value()) + (*(int*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
				temp_node->set_type(t_INT);
			}
			else{
				if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_INT)
					arithmetic_double = (*(double*)ritter[1]->get_value()) + (*(int*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(int*)ritter[1]->get_value()) + (*(double*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(double*)ritter[1]->get_value()) + (*(double*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_double), TOK_DECIMAL) );
				temp_node->set_type(t_DOUBLE);
			}
			break;
		case TOK_MINUS:
			if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_INT){
				arithmetic_int = (*(int*)ritter[1]->get_value()) - (*(int*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_int), TOK_INTEGER) );
				temp_node->set_type(t_INT);
			}
			else{
				if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_INT)
					arithmetic_double = (*(double*)ritter[1]->get_value()) - (*(int*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_INT && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(int*)ritter[1]->get_value()) - (*(double*)ritter[0]->get_value());
				else if(ritter[1]->get_type() == t_DOUBLE && ritter[0]->get_type() == t_DOUBLE)
					arithmetic_double = (*(double*)ritter[1]->get_value()) - (*(double*)ritter[0]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_double), TOK_DECIMAL) );
				temp_node->set_type(t_DOUBLE);
			}
			break;
		case TOK_EQ:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) == (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) == (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) == (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() == *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_NOTEQ:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) != (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) != (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) != (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() != *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_NOTGT:
		case TOK_LTEQ:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) <= (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) <= (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) <= (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() <= *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_NOTLT:
		case TOK_GTEQ:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) >= (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) >= (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) >= (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() >= *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_LT:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) < (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) < (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) < (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() < *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		case TOK_GT:
			if(ritter[0]->get_type() == t_INT)
				boolean_value = (*(int*)ritter[1]->get_value()) > (*(int*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_CSTRING)
				boolean_value = (*(std::string*)ritter[1]->get_value()) > (*(std::string*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_DOUBLE)
				boolean_value = (*(double*)ritter[1]->get_value()) > (*(double*)ritter[0]->get_value());
			else if(ritter[0]->get_type() == t_BOOL)
				boolean_value = *(bool*)ritter[1]->get_value() > *(bool*)ritter[0]->get_value();
			else
				break;
			temp_node = new expression_node_leaf( std::make_pair( (boolean_value)?(std::string("true")):(std::string("false")), TOK_BOOL ) );
			temp_node->set_type(t_BOOL);
			break;
		}
		children_leafs.pop_back();
		children_leafs.pop_back();
	}

	if(temp_node != NULL){
		temp_node->mark_temp();
		children_leafs.push_back(temp_node);
	}

}