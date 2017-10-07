#pragma once

#include <vector>
#include "ast.h"

class parser{
private:
	typedef struct
	{
		expression_node_branch* parent; 
		expression_node_branch* root;
	}parent_w_root_t;

	std::vector<std::pair<std::string, token_id>>::iterator current_token;
	std::vector<std::pair<std::string, token_id>> token_stream;
	std::pair<std::string, token_id> match(token_id token);
	void advance();

	node_branch *_query();
	select_clause *_select();
	from_clause *_from();
	where_clause *_where();
	order_by_clause *_order_by();
	group_by_clause *_group_by();
	void _tables(node_branch* parent);
	void _where_condition(node_branch* parent);
	node* _bool_or();
	parent_w_root_t _bool_or2();
	node* _bool_and();
	parent_w_root_t _bool_and2();
	node* _bool_not();
	node* _comparison();
	parent_w_root_t _comparison2();
	node* _pm_opperations();
	parent_w_root_t _pm_opperations2();
	node* _md_opperations();
	parent_w_root_t _md_opperations2();
	node* _bit_not();
	node* _base();
	group_by_clause *_group_by_clause();
	void _column(node_branch* parent);
	void _more_columns(node_branch* parent);
	order_by_clause *_order_by_clause();
public:
	parser();
	parser(std::vector<std::pair<std::string, token_id>> tokens);
	ast parse();
};