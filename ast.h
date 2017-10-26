#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "lexemes.h"
#include "defs.h"

// Virtual can be forward declared in the .h file, but the implementation and dereference 
// has to be in the .cpp to avoid circular references.
class visitor;

class node
{
public:
	virtual void print() = 0;
	virtual void accept(visitor* v) = 0;
};

/*************************************************************************************************/
/*************************************************************************************************/

class node_branch : public node
{
protected:
	std::vector<node *> children;
	std::string type;
	std::string sub_type;
public:
	node_branch();
	node_branch(std::string sub_type);
	node_branch(std::string type, std::string sub_type);

	std::string get_type();
	std::string get_sub_type();

	std::vector<node *> get_children();

	virtual void print()
	{
		for(auto i : this->children)
			if(i != NULL)
				;//i->print();
	}
	
	virtual void accept(visitor* v);
	virtual void add_child(node*) = 0;
	void remove_child(int index);
	
};

/*************************************************************************************************/
/*************************************************************************************************/

class node_leaf : public node
{
protected:
	token_id token;
	std::string literal;
public:
	node_leaf(std::pair<std::string, token_id>);
	node_leaf(token_id token, std::string literal);

	token_id get_token();
	std::string get_literal();

	virtual void accept(visitor* v);
	virtual void print()
	{
		std::cout << this->literal << " ";
	}
	
};

/*************************************************************************************************/
/*************************************************************************************************/

class field_leaf : public node_leaf
{	
protected:
	Type type;
	std::string table;
	int table_x_index;
public:
	field_leaf(std::pair<std::string, token_id> p);
	field_leaf(std::pair<std::string, token_id> p, std::string table);
	void add_table(std::string table);
	std::string get_table();
	void set_type(Type t);
	void set_table_x_index(int i);
	Type get_type();
	int get_table_x_index();

	virtual void accept(visitor* v);
	virtual void print()
	{
		std::cout << this->table << "." << this->literal << " ";
	}
};

/*************************************************************************************************/
/*************************************************************************************************/

class ast
{
private:
	node_branch *root;

public:
	ast();
	ast(node_branch *query);

	node_branch *get_root();
	bool good();
	void print();
};

/*************************************************************************************************/
/*************************************************************************************************/

class expression_node_branch : public node_branch
{
private:
	bool result;
	token_id opperation;
	std::string literal;
	enum{
		BINARY_EXP,
		UNARY_EXP
	}opperation_type;

public:
	expression_node_branch(std::pair<std::string, token_id>);
	expression_node_branch(token_id opperation, std::string literal);

	std::string get_literal();
	bool is_unary_exp()
	{
		return opperation_type == UNARY_EXP;
	}
	token_id get_opperation();
	virtual void accept(visitor* v);
	virtual void print()
	{
		std::cout << literal;
		// if(opperation_type == UNARY_EXP){
		// 	std::cout << literal << "(";
		// 	if(children[0] != NULL) 
		// 		children[0]->print();
		// 	std::cout << ")";
		// }
		// else if(opperation_type == BINARY_EXP){
		// 	std::cout << "(";
		// 	if(children[0] != NULL) 
		// 		children[0]->print();
		// 	std::cout << ") " << literal << " (";
		// 	if(children[1] != NULL) 
		// 		children[1]->print();
		// 	std::cout << ")";
		// }
	}
	virtual void add_child(node* child)
	{
		if(opperation_type == BINARY_EXP && children.size() > 1)
			throw "Error: attempt to add a third child to a BINARY_EXP";
		if(opperation_type == UNARY_EXP && children.size() > 0)
			throw "Error: attempt to add a second child to a BINARY_EXP";
		children.insert(children.begin(), child);
	}
};

class expression_node_leaf : public field_leaf
{
protected:
	int i;
	double d;
	bool b;
	std::string s;

	bool is_temp;
public:
	expression_node_leaf(std::pair<std::string, token_id>, std::string table);
	expression_node_leaf(std::pair<std::string, token_id>);

	void mark_temp();
	bool get_is_temp();
	void* get_value();
	virtual void accept(visitor* v);
	virtual void print()
	{
		switch(token){
		case TOK_IDENTIFIER:
			field_leaf::print();
			break;
		case TOK_INTEGER:
			node_leaf::print();
			break;
		case TOK_DECIMAL:
			node_leaf::print();
			break;
		case TOK_BOOL:
			if(b == true)
				std::cout << "true ";
			else
				std::cout << "false ";
			break;
		case TOK_STRING:
			std::cout << s;
			break;
		default:
			throw "Trying to print illegal expression_node_leaf";
			break;
			
		}
	}
};
/*************************************************************************************************/
/*                                                                                               */
/*                                             SELECT                                            */
/*                                                                                               */
/*************************************************************************************************/

class select_subject : public node_branch
{
public:
	select_subject();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_SELECT)
			throw "Error: attempt to add non-select_by token to select_subject";
		if(children.size() > 0)
			throw "Error: attempt to add a second leaf node to select_subject";
		this->children.push_back(child);
	}
	
};

/*************************************************************************************************/

class select_predicate : public node_branch
{
public:
	select_predicate();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_IDENTIFIER && child->get_token() != TOK_ASTERICK && child->get_token() != TOK_PERIOD)
			throw "Error: invalid token added to select_predicate";
		children.push_back(child);
	}
};

/*************************************************************************************************/

class select_clause : public node_branch
{
public:
	select_clause();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_branch *child = dynamic_cast<node_branch *>(_child);
		if(child->get_type() != "SELECT")
			throw "Error: attempt to add non-select child to select_clause";
		if(child->get_sub_type() == "SUBJECT")
			this->children.push_back((select_subject*)child);
		else if(child->get_sub_type() == "PREDICATE")
			this->children.push_back((select_predicate*)child);
		else
			throw "Error: attempt to add non-SUBJECT or non-PREDICATE sub_type in select_clause";
	}
};

/*************************************************************************************************/
/*                                                                                               */
/*                                              FROM                                             */
/*                                                                                               */
/*************************************************************************************************/

class from_subject : public node_branch
{
public:
	from_subject();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_FROM)
			throw "Error: attempt to add non-from_by token to from_subject";
		if(children.size() > 0)
			throw "Error: attempt to add a second leaf node to from_subject";
		this->children.push_back(child);
	}
	
};

/*************************************************************************************************/

class from_predicate : public node_branch
{
public:
	from_predicate();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(children.size() > 0)
			throw "Error: adding multipule children to from_predicate";
		if(child->get_token() != TOK_IDENTIFIER)
			throw "Error: adding non-identifier token into from predicate";
		children.push_back(child);
	}
	void add_child(node_leaf* child1, node_leaf* child2, node_leaf* child3)
	{
		if(children.size() > 0)
			throw "Error: adding multipul children to from_predicate";
		if(child1->get_token() != TOK_IDENTIFIER || child2->get_token() != TOK_PERIOD || child3->get_token() != TOK_IDENTIFIER)
			throw "Error: attempt to add 3 children not of the form TOK_IDENTIFIER TOK_PERIOD TOK_IDENTIFIER in from_predicate";
		children.push_back(child1);
		children.push_back(child2);
		children.push_back(child3);
	}
	
};

/*************************************************************************************************/

class from_clause : public node_branch
{
public:
	from_clause();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_branch *child = dynamic_cast<node_branch *>(_child);
		if(child->get_type() != "FROM")
			throw "Error: attempt to add non-from child to from_clause";
		if(child->get_sub_type() == "SUBJECT")
			children.push_back((from_subject*)child);
		else if(child->get_sub_type() == "PREDICATE")
			children.push_back((from_predicate*)child);
		else
			throw "Error: attempt to add non-SUBJECT or non-PREDICATE sub_type in from_clause";
	}
};

/*************************************************************************************************/
/*                                                                                               */
/*                                             WHERE                                             */
/*                                                                                               */
/*************************************************************************************************/

class where_subject : public node_branch
{
public:
	where_subject();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_WHERE)
			throw "Error: attempt to add non-where_by token to where_subject";
		if(children.size() > 0)
			throw "Error: attempt to add a second leaf node to where_subject";
		this->children.push_back(child);
	}
	
};

/*************************************************************************************************/

class where_predicate : public node_branch
{
public:
	where_predicate();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		//expression_node_branch *child = dynamic_cast<expression_node_branch *>(_child);
		children.push_back(_child);
	}
	
};

/*************************************************************************************************/

class where_clause : public node_branch
{
public:
	where_clause();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_branch *child = dynamic_cast<node_branch *>(_child);
		if(child->get_type() != "WHERE")
			throw "Error: attempt to add non-where child to where_clause";
		if(child->get_sub_type() == "SUBJECT")
			children.push_back((where_subject*)child);
		else if(child->get_sub_type() == "PREDICATE")
			children.push_back((where_predicate*)child);
		else
			throw "Error: attempt to add non-SUBJECT or non-PREDICATE sub_type in where_clause";
	}
};

/*************************************************************************************************/
/*                                                                                               */
/*                                            ORDER_BY                                           */
/*                                                                                               */
/*************************************************************************************************/

class order_by_subject : public node_branch
{
public:
	order_by_subject();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_ORDER && child->get_token() != TOK_BY)
			throw "Error: attempt to add non-group_by token to group_by_subject";
		if(child->get_token() == TOK_ORDER)
		{
			if(this->children.size() != 0)
				throw "Error: attempt to add TOK_ORDER to order_by_subject after a token has already been added";
		}
		else if(child->get_token() == TOK_BY)
		{
			if(this->children.size() != 1)
				throw "Error: attempt to add invalid TOK_BY to order_by_subject";
		}

		this->children.push_back(child);
	}
	void add_child(node_leaf *child1, node_leaf *child2)
	{
		if(child1->get_token() != TOK_ORDER || child2->get_token() != TOK_BY)
			throw "Error: attempt to add non-order_by token to order_by_subject";
		if(children.size() > 1)
			throw "Error: attempt to add a third leaf node to order_by_subject";
		this->children.push_back(child1);
		this->children.push_back(child2);
	}
	
};

/*************************************************************************************************/

class order_by_predicate : public node_branch
{
public:
	order_by_predicate();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_IDENTIFIER && child->get_token() != TOK_ASC && child->get_token() != TOK_DESC)
			throw "Error: adding non-identifier, non-asc, and non-desc token into order_by predicate";
		children.push_back(child);
	}
	void add_child(node_leaf* child1, node_leaf* child2, node_leaf* child3)
	{
		if(child1->get_token() != TOK_IDENTIFIER || child2->get_token() != TOK_PERIOD || child3->get_token() != TOK_IDENTIFIER)
			throw "Error: attempt to add 3 children not of the form TOK_IDENTIFIER TOK_PERIOD TOK_IDENTIFIER in order_by_predicate";
		children.push_back(child1);
		children.push_back(child2);
		children.push_back(child3);
	}
	
};

/*************************************************************************************************/

class order_by_clause : public node_branch
{
public:
	order_by_clause();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_branch *child = dynamic_cast<node_branch *>(_child);
		if(child->get_type() != "ORDER_BY")
			throw "Error: attempt to add non-order_by child to order_by_clause";
		if(child->get_sub_type() == "SUBJECT")
			children.push_back((order_by_subject*)child);
		else if(child->get_sub_type() == "PREDICATE")
			children.push_back((order_by_predicate*)child);
		else
			throw "Error: attempt to add non-SUBJECT or non-PREDICATE sub_type in order_by_clause";
	}
};

/*************************************************************************************************/
/*                                                                                               */
/*                                            GROUP_BY                                           */
/*                                                                                               */
/*************************************************************************************************/

class group_by_subject : public node_branch
{
public:
	group_by_subject();

	virtual void accept(visitor* v);
	void add_child(node* _child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_GROUP && child->get_token() != TOK_BY)
			throw "Error: attempt to add non-group_by token to group_by_subject";
		if(child->get_token() == TOK_GROUP)
			if(this->children.size() != 0)
				throw "Error: attempt to add TOK_GROUP to group_by_subject after a token has already been added";
			else
				this->children.push_back(child);
		else if(child->get_token() == TOK_BY)
			if(this->children.size() != 1)
				throw "Error: attempt to add invalid TOK_BY to group_by_subject";
			else
				this->children.push_back(child);

	}
	void add_child(node_leaf *child1, node_leaf *child2)
	{
		if(child1->get_token() != TOK_GROUP && child2->get_token() != TOK_BY)
			throw "Error: attempt to add non-group_by token to group_by_subject";
		if(children.size() > 0)
			throw "Error: attempt to add a third leaf node to group_by_subject";
		this->children.push_back(child1);
		this->children.push_back(child2);
	}
	
};

/*************************************************************************************************/

class group_by_predicate : public node_branch
{
public:
	group_by_predicate();

	virtual void accept(visitor* v);
	void add_child(node *_child)
	{
		node_leaf *child = dynamic_cast<node_leaf *>(_child);
		if(child->get_token() != TOK_IDENTIFIER && child->get_token() != TOK_PERIOD)
			throw "Error: adding non-identifier token into group_by predicate";
		children.push_back(child);
	}
	void add_child(node_leaf* child1, node_leaf* child2, node_leaf* child3)
	{
		if(children.size() > 0)
			throw "Error: adding multipul children to group_by_predicate";
		if(child1->get_token() != TOK_IDENTIFIER || child2->get_token() != TOK_PERIOD || child3->get_token() != TOK_IDENTIFIER)
			throw "Error: attempt to add 3 children not of the form TOK_IDENTIFIER TOK_PERIOD TOK_IDENTIFIER in group_by_predicate";
		children.push_back(child1);
		children.push_back(child2);
		children.push_back(child3);
	}
	
};

/*************************************************************************************************/

class group_by_clause : public node_branch
{
public:
	group_by_clause();

	virtual void accept(visitor* v);
	void add_child(node* _child)
	{
		node_branch *child = dynamic_cast<node_branch *>(_child);
		if(child->get_type() != "GROUP_BY")
			throw "Error: attempt to add non-group_by child to group_by_clause";
		if(child->get_sub_type() == "SUBJECT")
			children.push_back((group_by_subject*)child);
		else if(child->get_sub_type() == "PREDICATE")
			children.push_back((group_by_predicate*)child);
		else
			throw "Error: attempt to add non-SUBJECT or non-PREDICATE sub_type in group_by_clause";
	}
};


class select_statement : public node_branch
{
public:
	select_statement();

	virtual void accept(visitor* v);
	void add_child(node* _clause)
	{
		if(_clause == NULL)
			return;

		node_branch *clause = dynamic_cast<node_branch *>(_clause);

		if(children.end() != std::find_if(children.begin(), children.end(), 
			[=](auto e){
				if(dynamic_cast<node_branch *>(e)->get_type() == clause->get_type())
					return true;
				else
					return false;
			}))
			throw "Error: ast already contains " + clause->get_type() + " clause";
		children.push_back(clause);
	}
};
