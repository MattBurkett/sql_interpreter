#include "lexemes.h"
#include "parser.h"
#include <string>
#include <iostream>
#include <exception>

std::pair<std::string, token_id> parser::match(token_id token)
{
	std::pair<std::string, token_id> return_value = *current_token;
	if(token != current_token->second)
		throw "Parser: invalid match. Expected (" + std::to_string(token) + "), but recieved (" + std::to_string(current_token->second) + "): " + current_token->first;
	advance();
	return return_value;
}
void parser::advance()
{
	if(current_token == token_stream.end())
		throw "Parser: error, Advanced token past end of stream";
	current_token++;
}

parser::parser()
{

}
parser::parser(std::vector<std::pair<std::string, token_id>> tokens)
{
	token_stream = tokens;
}

node_branch* parser::_query()
{
	select_statement* query_node = new select_statement();
	query_node->add_child(_select());
	query_node->add_child(_from());
	query_node->add_child(_where());
	query_node->add_child(_group_by());
	query_node->add_child(_order_by());

	if(current_token != token_stream.end() && current_token->second == TOK_SEMI)
		match(TOK_SEMI);
	return query_node;
}

select_clause* parser::_select()
{
	select_clause *select_node = new select_clause();
	select_subject *select_node_subject = new select_subject();
	select_predicate *select_node_predicate = new select_predicate();
	
	select_node->add_child(select_node_subject);
	select_node->add_child(select_node_predicate);

	select_node_subject->add_child(new node_leaf(match(TOK_SELECT)));
	if(current_token->second == TOK_ASTERICK)
		select_node_predicate->add_child(new node_leaf(match(TOK_ASTERICK)));
	else{
		_column(select_node_predicate);
		_more_columns(select_node_predicate);	
	}
	return select_node;
}

from_clause* parser::_from()
{
	from_clause *from_node = new from_clause();
	from_subject *from_node_subject = new from_subject();
	from_predicate *from_node_predicate = new from_predicate();
	
	from_node->add_child(from_node_subject);
	from_node->add_child(from_node_predicate);

	from_node_subject->add_child(new node_leaf(match(TOK_FROM)));
	_tables(from_node_predicate);

	return from_node;
}	

where_clause* parser::_where()
{
	where_clause *where_node = NULL;
	if(current_token != token_stream.end() && current_token->second == TOK_WHERE){
		where_node = new where_clause();
		where_subject *where_node_subject = new where_subject();
		where_predicate *where_node_predicate = new where_predicate();
		
		where_node->add_child(where_node_subject);
		where_node->add_child(where_node_predicate);

		where_node_subject->add_child(new node_leaf(match(TOK_WHERE)));

		_where_condition(where_node_predicate);
	}
	return where_node;
}	

group_by_clause* parser::_group_by()
{
	if(current_token != token_stream.end() && current_token->second == TOK_GROUP){
		return _group_by_clause();
	}
	return NULL;
}	

order_by_clause* parser::_order_by()
{
	if(current_token != token_stream.end() && current_token->second == TOK_ORDER){
		return _order_by_clause();
	}
	return NULL;
}	

void parser::_tables(node_branch* parent)
{
	parent->add_child(new node_leaf(match(TOK_IDENTIFIER)));
}

void parser::_where_condition(node_branch* parent)
{
	node *child = _bool_or();
	parent->add_child(child);
}
node* parser::_bool_or()
{
	node* child = _bool_and();
	parent_w_root_t parent_w_root = _bool_or2();
	if(parent_w_root.parent == NULL){
		return child;
	}
	parent_w_root.parent->add_child(child);
	return parent_w_root.root;
}
parser::parent_w_root_t parser::_bool_or2()
{
	if(current_token != token_stream.end() && current_token->second == TOK_BOOL_OR){
		expression_node_branch* parent = new expression_node_branch(match(TOK_BOOL_OR));
		node* child = _bool_and();
		parent_w_root_t parent_w_root = _bool_or2();

		parent->add_child(child);
		if(parent_w_root.parent == NULL){
			return {parent, parent};
		}
		parent_w_root.parent->add_child(parent);
		return {parent, parent_w_root.root};
	}
	return {NULL, NULL};
}
node* parser::_bool_and()
{
	node* child = _bool_not();
	parent_w_root_t parent_w_root = _bool_and2();
	if(parent_w_root.parent == NULL){
		return child;
	}
	parent_w_root.parent->add_child(child);
	return parent_w_root.root;
}
parser::parent_w_root_t parser::_bool_and2()
{
	if(current_token != token_stream.end() && current_token->second == TOK_BOOL_AND){
		expression_node_branch* parent = new expression_node_branch(match(TOK_BOOL_AND));
		node* child = _bool_not();
		parent_w_root_t parent_w_root = _bool_and2();

		parent->add_child(child);
		if(parent_w_root.parent == NULL){
			return {parent, parent};
		}
		parent_w_root.parent->add_child(parent);
		return {parent, parent_w_root.root};
	}
	return {NULL, NULL};
}
node* parser::_bool_not()
{
	if(current_token != token_stream.end() && current_token->second == TOK_BOOL_NOT){
		expression_node_branch* parent = new expression_node_branch(match(TOK_BOOL_NOT));
		node* child  = _bool_not();

		parent->add_child(child);
		return parent;
	}
	else
		return _comparison();
}
node* parser::_comparison()
{
	node* child = _pm_opperations();
	parent_w_root_t parent_w_root = _comparison2();
	if(parent_w_root.parent == NULL){
		return child;
	}
	parent_w_root.parent->add_child(child);
	return parent_w_root.root;
}
parser::parent_w_root_t parser::_comparison2()
{
	expression_node_branch* parent;
	node* child;
	parent_w_root_t parent_w_root;

	if(current_token != token_stream.end()){
		switch(current_token->second){
		case TOK_EQ: 
			parent = new expression_node_branch(match(TOK_EQ));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_GT: 
			parent = new expression_node_branch(match(TOK_GT));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_LT: 
			parent = new expression_node_branch(match(TOK_LT));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_GTEQ: 
			parent = new expression_node_branch(match(TOK_GTEQ));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_LTEQ: 
			parent = new expression_node_branch(match(TOK_LTEQ));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_NOTEQ: 
			parent = new expression_node_branch(match(TOK_NOTEQ));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_NOTGT: 
			parent = new expression_node_branch(match(TOK_NOTGT));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		case TOK_NOTLT: 
			parent = new expression_node_branch(match(TOK_NOTLT));
			child = _pm_opperations();
			parent_w_root = _comparison2();
			break;
		default:
			return {NULL, NULL};
		}
	}
	parent->add_child(child);
	if(parent_w_root.parent == NULL){
		return {parent, parent};
	}
	parent_w_root.parent->add_child(parent);
	return {parent, parent_w_root.root};

}
node* parser::_pm_opperations()
{
	if(current_token != token_stream.end()){
		if(current_token->second == TOK_PLUS)
			match(TOK_PLUS);
		else if(current_token->second == TOK_MINUS)
			match(TOK_MINUS);
	}

	node* child = _md_opperations();
	parent_w_root_t parent_w_root = _pm_opperations2();
	if(parent_w_root.parent == NULL){
		return child;
	}
	parent_w_root.parent->add_child(child);
	return parent_w_root.root;
}
parser::parent_w_root_t parser::_pm_opperations2()
{
	expression_node_branch* parent;
	node* child;
	parent_w_root_t parent_w_root;

	if(current_token != token_stream.end()){
		switch(current_token->second){
		case TOK_PLUS: 
			parent = new expression_node_branch(match(TOK_PLUS));
			child = _md_opperations();
			parent_w_root = _pm_opperations2();
			break;
		case TOK_MINUS: 
			parent = new expression_node_branch(match(TOK_MINUS));
			child = _md_opperations();
			parent_w_root = _pm_opperations2();
			break;
		case TOK_BIT_AND: 
			parent = new expression_node_branch(match(TOK_BIT_AND));
			child = _md_opperations();
			parent_w_root = _pm_opperations2();
			break;
		case TOK_BIT_OR: 
			parent = new expression_node_branch(match(TOK_BIT_OR));
			child = _md_opperations();
			parent_w_root = _pm_opperations2();
			break;
		case TOK_BIT_XOR: 
			parent = new expression_node_branch(match(TOK_BIT_XOR));
			child = _md_opperations();
			parent_w_root = _pm_opperations2();
			break;
		default:
			return {NULL, NULL};
		}
	}

	parent->add_child(child);
	if(parent_w_root.parent == NULL){
		return {parent, parent};
	}
	parent_w_root.parent->add_child(parent);
	return {parent, parent_w_root.root};
}
node* parser::_md_opperations()
{
	node* child = _bit_not();
	parent_w_root_t parent_w_root = _md_opperations2();
	if(parent_w_root.parent == NULL){
		return child;
	}
	parent_w_root.parent->add_child(child);
	return parent_w_root.root;

	
}
parser::parent_w_root_t parser::_md_opperations2()
{
	expression_node_branch* parent;
	node* child;
	parent_w_root_t parent_w_root;

	if(current_token != token_stream.end()){
		switch(current_token->second){
		case TOK_ASTERICK: 
			parent = new expression_node_branch(match(TOK_ASTERICK));
			child = _bit_not();
			parent_w_root = _md_opperations2();
			break;
		case TOK_DIVIDE: 
			parent = new expression_node_branch(match(TOK_DIVIDE));
			child = _bit_not();
			parent_w_root = _md_opperations2();
			break;
		case TOK_MODULO: 
			parent = new expression_node_branch(match(TOK_MODULO));
			child = _bit_not();
			parent_w_root = _md_opperations2();
			break;
		default:
			return {NULL, NULL};		
		}
	}

	parent->add_child(child);
	if(parent_w_root.parent == NULL){
		return {parent, parent};
	}
	parent_w_root.parent->add_child(parent);
	return {parent, parent_w_root.root};
}
node* parser::_bit_not()
{

	if(current_token != token_stream.end() && current_token->second == TOK_BIT_NOT){
		expression_node_branch* parent = new expression_node_branch(match(TOK_BIT_NOT));
		node* child  = _bit_not();

		parent->add_child(child);
		return parent;
	}
	else 
		return _base();
}

node* parser::_base()
{
	node* parent = NULL;
	parent_w_root_t parent_w_root;
	std::pair<std::string, token_id> leaf;

	if(current_token != token_stream.end()){
		switch(current_token->second){
		case TOK_LPAREN:
			match(TOK_LPAREN);
			parent = _bool_or();
			match(TOK_RPAREN);
			break;
		case TOK_IDENTIFIER:
			leaf = match(TOK_IDENTIFIER);
			
			if(current_token != token_stream.end() && current_token->second == TOK_PERIOD){
				match(TOK_PERIOD);
				parent = new expression_node_leaf(leaf, match(TOK_IDENTIFIER).first);
			}
			else{
				parent = new expression_node_leaf(leaf, "");
			}
			break;
		case TOK_INTEGER:
			leaf = match(TOK_INTEGER);
			parent = new expression_node_leaf(leaf);
			break;
		case TOK_DECIMAL:
			leaf = match(TOK_DECIMAL);
			parent = new expression_node_leaf(leaf);
			break;
		case TOK_STRING:
			leaf = match(TOK_STRING);
			parent = new expression_node_leaf(leaf);
			break;
		case TOK_BOOL:
			leaf = match(TOK_BOOL);
			for(auto c : leaf.first)
				c = tolower(c);
			parent = new expression_node_leaf(leaf);
			break;
		default:
			throw "Parser: Reached _base() without a match. Current token: " + current_token->first + " (" + std::to_string(current_token->second) + ")";
		}
	}
	return parent;
}

group_by_clause *parser::_group_by_clause()
{
	group_by_clause *group_by_node = new group_by_clause();
	group_by_subject *group_by_node_subject = new group_by_subject();
	group_by_predicate *group_by_node_predicate = new group_by_predicate();
	
	group_by_node->add_child(group_by_node_subject);
	group_by_node->add_child(group_by_node_predicate);

	group_by_node_subject->add_child(new node_leaf(match(TOK_GROUP)));
	group_by_node_subject->add_child(new node_leaf(match(TOK_BY)));

	_column(group_by_node_predicate);
	_more_columns(group_by_node_predicate);

	return group_by_node;
}

void parser::_column(node_branch* parent)
{
	auto column = match(TOK_IDENTIFIER);
	if(current_token != token_stream.end() && current_token->second == TOK_PERIOD){
		match(TOK_PERIOD);
		auto table = match(TOK_IDENTIFIER);
		parent->add_child(new field_leaf(column, table.first));
	}
	else
		parent->add_child(new field_leaf(column));
}

void parser::_more_columns(node_branch* parent)
{
	if(current_token != token_stream.end() && current_token->second == TOK_COMMA){
		match(TOK_COMMA);
		_column(parent);
		_more_columns(parent);
	}
}

void parser::_column_with_asc(node_branch* parent)
{
	auto column = match(TOK_IDENTIFIER);
	if(current_token != token_stream.end() && current_token->second == TOK_PERIOD){
		match(TOK_PERIOD);
		auto table = match(TOK_IDENTIFIER);
		parent->add_child(new field_leaf(column, table.first));
	}
	else
		parent->add_child(new field_leaf(column));

	if(current_token != token_stream.end())
		if(current_token->second == TOK_ASC)
			parent->add_child( new node_leaf(match(TOK_ASC)) );
		else if(current_token->second == TOK_DESC)
			parent->add_child( new node_leaf(match(TOK_DESC)) );
			
}

void parser::_more_columns_with_asc(node_branch* parent)
{
	if(current_token != token_stream.end() && current_token->second == TOK_COMMA){
		match(TOK_COMMA);
		_column_with_asc(parent);
		_more_columns_with_asc(parent);
	}
}

order_by_clause *parser::_order_by_clause()
{
	order_by_clause *order_by_node = new order_by_clause();
	order_by_subject *order_by_node_subject = new order_by_subject();
	order_by_predicate *order_by_node_predicate = new order_by_predicate();
	
	order_by_node->add_child(order_by_node_subject);
	order_by_node->add_child(order_by_node_predicate);

	order_by_node_subject->add_child(new node_leaf(match(TOK_ORDER)));
	order_by_node_subject->add_child(new node_leaf(match(TOK_BY)));

	_column_with_asc(order_by_node_predicate);
	_more_columns_with_asc(order_by_node_predicate);

	return order_by_node;
}

ast parser::parse()
{
	current_token = token_stream.begin();
	node_branch *ast_root;

	ast_root = _query();
	if(current_token != token_stream.end())
		throw "\n\tFinished parsing with tokens remaining. Count: " + std::to_string(token_stream.end() - current_token);
	else
		return ast(ast_root);
	
}
