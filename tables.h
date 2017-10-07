#pragma once

#include <string>
#include <vector>

class table
{
private:
	enum Type{t_INT, t_CSTRING, t_DOUBLE, t_BOOL};
	std::vector<std::pair<std::string, Type>> Type_s = {{"int", t_INT}, {"string", t_CSTRING}, {"double", t_DOUBLE}, {"bool", t_BOOL}};
	typedef struct 
	{
		Type type;
		std::string name;
	} field;
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

	std::string table_name;
	std::vector<field> header;
	std::vector<std::vector<element>> rows;


public:
	table();
	table(std::string data_file);

	void add_file(std::string data_file);
};

class tables
{
private:
	std::vector<table> _tables;

public:
	tables();
	tables(const char data_file[]);
	tables(std::vector<std::string> data_files);

	void add_table(std::string data_file);
	void add_table(std::vector<std::string> data_files);

	std::vector<table> get_tables();
};