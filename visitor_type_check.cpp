#include <stdio.h>
#include <string>
#include "visitor_type_check.h"
#include "ast.h"

std::string type_string(Type type)
{
	switch(type){
	case t_UNINIT:
		return "(Un-initalized)";
	case t_INT:
		return "(Integer)";
	case t_CSTRING:
		return "(String)";
	case t_DOUBLE:
		return "(Double)";
	case t_BOOL:
		return "(Boolean)";
	}
	return "";
}
void type_check::visit_static(ast ast_tree)
{
	type_check vis;
	vis.prev_func_type = t_UNINIT;
	vis.visitor::visit(ast_tree);
}
void type_check::visit(expression_node_branch* ast_node)
{
	auto children = ast_node->get_children();
	std::vector<Type> children_types;
	for(auto c : children){
		c->accept(this);
		children_types.push_back(prev_func_type);
		prev_func_type = t_UNINIT;
	}

	if( ast_node->is_unary_exp() ){
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_NOT:
			if (children_types[0] != t_BOOL){
				std::cout << "\tType error: boolean not\n"
					<< "\t\tShould be of type BOOL, but child is " << type_string(children_types[0]) << "\n";
				throw("Type error: boolean not");
			}
			prev_func_type = t_BOOL;
			break;
		case TOK_BIT_NOT:
			switch( children_types[0] ){
			case t_INT:
				break;
			default:
				std::cout << "\tType error: bitwise not\n"
					<< "\t\tShould be of type INT, but child is " << type_string(children_types[0]) << "\n";
				throw("Type error: bitwise not");
			}
			prev_func_type = children_types[0];
			break;
		}
	}
	else{
		switch( ast_node->get_opperation() ){
		case TOK_BOOL_AND:
		case TOK_BOOL_OR:
			if (children_types[0] != t_BOOL || 
			    children_types[1] != t_BOOL){
				std::cout << "\tType error: boolean AND/OR\n"
					<< "\t\tChildren should be BOOL, but children are " << type_string(children_types[0]) << " and " << type_string(children_types[1]) << "\n";
				throw("Type error: boolean AND/OR");
			}
			prev_func_type = t_BOOL;
			break;
		case TOK_BIT_AND:
		case TOK_BIT_XOR:
		case TOK_BIT_OR:
		case TOK_MODULO:	//As with bitwise opperators, Modulo can only be used with ints
			if (children_types[0] != t_INT || children_types[1] != t_INT){
				std::cout << "\tType error: bitwise AND/XOR/OR\n"
					<< "\t\tChildren should be INT, but children are " << type_string(children_types[0]) << " and " << type_string(children_types[1]) << "\n";
				throw("Type error: bitwise AND/XOR/OR");
			}
			prev_func_type = children_types[0];
			break;
		case TOK_DIVIDE:
		case TOK_ASTERICK:
		case TOK_PLUS:
		case TOK_MINUS:
			if (children_types[0] != t_INT &&  children_types[0] != t_DOUBLE || 
			    children_types[1] != t_INT &&  children_types[1] != t_DOUBLE){
				std::cout << "\tType error: arithmetic opperation\n"
					<< "\t\tChildren should be INT or DOUBLE, but children are " << type_string(children_types[0]) << " and " << type_string(children_types[1]) << "\n";
				throw("Type error: arithmetic opperation");
			}
			if (children_types[0] == t_INT && children_types[1] == t_INT)
				prev_func_type = t_INT;
			else
				prev_func_type = t_DOUBLE;
			break;
		case TOK_EQ:
		case TOK_NOTEQ:
		case TOK_NOTGT:
		case TOK_NOTLT:
		case TOK_LTEQ:
		case TOK_GTEQ:
		case TOK_LT:
		case TOK_GT:
			if (children_types[0] != children_types[1]){
				std::cout << "\tType error: comparison opperation: " << ast_node->get_literal() << "\n"
					<< "\t\tChildren should be of the same type, but children are " << type_string(children_types[0]) << " and " << type_string(children_types[1]) << "\n";
				throw("Type error: comparison opperation");
			}
			prev_func_type = t_BOOL;
			break;
		}
	}
}
void type_check::visit(field_leaf* ast_node)
{
	prev_func_type = ast_node->get_type();
}
void type_check::visit(expression_node_leaf* ast_node)
{
	prev_func_type = ast_node->get_type();
}