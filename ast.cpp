
#include "ast.h"
#include "visitor.h"

/*************************************************************************************************/
/*                                                                                               */
/*                                           node_branch                                         */
/*                                                                                               */
/*************************************************************************************************/

node_branch::node_branch()
{
}

node_branch::node_branch(std::string sub_type)
{
	this->type = "TOKEN";
	this->sub_type = sub_type;
}
node_branch::node_branch(std::string type, std::string sub_type)
{
	this->type = type;
	this->sub_type = sub_type;
}

void node_branch::accept(visitor* v)
{
	v->visit(this);
}

std::string node_branch::get_type()
{
	return type;
}

std::string node_branch::get_sub_type()
{
	return sub_type;
}


std::vector<node *> node_branch::get_children()
{
	return children;
}

void node_branch::remove_child(int index)
{
	children.erase(children.begin() + index);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                            node_leaf                                          */
/*                                                                                               */
/*************************************************************************************************/

node_leaf::node_leaf(std::pair<std::string, token_id> token_pair)
{
	this->token = token_pair.second;
	this->literal = token_pair.first;
}

node_leaf::node_leaf(token_id token, std::string literal)
{
	this->token = token;
	this->literal = literal;
}

void node_leaf::accept(visitor* v)
{
	v->visit(this);
}

token_id node_leaf::get_token()
{
	return this->token;
}

std::string node_leaf::get_literal()
{
	return this->literal;
}

/*************************************************************************************************/
/*                                                                                               */
/*                                           field_leaf                                          */
/*                                                                                               */
/*************************************************************************************************/

field_leaf::field_leaf(std::pair<std::string, token_id> p) : node_leaf(p) 
{
	type = t_UNINIT;
	this->table = "";
	table_x_index = 0;
}

field_leaf::field_leaf(std::pair<std::string, token_id> p, std::string table) : node_leaf(p)
{
	type = t_UNINIT;
	this->table = table;
	table_x_index = 0;
}

std::string field_leaf::get_table()
{
	return table;
}

void field_leaf::accept(visitor* v)
{
	v->visit(this);
}

void field_leaf::add_table(std::string table)
{
	this->table = table;
}

void field_leaf::set_type(Type t)
{
	type = t;
}

void field_leaf::set_table_x_index(int i)
{
	table_x_index = i;
}

Type field_leaf::get_type()
{
	return type;
}

int field_leaf::get_table_x_index()
{
	return table_x_index;
}
/*************************************************************************************************/
/*                                                                                               */
/*                                     expression_node_branch                                    */
/*                                                                                               */
/*************************************************************************************************/

token_id expression_node_branch::get_opperation()
{
	return opperation;
}

expression_node_branch::expression_node_branch(std::pair<std::string, token_id> token_pair)
{
	this->opperation = token_pair.second;
	this->literal = token_pair.first;
	switch(opperation){
	case TOK_BOOL_NOT:
	case TOK_BIT_NOT:
		opperation_type = UNARY_EXP;
		break;
	default:
		opperation_type = BINARY_EXP;
		break;
	}
}

expression_node_branch::expression_node_branch(token_id opperation, std::string literal)
{
	this->opperation = opperation;
	this->literal = literal;
	switch(opperation){
	case TOK_BOOL_NOT:
	case TOK_BIT_NOT:
		opperation_type = UNARY_EXP;
		break;
	default:
		opperation_type = BINARY_EXP;
		break;
	}
}

void expression_node_branch::accept(visitor* v)
{
	v->visit(this);
}


/*************************************************************************************************/
/*                                                                                               */
/*                                      expression_node_leaf                                     */
/*                                                                                               */
/*************************************************************************************************/

expression_node_leaf::expression_node_leaf(std::pair<std::string, token_id> token_pair, std::string table) : field_leaf(token_pair, table)
{
	is_temp = false;
}

expression_node_leaf::expression_node_leaf(std::pair<std::string, token_id> token_pair) : field_leaf(token_pair)
{
	is_temp = false;
	switch(token){
	case TOK_INTEGER: 
		this->i = stoi(token_pair.first);
		break;
	case TOK_DECIMAL:
		this->d = stof(token_pair.first);
		break;
	case TOK_BOOL:
		this->b = token_pair.first == "true";
		break;
	case TOK_STRING:
		this->s = token_pair.first.substr(1, token_pair.first.size() - 2);
	}
}

void expression_node_leaf::mark_temp()
{
	is_temp = true;
}

bool expression_node_leaf::get_is_temp()
{
	return is_temp;
}

void* expression_node_leaf::get_value()
{
	switch(token){
	case TOK_INTEGER: 
		return &i;
	case TOK_DECIMAL:
		return &d;
	case TOK_BOOL:
		return &b;
	case TOK_STRING:
		return &s;
	}
	return NULL;
}

void expression_node_leaf::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                             SELECT                                            */
/*                                                                                               */
/*************************************************************************************************/

select_subject::select_subject()
{
	type = "SELECT";
	sub_type = "SUBJECT";
}

void select_subject::accept(visitor* v)
{
	v->visit(this);
}

select_predicate::select_predicate()
{
	type = "SELECT";
	sub_type = "PREDICATE";
}

void select_predicate::accept(visitor* v)
{
	v->visit(this);
}

select_clause::select_clause()
{
	type = "SELECT";
	sub_type = "CLAUSE";
}

void select_clause::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                              FROM                                             */
/*                                                                                               */
/*************************************************************************************************/
from_subject::from_subject()
{
	type = "FROM";
	sub_type = "SUBJECT";
}

void from_subject::accept(visitor* v)
{
	v->visit(this);
}

from_predicate::from_predicate()
{
	type = "FROM";
	sub_type = "PREDICATE";
}

void from_predicate::accept(visitor* v)
{
	v->visit(this);
}

from_clause::from_clause()
{
	type = "FROM";
	sub_type = "CLAUSE";
}

void from_clause::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                             WHERE                                             */
/*                                                                                               */
/*************************************************************************************************/
where_subject::where_subject()
{
	type = "WHERE";
	sub_type = "SUBJECT";
}

void where_subject::accept(visitor* v)
{
	v->visit(this);
}

where_predicate::where_predicate()
{
	type = "WHERE";
	sub_type = "PREDICATE";
}

void where_predicate::accept(visitor* v)
{
	v->visit(this);
}

where_clause::where_clause()
{
	type = "WHERE";
	sub_type = "CLAUSE";
}

void where_clause::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                            ORDER_BY                                           */
/*                                                                                               */
/*************************************************************************************************/
order_by_subject::order_by_subject()
{
	type = "ORDER_BY";
	sub_type = "SUBJECT";
}

void order_by_subject::accept(visitor* v)
{
	v->visit(this);
}

order_by_predicate::order_by_predicate()
{
	type = "ORDER_BY";
	sub_type = "PREDICATE";
}

void order_by_predicate::accept(visitor* v)
{
	v->visit(this);
}

order_by_clause::order_by_clause()
{
	type = "ORDER_BY";
	sub_type = "CLAUSE";
}

void order_by_clause::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                            GROUP_BY                                           */
/*                                                                                               */
/*************************************************************************************************/
group_by_subject::group_by_subject()
{
	type = "GROUP_BY";
	sub_type = "SUBJECT";
}

void group_by_subject::accept(visitor* v)
{
	v->visit(this);
}

group_by_predicate::group_by_predicate()
{
	type = "GROUP_BY";
	sub_type = "PREDICATE";
}

void group_by_predicate::accept(visitor* v)
{
	v->visit(this);
}

group_by_clause::group_by_clause()
{
	type = "GROUP_BY";
	sub_type = "CLAUSE";
}

void group_by_clause::accept(visitor* v)
{
	v->visit(this);
}

/*************************************************************************************************/
/*                                                                                               */
/*                                              AST                                              */
/*                                                                                               */
/*************************************************************************************************/
ast::ast()
{
	this->root = NULL;
}

ast::ast(node_branch *query)
{
	this->root = query;
}

node_branch *ast::get_root()
{
	return root;
}

bool ast::good()
{
	return root != NULL;
}

void ast::print()
{
	root->print();
}

/*************************************************************************************************/
/*                                                                                               */
/**/
/*                                                                                               */
/*************************************************************************************************/
select_statement::select_statement()
{
}

void select_statement::accept(visitor* v)
{
	v->visit(this);
}