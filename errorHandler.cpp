#include "errorHandler.h"

errorHandler::errorHandler(bool mode,string targetfile)
{
	this->mode = mode;
	this->targetfile = targetfile;
	if (mode) {
		initNaiveMap();
	}
}

void errorHandler::newError(int row, int col, type T)
{
	if (mode) {
		errorsList.push_back(make_tuple(row, col, naiveHandler[T]));
	}
}

bool errorHandler::cmp(tuple<int, int, string> a, tuple<int, int, string> b)
{
	return (get<0>(a)<get<0>(b))||(get<0>(a) == get<0>(b)&&get<1>(a) < get<1>(b));
}

void errorHandler::output()
{
	sort(errorsList.begin(), errorsList.end(), cmp);
	if (mode) {
		for (int i = 0; i < errorsList.size(); i++) {
			outputList.push_back(to_string(get<0>(errorsList[i])) + " " + get<2>(errorsList[i]));
		}
	}
	outputHandler output = outputHandler(outputList, targetfile);
}


void errorHandler::initNaiveMap()
{
	naiveHandler[illegalSymbolOrLexicalInconformity] = "a";
	naiveHandler[nameRedefinition] = "b";
	naiveHandler[undefinedName] = "c";
	naiveHandler[numberOfArgumentsNotMatch] = "d";
	naiveHandler[typeOfArgumentsNotMatch] = "e";
	naiveHandler[illegalTypesInCondition] = "f";
	naiveHandler[functionWithoutReturnValueMismatch] = "g";
	naiveHandler[functionWithReturnValueMismatchOrLose] = "h";
	naiveHandler[indexOfArrayCanOnlyBeIntegerExpresion] = "i";
	naiveHandler[cannotChangeValueOfConstant] = "j";
	naiveHandler[shouldBeSemicolon] = "k";
	naiveHandler[shouldBeRparent] = "l";
	naiveHandler[shouldBeRbrack] = "m";
	naiveHandler[loseWhileIn_DoWhile] = "n";
	naiveHandler[constantCanOnlyBeFollowedByIntegerOrCharacter] = "o";
}
