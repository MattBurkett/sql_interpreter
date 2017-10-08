#pragma once

#include <string>
#include <vector>

#define DATADIR "databases/"
#define DATAEXT ".csv"

class table
{
public:
	enum Type{t_INT, t_CSTRING, t_DOUBLE, t_BOOL};
private:
	typedef struct 
	{
		Type type;
		union 
		{
			int i; 
			double d; 
			char* s;
			bool b;
		} data;
	} element;
	typedef struct 
	{
		Type type;
		std::string name;
	} field;

	std::vector<std::pair<std::string, Type>> Type_s = {{"int", t_INT}, {"string", t_CSTRING}, {"double", t_DOUBLE}, {"bool", t_BOOL}};
	std::string table_name;
	std::vector<field> header;
	std::vector<std::vector<element>> rows;


public:
	table();
	table(std::string data_file);
	std::string get_name();
	std::string get_name_useable();

	void add_file(std::string data_file);
	std::vector<std::pair<std::string, table::Type>> get_columns();
};

class tables
{
private:
	std::vector<table> _tables;
	int query_table_index;

public:
	tables();
	tables(const char data_file[]);
	tables(std::vector<std::string> data_files);

	void add_table(std::string data_file);
	void add_table(std::vector<std::string> data_files);
	bool set_query_table(std::string table_name);

	std::vector<table> get_tables();
	table get_query_table();
};