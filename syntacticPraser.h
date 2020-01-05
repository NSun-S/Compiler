#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<utility>
#include<map>
#include "Token.h"
#include "Syntax.h"
#include "outputHandler.h"
#include "errorHandler.h"
#include "symbolTable.h"

class syntacticPraser
{
public:
	syntacticPraser(std::vector<std::pair<std::string,std::string>> TokenList,
		std::vector<pair<int,int>> PositionList, std::string targetfile, errorHandler& handler);
	vector<vector<string>> getMidCode();
private:
	std::string targetfile;//the output path
	std::vector<std::pair<std::string, std::string>> TokenList;
	std::vector<std::pair<int,int>> PositionList;
	std::vector<std::string> SyntaxList;//outputList in syntactic analysize
	//std::map<std::string, int> funcTable;
	std::vector<symbolTable::type> typeList;
	symbolTable table;
	int firstMark,secondMark;
	Token transfer;
	Syntax syn;
	int index,length;//the index and length of TokenList
	std::string symbol;
	std::string content;
	int row, col;
	int returnType;
	//type of function's return value:0 means no return value,1 means need int return value,2 means need char return value
	bool returnMark;
	//used to mark whether a function with return value returns a value
	vector<vector<string>> intermediateCode;
	int location;//the index of intermediateCode
	int partition;//use to mark the partition of global variables and others
	map<string, int> globalVarMap;
	map<string, int> localVarMap;
	map<string, string> localConstMap;
	map<string, string> globalConstMap;
	int loopNum;//mark to distinguish different loop
	int varNum;//mark to distinguish different variable
	vector<string> reversePolish;

	void syntacticPrase(errorHandler& handler);
	void program(errorHandler& handler);
	void constdec(errorHandler& handler,bool localMark);//true means this is constdec in function
	void constdef(errorHandler& handler,bool localMark);
	void paraList(errorHandler& handler);
	void compound(errorHandler& handler);
	void states(errorHandler& handler);
	void state(errorHandler& handler);
	void constate(errorHandler& handler);
	void loopstate(errorHandler& handler);
	string assstate(errorHandler& handler,string &operand);
	//return the index of target,empty when not an array
	void readstate(errorHandler& handler);
	void writestate(errorHandler& handler);
	void condition(errorHandler& handler);
	bool exp(errorHandler& handler,string &operand);
	string item(errorHandler& handler,bool &mark);
	string factor(errorHandler& handler,bool &mark);//return identifier of factor
	void vParaList(errorHandler& handler,string funcName);
	void getsym();
	void outputSyn();
	void outputMid();
};

