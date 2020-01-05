#include "outputHandler.h"
#include<iostream>
#include<ios>

outputHandler::outputHandler(std::vector<std::string> content, std::string filename)
{
	targetFile = filename;
	this->content = content;
	output();
}

void outputHandler::output()
{
	std::fstream outputStream(targetFile,std::ios::out);
	for (auto iter = content.cbegin(); iter != content.cend(); iter++) {
		outputStream << *iter<<std::endl;
	}
	outputStream.close();
}
