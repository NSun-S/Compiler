#pragma once
#include<map>
#include<string>
#include<vector>
#include<tuple>
#include<algorithm>
#include "outputHandler.h"
using namespace std;
class errorHandler
{
public:
	errorHandler(bool mode,string targetfile);
	enum type {
		illegalSymbolOrLexicalInconformity,
		nameRedefinition,
		undefinedName,
		numberOfArgumentsNotMatch,
		typeOfArgumentsNotMatch,
		illegalTypesInCondition,
		functionWithoutReturnValueMismatch,
		functionWithReturnValueMismatchOrLose,
		indexOfArrayCanOnlyBeIntegerExpresion,
		cannotChangeValueOfConstant,
		shouldBeSemicolon,
		shouldBeRparent,
		shouldBeRbrack,
		loseWhileIn_DoWhile,
		constantCanOnlyBeFollowedByIntegerOrCharacter
	};
	void newError(int row,int col,type T);
	void output();
private:
	bool mode;//true while judge
	static bool cmp(tuple<int, int, string> a, tuple<int, int, string> b);
	map<type, string> naiveHandler;
	vector<tuple<int,int,string>> errorsList;
	vector<string> outputList;
	string targetfile;
	void initNaiveMap();
};

