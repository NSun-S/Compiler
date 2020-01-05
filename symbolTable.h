#pragma once
#include<vector>
#include<string>
#include<tuple>
//using namespace std;
class symbolTable
{
public:
	enum type {
		integer,character,array,refun,norefun
	};
	enum addition {
		_const,_int,_char,_void,_empty
	};
	symbolTable();
	int index;//the index of current symbol
	bool add(std::string name,type T,bool paraMark,int newSpaceMark,addition add);
	//add a new symbol into the symbol table, return false when error
	int find(std::string name, std::vector<type> typeList);
	//find a identifier in the table, return negetive integer when error
	void drop();
	//drop current scope
private:
	int lastIndex;//the index of last scope
	std::vector<std::tuple<std::string, type, std::vector<type>, int, addition>> table;
	//name, type, parameter, last index, type of return value

};

