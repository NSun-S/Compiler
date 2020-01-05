#pragma once
#include<string>
#include<vector>
#include<fstream>

class outputHandler
{
public:
	outputHandler(std::vector<std::string> content,std::string filename);
private:
	std::string targetFile;
	std::vector <std::string> content;
	void output();
};

