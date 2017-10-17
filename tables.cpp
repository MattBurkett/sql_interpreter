#include <fstream>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <algorithm>
#include "tables.h"

table::table()
{}

table::table(std::string data_file)
{
	add_file(data_file);
}

table::table(std::vector<std::vector<element>> rows, std::vector<field> header)
{
	this->rows = rows;
	this->header = header;
}

std::string table::get_name()
{
	return table_name;
}

std::string table::get_name_useable()
{
	return table_name.substr(strlen(DATADIR), table_name.length() - strlen(DATADIR) - strlen(DATAEXT));
}

std::vector<std::vector<table::element>> table::get_rows()
{
	return rows;
}

std::vector<table::field> table::get_header()
{
	return header;
}

void table::sort(std::string name, bool ascending)
{
	auto ittr = header.begin();
	for(; ittr < header.end(); ittr++)
		if(ittr->name == name)
			break;
	int index = ittr - header.begin();

	std::stable_sort(rows.begin(), rows.end(),
		[=](const auto &row1, const auto &row2)
		{
			switch(ittr->type){
			case t_INT:
				return (row1[index].data.i < row2[index].data.i) == ascending;
				break;
			case t_CSTRING:
				return (strcmp(row1[index].data.s, row2[index].data.s) <= 0) == ascending;
				break;
			case t_DOUBLE:
				return (row1[index].data.d < row2[index].data.d) == ascending;
				break;
			case t_BOOL:
				return (row1[index].data.b < row2[index].data.b) == ascending;
				break;
			}
			return true;
		});
}

void table::remove_columns(std::vector<field> columns)
{
	int offset;
	for(field column : columns){
		offset = std::find_if(header.begin(), header.end(),
			[=](field elem1)
			{
				return elem1.type == column.type && elem1.name == column.name;
			}) - header.begin();

		if(offset < header.size()){
			header.erase(header.begin() + offset);
			for(auto &row : rows)
				row.erase(row.begin() + offset);
		}
		else
			std::cout << "Attempt to remove not existing column: " << column.name << "\n";
	}

}

void table::add_file(std::string data_file)
{
	table_name = data_file;

	std::fstream file_stream (data_file, std::fstream::in);
	std::string line1;
	std::string line2;

	if(file_stream.is_open()){
		int line2_offset = 0;
		field tmp;
		//Reading header
		std::getline(file_stream, line1);
		std::getline(file_stream, line2);
		
		line1 += ',';
		auto current_type = line2.begin();
		for(auto e : line1)
			if(e == ','){
				for(auto s : Type_s)
					if(line2.compare(line2_offset,s.first.size(),s.first) == 0){
						tmp.type = s.second;
						line2_offset += s.first.size() + 1;
						break;
					}
				header.push_back(tmp);
				tmp.name = "";
			}
			else
				tmp.name += e;

		//Reading Data
		while(!file_stream.eof()){
			std::vector<element> row;
			element tmp;
			for(auto iter = header.begin(); iter < header.end(); iter++){
				if(iter != header.end() - 1)
					std::getline(file_stream, line1, ',');
				else
					std::getline(file_stream, line1, '\n');

				tmp.type = iter->type;
				switch(iter->type)
				{
				case t_INT:
					tmp.data.i = stoi(line1);
					break;

				case t_CSTRING:
					tmp.data.s = (char*)malloc((line1.size() + 1)*sizeof(char));
					line1.copy(tmp.data.s, line1.size());
					tmp.data.s[line1.size()] = '\0';
					break;

				case t_DOUBLE:
					tmp.data.d = stof(line1);
					break;

				case t_BOOL:
					if(line1.compare("true") == 0)
						tmp.data.b = true;
					else
						tmp.data.b = false;
					break;
				}
				row.push_back(tmp);
			}
			rows.push_back(row);
		}
	}
	file_stream.close();
}

void table::set_columns(std::vector<field> header)
{
	std::vector<field> new_set(this->header.size());
	std::vector<field> header_copy(this->header);

	std::sort(header_copy.begin(), header_copy.end());
	std::sort(header.begin(), header.end());

	//set_difference requires elements to be sorted
	auto end_ittr = std::set_difference(
			header_copy.begin(), header_copy.end(),
			header.begin(), header.end(), 
			new_set.begin());
	new_set.erase(end_ittr, new_set.end());

	remove_columns(new_set);
}

std::vector<std::pair<std::string, Type>> table::get_columns()
{
	std::vector<std::pair<std::string, Type>> columns;
	for(auto elem : header)
		columns.push_back( std::pair<std::string, Type>(elem.name, elem.type) );

	return columns;
}

void table::print()
{
	for(const auto &f : header){
		std::cout << f.name << ((&f != &header.back()) ? std::string(", ") : std::string("\n"));
	}
	for(const auto &row : rows){
		for(const auto &elem : row){
			switch(elem.type){
			case t_INT:
				std::cout << elem.data.i;
				break;
			case t_CSTRING:
				std::cout << elem.data.s;
				break;
			case t_DOUBLE:
				std::cout << elem.data.d << ((elem.data.d - (int)elem.data.d == 0) ? std::string(".0") : std::string(""));
				break;
			case t_BOOL:
				std::cout << ( elem.data.b ? std::string("true") : std::string("false") );
				break;
			}
			std::cout << ((&elem != &row.back()) ? std::string(" | ") : std::string("\n"));
		}
	}
}

tables::tables()
{
	query_table_index = 0;
}

tables::tables(const char directory_location[])
{	
	query_table_index = 0;
	std::string line;
	std::vector<std::string> data_files;
	DIR *directory = opendir(directory_location);
	struct dirent *dir;

	if(directory == NULL){
		std::cout << "failed to open " << directory_location;
		exit(-1);
	}
	while(dir = readdir(directory))
	{
		if(dir->d_name[0] != '.'){
			line = dir->d_name;
			data_files.push_back(directory_location + line);
		}
	}
	add_table(data_files);
}

tables::tables(std::vector<std::string> data_files)
{
	query_table_index = 0;
	add_table(data_files);
}

void tables::add_table(std::string data_file)
{
	_tables.push_back(table(data_file));
}

void tables::add_table(std::vector<std::string> data_files)
{
	for (auto s : data_files)
		add_table(s);
}

std::vector<table> tables::get_tables()
{
	return _tables;
}

table tables::get_query_table()
{
	return _tables[query_table_index];
}

bool tables::set_query_table(std::string table_name)
{
	int i = 0;
	for(table t : _tables)
		if(t.get_name_useable() == table_name){
			query_table_index = i;
			return true;
		}
		else
			i++;
	return false;
}

