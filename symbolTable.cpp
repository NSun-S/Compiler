#include "symbolTable.h"
#include <iostream>

symbolTable::symbolTable()
{
	index = 0;
	lastIndex = 0;
}

bool symbolTable::add(std::string name, type T,bool paraMark,int newSapceMark,addition add)
{
	if (newSapceMark == 2) drop();
	int temp = index - 1;
	bool success = true;
	while (temp >= 0 && std::get<3>(table[temp]) == -1){
		if (std::get<0>(table[temp]) == name
			&&!(paraMark && (std::get<4>(table[temp])==_int 
				|| std::get<4>(table[temp]) == _void||std::get<4>(table[temp]) == _char))) {
			success = false;
			break;
		}
		temp--;
	}
	if (1) {
		std::vector<type> paraList;
		if (newSapceMark == 1) {//deeper layer
			//if (index > 0)
			lastIndex = index - 1;
			//else
			//	lastIndex = index;
			table.push_back(std::make_tuple(name, T, paraList, lastIndex, add));
		}
		else if (newSapceMark == 2) {//same depth layer
			lastIndex = index-1;
			table.push_back(std::make_tuple(name, T, paraList, lastIndex, add));
		}
		else {
			table.push_back(std::make_tuple(name, T, paraList, -1, add));
		}
		if (paraMark) {
			std::get<2>(table[lastIndex+1]).push_back(T);
		}
		index++;
	}
	/*for (int i = index-1; i >= 0; i--) {
		std::cout << std::get<0>(table[i]) << " "
			<< std::to_string(std::get<3>(table[i]))<<std::endl;
	}
	std::cout << "-------------------------\n";*/
	return success;
}

int symbolTable::find(std::string name, std::vector<type> typeList)
{
	int temp = index-1;
	bool findMark = false;
	while (temp >= 0) {
		if (std::get<0>(table[temp]) == name) {
			findMark = true;
			break;
		}
		if (temp>0&&std::get<3>(table[temp]) != -1) {
			temp = std::get<3>(table[temp]);
		}
		else {
			temp--;
		}
	}
	if (!findMark) return -1;//no find error

	if (std::get<1>(table[temp]) == refun||std::get<1>(table[temp])==norefun) {
		if (typeList.size() != std::get<2>(table[temp]).size()) {
			return -2;//parameter number no match
		}
		for (int i = 0; i < typeList.size(); i++) {
			if (typeList[i] != std::get<2>(table[temp])[i]) {
				return -3;//parameter type no match
			}
		}
	}
	if (std::get<1>(table[temp]) == integer && std::get<4>(table[temp]) == _const) return 3;
	else if (std::get<1>(table[temp]) == character && std::get<4>(table[temp]) == _const) return 4;
	else if (std::get<1>(table[temp]) == integer) return 1;
	else if (std::get<1>(table[temp]) == character) return 2;
	else if (std::get<1>(table[temp]) == norefun) return 0;
	else {
		if (std::get<4>(table[temp]) == _int) return 5;
		else return 6;
	}
}

void symbolTable::drop()
{
	index--;
	while (index > 1&&std::get<3>(table[index]) == -1) {
		table.pop_back();
		index--;
	}
	std::get<3>(table[index]) = -1;
	index++;
}
