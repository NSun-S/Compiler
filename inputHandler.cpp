#include "inputHandler.h"

inputHandler::inputHandler(std::string filename)
{
	readFile(filename);
}

std::string inputHandler::toString()
{
	return txtContent;
}

void inputHandler::readFile(std::string filename)
{
	std::fstream file(filename);
	char ch;
	while (!file.eof()) {
		file.get(ch);
		txtContent += ch;
	}
	file.close();
}
