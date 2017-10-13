#include <string>
#include <stdio.h>
#include "visitor_execute.h"

void execute::visit_static(ast ast_tree, tables sql_tables)
{
	execute vis;
	for ( auto row : sql_tables.get_query_table().get_rows()){
		vis.current_row = row;
		vis.visitor::visit(ast_tree);
	}
}

void execute::visit(where_predicate* ast_node)
{
	current_state = execute::WHERE;
	for(auto e : ast_node->get_children())
		e->accept(this);
	std::cout << (prev_node?"true\n":"false\n");
	current_state = execute::UNKNOWN;
}

void execute::visit(field_leaf* ast_node)
{
	// expression_node_leaf* temp_node = NULL;
	// temp_node->mark_temp();
	// switch(ast_node->get_type()){
	// case t_INT:
	// 	temp_node = new expression_node_leaf( std::make_pair(std::to_string(current_row[ast_node->get_table_x_index()].data.i), TOK_INTEGER) );
	// 	break;
	// }
	// children_leafs.push_back(temp_node);
}

void execute::visit(expression_node_leaf* ast_node)
{
	children_leafs.push_back(ast_node);
	if(ast_node->get_type() == t_BOOL)
		prev_node = *(bool*)ast_node->get_value();
}

void execute::visit(expression_node_branch* ast_node)
{
	std::vector<bool> children_boolean;
	//delete previous temporary nodes and clear children vector
	for( auto node : children_leafs )
		if(node->get_is_temp())
			delete node;
	children_leafs.clear();

	for(auto e : ast_node->get_children()){
		prev_node = false;
		e->accept(this);
		children_boolean.push_back(prev_node);
	}

	int arithmetic_value;
	expression_node_leaf* temp_node = NULL;
	if( ast_node->is_unary_exp() ){
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_NOT:
			prev_node = !children_boolean[0];
			break;
		case TOK_BIT_NOT:
			arithmetic_value = ~*(int*)children_leafs[0]->get_value();
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			break;
		}
	}
	else{
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_AND:
			prev_node = children_boolean[0] && children_boolean[1];
			break;
		case TOK_BOOL_OR:
			prev_node = children_boolean[0] || children_boolean[1];
			break;
		case TOK_BIT_AND:
			arithmetic_value = (*(int*)children_leafs[0]->get_value()) & (*(int*)children_leafs[1]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			break;
		case TOK_BIT_XOR:
			arithmetic_value = (*(int*)children_leafs[0]->get_value()) ^ (*(int*)children_leafs[1]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			break;
		case TOK_BIT_OR:
			arithmetic_value = (*(int*)children_leafs[0]->get_value()) | (*(int*)children_leafs[1]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			break;
		case TOK_MODULO:
			arithmetic_value = (*(int*)children_leafs[0]->get_value()) % (*(int*)children_leafs[1]->get_value());
			temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			break;
		
		case TOK_DIVIDE:
			if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_INT){
				arithmetic_value = (*(int*)children_leafs[0]->get_value()) / (*(int*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			}
			else{
				if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_INT)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) / (*(int*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(int*)children_leafs[0]->get_value()) / (*(double*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) / (*(double*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_DECIMAL) );
			}
			break;
		case TOK_ASTERICK:
			if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_INT){
				arithmetic_value = (*(int*)children_leafs[0]->get_value()) * (*(int*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			}
			else{
				if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_INT)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) * (*(int*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(int*)children_leafs[0]->get_value()) * (*(double*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) * (*(double*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_DECIMAL) );
			}
			break;
		case TOK_PLUS:
			if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_INT){
				arithmetic_value = (*(int*)children_leafs[0]->get_value()) + (*(int*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			}
			else{
				if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_INT)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) + (*(int*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(int*)children_leafs[0]->get_value()) + (*(double*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) + (*(double*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_DECIMAL) );
			}
			break;
		case TOK_MINUS:
			if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_INT){
				arithmetic_value = (*(int*)children_leafs[0]->get_value()) - (*(int*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_INTEGER) );
			}
			else{
				if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_INT)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) - (*(int*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_INT && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(int*)children_leafs[0]->get_value()) - (*(double*)children_leafs[1]->get_value());
				else if(children_leafs[0]->get_type() == t_DOUBLE && children_leafs[1]->get_type() == t_DOUBLE)
					arithmetic_value = (*(double*)children_leafs[0]->get_value()) - (*(double*)children_leafs[1]->get_value());
				temp_node = new expression_node_leaf( std::make_pair(std::to_string(arithmetic_value), TOK_DECIMAL) );
			}
			break;
		case TOK_EQ:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) == (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) == (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) == (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] == children_boolean[1];
			break;
		case TOK_NOTEQ:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) != (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) != (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) != (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] != children_boolean[1];
			break;
		case TOK_NOTGT:
		case TOK_LTEQ:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) <= (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) <= (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) <= (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] <= children_boolean[1];
			break;
		case TOK_NOTLT:
		case TOK_GTEQ:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) >= (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) >= (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) >= (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] >= children_boolean[1];
			break;
		case TOK_LT:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) < (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) < (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) < (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] < children_boolean[1];
			break;
		case TOK_GT:
			if(children_leafs[0]->get_type() == t_INT)
				prev_node = (*(int*)children_leafs[0]->get_value()) > (*(int*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_CSTRING)
				prev_node = (*(std::string*)children_leafs[0]->get_value()) > (*(std::string*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_DOUBLE)
				prev_node = (*(double*)children_leafs[0]->get_value()) > (*(double*)children_leafs[1]->get_value());
			else if(children_leafs[0]->get_type() == t_BOOL)
				prev_node = children_boolean[0] > children_boolean[1];
			break;
		}
	}
	if(temp_node != NULL){
		temp_node->mark_temp();
		children_leafs.push_back(temp_node);
	}

}