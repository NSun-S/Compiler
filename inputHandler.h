#pragma once
#include<string>
#include<fstream>

class inputHandler
{
public:
	inputHandler(std::string filename);
	std::string toString();
private:
	std::string txtContent;
	void readFile(std::string filename);
};

