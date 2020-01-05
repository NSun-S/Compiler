#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<utility>
#include "Token.h"
#include "outputHandler.h"
#include "errorHandler.h"
class lexicalPraser
{
public:
	lexicalPraser(std::string code,std::string filename,errorHandler &handler);
	std::vector<std::pair<std::string, std::string>> getTokenList();
	std::vector<std::pair<int,int>> getPositionList();
private:
	int row, col;
	int index;
	char ch;
	std::string code;
	std::string targetfile;
	std::vector<std::string> outputList;
	std::vector<std::pair<std::string, std::string>> TokenList;
	std::vector<std::pair<int, int>> PositionList;
	std::map<std::string,int> reserverMap;
	void lexicalPrase(errorHandler& handler);
	void initReserverMap();
	void output();
	void localization();
};

