#include <fstream>
#include <iostream>
#include <dirent.h>
#include "tables.h"

table::table()
{}

table::table(std::string data_file)
{
	add_file(data_file);
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



tables::tables()
{}

tables::tables(const char directory_location[])
{	
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
