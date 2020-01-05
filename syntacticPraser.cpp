#include "syntacticPraser.h"
#include <iostream>

syntacticPraser::syntacticPraser(std::vector<std::pair<std::string,std::string>> TokenList,
	std::vector<pair<int, int>> PositionList, std::string targetfile, errorHandler& handler)
{
	this->TokenList = TokenList;
	this->targetfile = targetfile;
	this->PositionList = PositionList;
	length = TokenList.size();
	index = 0;
	firstMark = 0;
	secondMark = 0;
	location = 0;
	partition = 0;
	loopNum = 0;
	varNum = 0;
	syntacticPrase(handler);
	//outputSyn();
	outputMid();
}

vector<vector<string>> syntacticPraser::getMidCode()
{
	return intermediateCode;
}

void syntacticPraser::syntacticPrase(errorHandler& handler)
{
	getsym();
	program(handler);
	vector<string> temp;
	intermediateCode.insert(intermediateCode.begin(), temp);
	intermediateCode[0].push_back(".data");
	partition++;
	temp.clear();
	intermediateCode.insert(intermediateCode.begin() + partition, temp);
	intermediateCode[partition].push_back(".text");
	partition++;
	temp.clear();
	intermediateCode.insert(intermediateCode.begin() + partition, temp);
	intermediateCode[partition].push_back("jump");
	intermediateCode[partition].push_back("main");
}

void syntacticPraser::program(errorHandler& handler)
{
	if (symbol == transfer.get(transfer._const)) {
		constdec(handler,false);
	}
	bool varmark = false;
	bool hasfunc = false;
	while(symbol == transfer.get(transfer._int)
		|| symbol == transfer.get(transfer._char)){
		std::string tempSymbol = symbol;
		string tempType = content;
		getsym();
		if (symbol == transfer.get(transfer.identifier)) {
			std::string tempid = content;
			getsym();
			if (symbol == transfer.get(transfer.lparent)) {
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("funcLabel");
				intermediateCode[location].push_back(tempid + ":");
				location++;
				if (varmark) {
					SyntaxList.insert(SyntaxList.end() - 2, syn.get(syn.vardec));
					varmark = false;
				}
				returnMark = false;
				if (secondMark == 0) {
					secondMark = 1;
					if (tempSymbol == transfer.get(transfer._int))
					{
						returnType = 1;
						if (!table.add(tempid, table.refun, false, 1, table._int))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						returnType = 2;
						if (!table.add(tempid, table.refun, false, 1, table._char))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				else {
					if (tempSymbol == transfer.get(transfer._int))
					{
						returnType = 1;
						if (!table.add(tempid, table.refun, false, 2, table._int))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						returnType = 2;
						if (!table.add(tempid, table.refun, false, 2, table._char))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				hasfunc = true;
				//funcTable[tempid] = 1;
				SyntaxList.push_back(syn.get(syn.statehead));
				getsym();
				paraList(handler);
				if (symbol != transfer.get(transfer.rparent)) {
					handler.newError(row, col, handler.shouldBeRparent);
				}
				else getsym();
				if (symbol == transfer.get(transfer.lbrace)) {
					getsym();
					compound(handler);
					if (!returnMark)
						handler.newError(row, col, handler.functionWithReturnValueMismatchOrLose);
					if (symbol == transfer.get(transfer.rbrace)) {
						getsym();
					}
					else {}
				}else{}
				temp.clear();
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("back");
				location++;
				SyntaxList.push_back(syn.get(syn.redef));
			}
			else {
				if (firstMark == 0) {
					firstMark = 1;
					if (tempSymbol == transfer.get(transfer._int))
					{
						globalVarMap[tempid] = 1;
						if (!table.add(tempid, table.integer, false, 1, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						globalVarMap[tempid] = 2;
						if (!table.add(tempid, table.character, false, 1, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				else
				{
					if (tempSymbol == transfer.get(transfer._int))
					{
						globalVarMap[tempid] = 1;
						if (!table.add(tempid, table.integer, false, 0, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						globalVarMap[tempid] = 2;
						if (!table.add(tempid, table.character, false, 0, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				string tempSize = "1";
				if (symbol == transfer.get(transfer.lbrack)) {
					getsym();
					if (symbol == transfer.get(transfer.intconst)) {
						tempSize = content;
						getsym();
						SyntaxList.push_back(syn.get(syn.unsignedInt));
						if (symbol != transfer.get(transfer.rbrack)) {
							handler.newError(row, col, handler.shouldBeRbrack);
						}
						else getsym();
					}	
				}
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("var");
				intermediateCode[location].push_back(tempType);
				intermediateCode[location].push_back(tempid);
				intermediateCode[location].push_back(tempSize);
				location++;
				partition++;
				if (symbol == transfer.get(transfer.comma)) {
					do {
						getsym();
						if (symbol == transfer.get(transfer.identifier)) {
							tempid = content;
							if (tempSymbol == transfer.get(transfer._int))
							{
								globalVarMap[tempid] = 1;
								if (!table.add(content, table.integer, false, 0, table._empty))
									handler.newError(row, col, handler.nameRedefinition);
							}
							else
							{
								globalVarMap[tempid] = 2;
								if (!table.add(content, table.character, false, 0, table._empty))
									handler.newError(row, col, handler.nameRedefinition);
							}
							getsym();
							string tempSize = "1";
							if (symbol == transfer.get(transfer.lbrack)) {
								getsym();
								if (symbol == transfer.get(transfer.intconst)) {
									tempSize = content;
									getsym();
									SyntaxList.push_back(syn.get(syn.unsignedInt));
									if (symbol != transfer.get(transfer.rbrack)) {
										handler.newError(row, col, handler.shouldBeRbrack);
									}
									else getsym();
								}
								else {}
							}
							vector<string> temp;
							intermediateCode.push_back(temp);
							intermediateCode[location].push_back("var");
							intermediateCode[location].push_back(tempType);
							intermediateCode[location].push_back(tempid);
							intermediateCode[location].push_back(tempSize);
							location++;
							partition++;
						}
						else {}
					} while (symbol == transfer.get(transfer.comma));
					if (symbol != transfer.get(transfer.semicolon)) {
						handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
					}
					else {
						SyntaxList.push_back(syn.get(syn.vardef));
						getsym();
						varmark = true;
					}
				}
				else {
					if (symbol != transfer.get(transfer.semicolon)) {
						handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
					}
					else {
						SyntaxList.push_back(syn.get(syn.vardef));
						getsym();
						varmark = true;
					}
				}

			}
		}
	}
	if (!hasfunc && varmark) {
		SyntaxList.push_back(syn.get(syn.vardec));
	}
	while (symbol == transfer.get(transfer._void)
		||symbol == transfer.get(transfer._int)
		||symbol == transfer.get(transfer._char)) {
		std::string tempSymbol = symbol;
		if (symbol == transfer.get(transfer._void)) {
			returnType = 0;
			getsym();
			if (symbol == transfer.get(transfer.identifier)) { // function without return value
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("funcLabel");
				intermediateCode[location].push_back(content + ":");
				location++;
				if (secondMark == 0) {
					secondMark = 1;
					if (!table.add(content, table.norefun, false, 1, table._void))
						handler.newError(row, col, handler.nameRedefinition);
				}
				else {
					if(!table.add(content, table.norefun, false, 2, table._void))
						handler.newError(row, col, handler.nameRedefinition);
				}
				hasfunc = true;
				//funcTable[content] = 0;
				getsym();
				if (symbol == transfer.get(transfer.lparent)) {
					getsym();
					paraList(handler);
					if (symbol != transfer.get(transfer.rparent)) {
						handler.newError(row, col, handler.shouldBeRparent);
					}
					else getsym();
					if (symbol == transfer.get(transfer.lbrace)) {
						getsym();
						compound(handler);
						if (symbol == transfer.get(transfer.rbrace)) {
							getsym();
						}
						else {}
					}
					else {}
				}
				else {}
				temp.clear();
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("back");
				location++;
				SyntaxList.push_back(syn.get(syn.noredef));
			}
			else if (symbol == transfer.get(transfer._main)) { // main function
				localVarMap.clear();
				localConstMap.clear();
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("mainLabel");
				intermediateCode[location].push_back(content + ":");
				location++;
				if(hasfunc)
					table.add(content, table.norefun, false, 2, table._void);
				else
				{
					secondMark = 1;
					table.add(content, table.norefun, false, 1, table._void);
				}
				getsym();
				if (symbol == transfer.get(transfer.lparent)) {
					getsym();
					if (symbol != transfer.get(transfer.rparent)) {
						handler.newError(row, col, handler.shouldBeRparent);
					}
					else getsym();
				}
				else {}
				if (symbol == transfer.get(transfer.lbrace)) {
					getsym();
					compound(handler);
					if (symbol == transfer.get(transfer.rbrace)) {
						getsym();
					}
				}
				SyntaxList.push_back(syn.get(syn.main));
				break;
			}
			else {}
		}
		else { //function with return value
			returnMark = false;
			if (symbol == transfer.get(transfer._int))
				returnType = 1;
			else
				returnType = 2;
			getsym();
			if (symbol == transfer.get(transfer.identifier)) {
				if (secondMark == 0) {
					secondMark = 1;
					if (tempSymbol == transfer.get(transfer._int))
					{
						if (!table.add(content, table.refun, false, 1, table._int))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						if (!table.add(content, table.refun, false, 1, table._char))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				else {
					if (tempSymbol == transfer.get(transfer._int))
					{
						if (!table.add(content, table.refun, false, 2, table._int))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						if (!table.add(content, table.refun, false, 2, table._char))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("funcLabel");
				intermediateCode[location].push_back(content + ":");
				location++;
				hasfunc = true;
				//funcTable[content] = 1;
				getsym();
				if (symbol == transfer.get(transfer.lparent)) {
					SyntaxList.push_back(syn.get(syn.statehead));
					getsym();
					paraList(handler);
					if (symbol != transfer.get(transfer.rparent)) {
						handler.newError(row, col, handler.shouldBeRparent);
					}
					else getsym();
					if (symbol == transfer.get(transfer.lbrace)) {
						getsym();
						compound(handler);
						
						if (!returnMark)
							handler.newError(row, col, handler.functionWithReturnValueMismatchOrLose);
						if (symbol == transfer.get(transfer.rbrace)) {
							getsym();
						}
						else {}
					}
					else {}
				}
				temp.clear();
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("back");
				location++;
			}
			SyntaxList.push_back(syn.get(syn.redef));
		}
	}
	SyntaxList.push_back(syn.get(syn.program));
}

void syntacticPraser::constdec(errorHandler& handler, bool localMark)
{	
	do {
		getsym();
		constdef(handler, localMark);
		if (symbol != transfer.get(transfer.semicolon)) {		
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else getsym();
	} while (symbol == transfer.get(transfer._const));
	SyntaxList.push_back(syn.get(syn.constdec));
}

void syntacticPraser::constdef(errorHandler& handler, bool localMark)
{
	string tempType = content;
	if (symbol == transfer.get(transfer._int)) {
		do {
			getsym();
			if (symbol == transfer.get(transfer.identifier)) {
				string tempId = content;
				
				if (firstMark == 0) {
					firstMark = 1;
					if(!table.add(content, table.integer, false, 1, table._const))
						handler.newError(row, col, handler.nameRedefinition);
				}
				else {
					if (!table.add(content, table.integer, false, 0, table._const))
						handler.newError(row, col, handler.nameRedefinition);
				}
				
				getsym();
				if (symbol == transfer.get(transfer.assign)) {
					getsym();
					string tempValue = "";
					if (symbol == transfer.get(transfer.plus)
						|| symbol == transfer.get(transfer.minus)) {
						tempValue = tempValue + content;
						getsym();
					}
					if (symbol == transfer.get(transfer.intconst)) {
						tempValue = tempValue + content;
						getsym();
						SyntaxList.push_back(syn.get(syn.unsignedInt));
						SyntaxList.push_back(syn.get(syn.integer));
					} else {
						handler.newError(row, col, handler.constantCanOnlyBeFollowedByIntegerOrCharacter);
						getsym();
					}
					vector<string> temp;
					//intermediateCode.push_back(temp);
					if (!localMark) {
						//intermediateCode[location].push_back("const");
						//partition++;
						globalConstMap[tempId] = tempValue;
					}
					else {
						//intermediateCode[location].push_back("localConst");
						//localVarMap[tempId] = 1;
						localConstMap[tempId] = tempValue;
					}
					/*intermediateCode[location].push_back(tempType);
					intermediateCode[location].push_back(tempId);
					intermediateCode[location].push_back("=");
					intermediateCode[location].push_back(tempValue);
					location++;*/
				}
				else {
				}
			}else{}
		} while (symbol == transfer.get(transfer.comma));
		
	}
	else if (symbol == transfer.get(transfer._char)) {
		do {
			getsym();
			if (symbol == transfer.get(transfer.identifier)) {
				string tempId = content;

				if (firstMark == 0) {
					firstMark = 1;
					if(!table.add(content, table.character, false, 1, table._const))
						handler.newError(row, col, handler.nameRedefinition);
				}
				else {
					if (!table.add(content, table.character, false, 0, table._const))
						handler.newError(row, col, handler.nameRedefinition);
				}
				getsym();
				if (symbol == transfer.get(transfer.assign)) {
					getsym();
					string tempValue;
					if (symbol == transfer.get(transfer.charconst)) {
						tempValue = content;
						getsym();
					}
					else {
						handler.newError(row, col, handler.constantCanOnlyBeFollowedByIntegerOrCharacter);
						getsym();
					}
					vector<string> temp;
					//intermediateCode.push_back(temp);
					if (!localMark) {
						//intermediateCode[location].push_back("const");
						//partition++;
						globalConstMap[tempId] = "'"+tempValue+"'";
					}
					else {
						//intermediateCode[location].push_back("localConst");
						//localVarMap[tempId] = 2;
						localConstMap[tempId] = "'"+tempValue+"'";
					}
					/*intermediateCode[location].push_back(tempType);
					intermediateCode[location].push_back(tempId);
					intermediateCode[location].push_back("=");
					intermediateCode[location].push_back(tempValue);
					location++;*/
				}
				else {
				}
			}else{}
		} while (symbol == transfer.get(transfer.comma));
	} else {
	}
	SyntaxList.push_back(syn.get(syn.constdef));
}

void syntacticPraser::paraList(errorHandler& handler)
{
	localVarMap.clear();
	localConstMap.clear();
	if (symbol == transfer.get(transfer.rparent)) {	
		SyntaxList.push_back(syn.get(syn.paraList));
	} else {
		bool firstmark = true;
		do {
			if (!firstmark) {
				getsym();
			}
			if (symbol == transfer.get(transfer._int)
				|| symbol == transfer.get(transfer._char)) {
				string tempSymbol = symbol;
				string tempType = content;
				getsym();
				if (symbol == transfer.get(transfer.identifier)) {
					if (tempSymbol == transfer.get(transfer._int))
					{
						localVarMap[content] = 1;
						if (!table.add(content, table.integer, true, 0, table._int))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						localVarMap[content] = 2;
						if (!table.add(content, table.character, true, 0, table._char))
							handler.newError(row, col, handler.nameRedefinition);
					}
					
					vector<string> temp;
					intermediateCode.push_back(temp);
					intermediateCode[location].push_back("para");
					intermediateCode[location].push_back(tempType);
					intermediateCode[location].push_back(content);
					location++;
					getsym();
				}else{}
			}else{}
			firstmark = false;
		} while (symbol == transfer.get(transfer.comma));
		SyntaxList.push_back(syn.get(syn.paraList));
	}
}

void syntacticPraser::compound(errorHandler& handler)
{
	if (symbol == transfer.get(transfer._const)) {
		constdec(handler,true);
	}
	bool varmark = false;
	while (symbol == transfer.get(transfer._int)
		|| symbol == transfer.get(transfer._char)) {
		string tempSymbol = symbol;
		string tempType = content;
		do {
			getsym();
			if (symbol == transfer.get(transfer.identifier)) {
				string tempid = content;
				if (firstMark == 0) {
					firstMark = 1;
					if (tempSymbol == transfer.get(transfer._int))
					{
						if (!table.add(content, table.integer, false, 1, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						if (!table.add(content, table.character, false, 1, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				else if (secondMark == 0) {
					secondMark = 1;
					if (tempSymbol == transfer.get(transfer._int))
					{
						if (!table.add(content, table.integer, false, 2, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						if (!table.add(content, table.character, false, 2, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
				}
				else
				{
					if (tempSymbol == transfer.get(transfer._int)) {
						if (!table.add(content, table.integer, false, 0, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}
					else
					{
						if (!table.add(content, table.character, false, 0, table._empty))
							handler.newError(row, col, handler.nameRedefinition);
					}

				}
				getsym();
				string tempSize = "1";
				if (symbol == transfer.get(transfer.lbrack)) {
					getsym();
					if (symbol == transfer.get(transfer.intconst)) {
						tempSize = content;
						getsym();
						SyntaxList.push_back(syn.get(syn.unsignedInt));
						if (symbol != transfer.get(transfer.rbrack)) {
							handler.newError(row, col, handler.shouldBeRbrack);
						}
						else getsym();
					}
					else {}
				}
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("localVar");
				intermediateCode[location].push_back(tempType);
				intermediateCode[location].push_back(tempid);
				intermediateCode[location].push_back(tempSize);
				if(tempSymbol == transfer.get(transfer._int))
					localVarMap[tempid] = 1;
				else
					localVarMap[tempid] = 2;
				location++;
			}
			else {}
		} while (symbol == transfer.get(transfer.comma));
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else {
			SyntaxList.push_back(syn.get(syn.vardef));
			getsym();
			varmark = true;
		}
	}
	if (varmark) SyntaxList.push_back(syn.get(syn.vardec));
	states(handler);
	SyntaxList.push_back(syn.get(syn.compound));
}

void syntacticPraser::states(errorHandler& handler)
{
	while (symbol != transfer.get(transfer.rbrace)) {
		state(handler);
	}
	SyntaxList.push_back(syn.get(syn.states));
}

void syntacticPraser::state(errorHandler& handler)
{
	if (symbol == transfer.get(transfer._if)) {//condition state
		getsym();
		constate(handler);
	}
	else if (symbol == transfer.get(transfer.lbrace)) {//states
		getsym();
		states(handler);
		if (symbol == transfer.get(transfer.rbrace)) {
			getsym();
		}else{}
	}
	else if (symbol == transfer.get(transfer.identifier)) {
		std::string tempid = content;
		getsym();
		if (symbol == transfer.get(transfer.lparent)) {//call state
			getsym();
			vector<string> temp;
			/*intermediateCode.push_back(temp);
			intermediateCode[location].push_back("changeScope");
			intermediateCode[location].push_back(tempid);
			location++;*/
			vParaList(handler,tempid);
			/*temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("release");
			location++;*/
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("call");
			intermediateCode[location].push_back(tempid);
			location++;
			int findRes = table.find(tempid, typeList);
			if (findRes == -1)
				handler.newError(row, col, handler.undefinedName);
			if (findRes == -2)
				handler.newError(row, col, handler.numberOfArgumentsNotMatch);
			if (findRes == -3)
				handler.newError(row, col, handler.typeOfArgumentsNotMatch);
			if (findRes == 0) {
				SyntaxList.push_back(syn.get(syn.callnore));
			}
			else {
				SyntaxList.push_back(syn.get(syn.callre));
			}
			if (symbol != transfer.get(transfer.rparent)) {
				handler.newError(row, col, handler.shouldBeRparent);
			}
			else getsym();
			/*if (funcTable.count(tempid)) {
					if (funcTable[tempid] == 1) {
						SyntaxList.push_back(syn.get(syn.callre));
					}
					else {
						SyntaxList.push_back(syn.get(syn.callnore));
					}
				}else {}*/
			if (symbol != transfer.get(transfer.semicolon)) {
				handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
			}
			else
				getsym();
		}
		else if (symbol == transfer.get(transfer.assign)
			||symbol == transfer.get(transfer.lbrack)) {//assgin state
			int findRes = table.find(tempid, typeList);
			if (findRes == -1)
				handler.newError(row, col, handler.undefinedName);
			if (findRes == 3 || findRes == 4)
				handler.newError(row, col, handler.cannotChangeValueOfConstant);
			string operand,tempIndex;
			tempIndex = assstate(handler,operand);
			vector<string> temp;
			
			if (tempIndex == "")
			{
				if (intermediateCode[intermediateCode.size() - 1][0] == "OpState"
					|| intermediateCode[intermediateCode.size() - 1][0] == "assState")
				{
					if (intermediateCode[intermediateCode.size() - 1][1] == operand&&operand[0] == '\"')
					{
						intermediateCode[intermediateCode.size() - 1][1] = tempid;
					}
					else
					{
						intermediateCode.push_back(temp);
						intermediateCode[location].push_back("assState");
						intermediateCode[location].push_back(tempid);
						intermediateCode[location].push_back("=");
						intermediateCode[location].push_back(operand);
						location++;
					}
				}
				else
				{
					intermediateCode.push_back(temp);
					intermediateCode[location].push_back("assState");
					intermediateCode[location].push_back(tempid);
					intermediateCode[location].push_back("=");
					intermediateCode[location].push_back(operand);
					location++;
				}
			}
			else
			{
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("assState");
				intermediateCode[location].push_back(tempid);
				if (tempIndex != "")
					intermediateCode[location].push_back(tempIndex);
				intermediateCode[location].push_back("=");
				intermediateCode[location].push_back(operand);
				location++;

			}
			if (symbol != transfer.get(transfer.semicolon)) {
				handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
			}
			else getsym();
		}
		else{}
	}
	else if (symbol == transfer.get(transfer._scanf)) {//read state
		getsym();
		readstate(handler);
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else getsym();
	}
	else if (symbol == transfer.get(transfer._printf)) {//write state
		getsym();
		writestate(handler);
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else getsym();
	}
	else if (symbol == transfer.get(transfer.semicolon)) {//empty
		getsym();
	}
	else if (symbol == transfer.get(transfer._return)) {//retrun state
		getsym();
		if (symbol == transfer.get(transfer.lparent)) {
			if (returnType == 0)
				handler.newError(row, col, handler.functionWithoutReturnValueMismatch);
			getsym();
			string ret;
			if (exp(handler,ret)) {
				if (returnType == 1) {
					handler.newError(row, col, handler.functionWithReturnValueMismatchOrLose);
				}
			}
			else {
				if (returnType == 2) {
					handler.newError(row, col, handler.functionWithReturnValueMismatchOrLose);
				}
			}
			if (symbol != transfer.get(transfer.rparent)) {
				handler.newError(row, col, handler.shouldBeRparent);
			}
			else getsym();
			vector<string> temp;
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("ret");
			intermediateCode[location].push_back(ret);
			location++;
		}
		else {
			vector<string> temp;
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("ret");
			location++;
			if (returnType != 0)
				handler.newError(row, col, handler.functionWithReturnValueMismatchOrLose);
		}
		returnMark = true;
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else {
			SyntaxList.push_back(syn.get(syn.restate));
			getsym();
		}
	}
	else {//loop state
		loopstate(handler);
	}
	SyntaxList.push_back(syn.get(syn.state));
}

void syntacticPraser::constate(errorHandler& handler)
{
	if (symbol == transfer.get(transfer.lparent)) {
		int tempMark = loopNum;
		loopNum++;
		getsym();
		vector<string> temp;
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("if" + to_string(tempMark) + ":");
		location++;
		condition(handler);
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("branch");
		intermediateCode[location].push_back("BZ");
		intermediateCode[location].push_back("else" + to_string(tempMark));
		//intermediateCode[location].push_back("release");
		location++;
		if (symbol != transfer.get(transfer.rparent)) {
			handler.newError(row, col, handler.shouldBeRparent);
		}
		else getsym();
		state(handler);
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		
		if (symbol == transfer.get(transfer._else))
		{
			
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("jump");
			intermediateCode[location].push_back("endIf" + to_string(tempMark));
			location++;
		}
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("else" + to_string(tempMark) + ":");
		location++;
		if (symbol == transfer.get(transfer._else)) {
			/*temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("release");
			location++;
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("jump");
			intermediateCode[location].push_back("endIf" + to_string(tempMark));
			location++;*/
			getsym();
			state(handler);
			//getsym();
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("release");
			location++;
		}
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("endIf"+to_string(tempMark)+":");
		location++;
		
	}else{}
	SyntaxList.push_back(syn.get(syn.constate));
}

void syntacticPraser::loopstate(errorHandler& handler)
{
	vector<string> temp;
	int tempMark = loopNum;
	loopNum++;
	if (symbol == transfer.get(transfer._while)) {
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		/*temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("whileLoop" + to_string(tempMark) + ":");
		location++;*/
		int conIndex = location;
		vector<vector<string>> cons;
		getsym();
		if (symbol == transfer.get(transfer.lparent)) {
			getsym();
			condition(handler);
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("branch");
			intermediateCode[location].push_back("BZ");
			intermediateCode[location].push_back("whileEnd" + to_string(tempMark));
			location++;
			for (int tempIndex = conIndex; intermediateCode[tempIndex][0] != "branch"; tempIndex++)
			{
				cons.push_back(intermediateCode[tempIndex]);
			}
			if (symbol != transfer.get(transfer.rparent)) {
				handler.newError(row, col, handler.shouldBeRparent);
			}
			else getsym();
			temp.clear();//////////////////
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("label");
			intermediateCode[location].push_back("whileLoop" + to_string(tempMark) + ":");
			location++;/////////////////
			state(handler);
			/*temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("release");
			location++;
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("jump");
			intermediateCode[location].push_back("whileLoop" + to_string(tempMark));
			location++;*/
			for (int tempIndex = 0; tempIndex<cons.size(); tempIndex++)
			{
				intermediateCode.push_back(cons[tempIndex]);
				location++;
			}
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("branch");
			intermediateCode[location].push_back("BNZ");
			intermediateCode[location].push_back("whileLoop" + to_string(tempMark));
			location++;
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("label");
			intermediateCode[location].push_back("whileEnd" + to_string(tempMark) + ":");
			location++;
		}else{}
	}
	else if (symbol == transfer.get(transfer._do)) {
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("do_whileLoop" + to_string(tempMark) + ":");
		location++;
		getsym();
		state(handler);
		if (symbol != transfer.get(transfer._while)) {
			handler.newError(row, col, handler.loseWhileIn_DoWhile);
		}
		else {
			getsym();
		}
		if (symbol == transfer.get(transfer.lparent)) {
			getsym();
			condition(handler);
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("branch");
			intermediateCode[location].push_back("BNZ");
			intermediateCode[location].push_back("do_whileLoop" + to_string(tempMark));
			location++;
			if (symbol != transfer.get(transfer.rparent)) {
				handler.newError(row, col, handler.shouldBeRparent);
			}
			else getsym();
		}else{}
	}
	else if (symbol == transfer.get(transfer._for)) {
		getsym();
		if (symbol == transfer.get(transfer.lparent)) {
			getsym();
		}else {}
		string tempid = content;
		string result;
		if (symbol == transfer.get(transfer.identifier)) {
			if (table.find(content, typeList) == -1)
				handler.newError(row, col, handler.undefinedName);
			getsym();
		}else {}
		if (symbol == transfer.get(transfer.assign)) {
			getsym();	
			exp(handler,result);
		}else{}
		vector<string> temp;
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("assState");
		intermediateCode[location].push_back(tempid);
		intermediateCode[location].push_back("=");
		intermediateCode[location].push_back(result);
		location++;
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else getsym();
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		/*temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("forLoop" + to_string(tempMark) + ":");
		location++;*/
		int conIndex = location;
		vector<vector<string>> cons;
		//cout << conIndex << "-------" << location<<endl;
		condition(handler);
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("branch");
		intermediateCode[location].push_back("BZ");
		intermediateCode[location].push_back("forEnd" + to_string(tempMark));
		location++;
		for (int tempIndex = conIndex; intermediateCode[tempIndex][0] != "branch"; tempIndex++)
		{
			cons.push_back(intermediateCode[tempIndex]);
		}
		temp.clear();//////////////////
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("forLoop" + to_string(tempMark) + ":");
		location++;//////////////////////
		if (symbol != transfer.get(transfer.semicolon)) {
			handler.newError(PositionList[index - 2].first, col, handler.shouldBeSemicolon);
		}
		else getsym();
		tempid = content;
		if (symbol == transfer.get(transfer.identifier)) {
			if (table.find(content, typeList) == -1)
				handler.newError(row, col, handler.undefinedName);
			getsym();
		}else{}
		if (symbol == transfer.get(transfer.assign)) {
			getsym();
		}else{}
		string tempid2 = content;
		if (symbol == transfer.get(transfer.identifier)) {
			if (table.find(content, typeList) == -1)
				handler.newError(row, col, handler.undefinedName);
			getsym();
		}else{}
		string tempOp = "";
		if (symbol == transfer.get(transfer.plus)
			|| symbol == transfer.get(transfer.minus)) {
			tempOp = content;
			getsym();
		}else{}
		string tempValue = "";
		if (symbol == transfer.get(transfer.intconst)) {
			tempValue = content;
			getsym();
			SyntaxList.push_back(syn.get(syn.unsignedInt));
			SyntaxList.push_back(syn.get(syn.stepsize));
		}else{}
		if (symbol != transfer.get(transfer.rparent)) {
			handler.newError(row, col, handler.shouldBeRparent);
		}
		else getsym();
		if (localConstMap.find(tempid2) != localConstMap.end()) {
			tempid2 = localConstMap[tempid2];
		}
		else if (globalConstMap.find(tempid2) != globalConstMap.end()) {
			tempid2 = globalConstMap[tempid2];
		}
		state(handler);
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("OpState");
		intermediateCode[location].push_back(tempid);
		intermediateCode[location].push_back("=");
		intermediateCode[location].push_back(tempid2);
		intermediateCode[location].push_back(tempOp);
		intermediateCode[location].push_back(tempValue);
		location++;
		for (int tempIndex = 0; tempIndex < cons.size(); tempIndex++)
		{
			intermediateCode.push_back(cons[tempIndex]);
			location++;
		}
		//cout << "_________________" << endl;
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("branch");
		intermediateCode[location].push_back("BNZ");
		intermediateCode[location].push_back("forLoop" + to_string(tempMark));
		location++;
		/*temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("release");
		location++;
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("jump");
		intermediateCode[location].push_back("forLoop" + to_string(tempMark));
		location++;*/
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("label");
		intermediateCode[location].push_back("forEnd" + to_string(tempMark) + ":");
		location++;
	}else{}
	SyntaxList.push_back(syn.get(syn.loopstate));
}

string syntacticPraser::assstate(errorHandler& handler,string &operand)
{
	string tempIndex = "";
	if (symbol == transfer.get(transfer.assign)) {
		getsym();
		exp(handler,operand);
	}
	else if (symbol == transfer.get(transfer.lbrack)) {
		getsym();
		if (exp(handler,operand))
			handler.newError(row, col, handler.indexOfArrayCanOnlyBeIntegerExpresion);
		tempIndex = operand;
		if (symbol != transfer.get(transfer.rbrack)) {
			handler.newError(row, col, handler.shouldBeRbrack);
		} 
		else getsym();
		if (symbol == transfer.get(transfer.assign)) {
			getsym();
			exp(handler,operand);
		}else{}
	} else{}
	SyntaxList.push_back(syn.get(syn.assstate));
	return tempIndex;
}

void syntacticPraser::readstate(errorHandler& handler)
{
	if (symbol == transfer.get(transfer.lparent)) {
		do {
			getsym();
			if (symbol == transfer.get(transfer.identifier)) {
				int findRes = table.find(content, typeList);
				if (findRes == -1)
					handler.newError(row,col,handler.undefinedName);
				if (findRes == 3 || findRes == 4)
					handler.newError(row, col, handler.cannotChangeValueOfConstant);
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("read");
				if (localVarMap.find(content) != localVarMap.end()) {
					if(localVarMap[content] == 1)
						intermediateCode[location].push_back("int");
					else
						intermediateCode[location].push_back("char");
				}
				else if(globalVarMap.find(content) != globalVarMap.end()){
					if (globalVarMap[content] == 1)
						intermediateCode[location].push_back("int");
					else
						intermediateCode[location].push_back("char");
				}
				intermediateCode[location].push_back(content);
				location++;
				/*if (localVarMap.find(content) != localVarMap.end()) {
					temp.clear();
					intermediateCode.push_back(temp);
					intermediateCode[location].push_back("saveStack");
					intermediateCode[location].push_back(content);
					intermediateCode[location].push_back("$v0");
					location++;
				}*/
				getsym();
			}else{}
		} while (symbol == transfer.get(transfer.comma));
	}else{}
	if (symbol != transfer.get(transfer.rparent)) {
		handler.newError(row, col, handler.shouldBeRparent);
	}
	else getsym();
	SyntaxList.push_back(syn.get(syn.readstate));
}

void syntacticPraser::writestate(errorHandler& handler)
{
	string operand;
	if (symbol == transfer.get(transfer.lparent)) {
		vector<string> temp;
		getsym();
		if (symbol == transfer.get(transfer.str)) {	
			intermediateCode.insert(intermediateCode.begin()+partition,temp);
			intermediateCode[partition].push_back("str");
			intermediateCode[partition].push_back("$str" + to_string(varNum));
			string newContent = "";
			for (int i = 0; i < content.length(); i++)
			{
				if (content[i] == '\\')
				{
					newContent = newContent + "\\\\";
				}
				else
				{
					newContent = newContent + content[i];
				}
			}
			intermediateCode[partition].push_back("\""+newContent+"\"");
			partition++;
			location++;
			varNum++;
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("printStr");
			intermediateCode[location].push_back("$str" + to_string(varNum-1));
			location++;
			getsym();
			SyntaxList.push_back(syn.get(syn.str));
			if (symbol == transfer.get(transfer.comma)) {
				getsym();
				if (exp(handler, operand)) {
					temp.clear();
					intermediateCode.push_back(temp);
					intermediateCode[location].push_back("printChar");
					intermediateCode[location].push_back(operand);
					location++;
				}
				else {
					temp.clear();
					intermediateCode.push_back(temp);
					intermediateCode[location].push_back("printInt");
					intermediateCode[location].push_back(operand);
					location++;
				}
			}
		}
		else {
			if (exp(handler, operand)) {
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("printChar");
				intermediateCode[location].push_back(operand);
				location++;
			}
			else {
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("printInt");
				intermediateCode[location].push_back(operand);
				location++;
			}
		}
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("lineFeed");
		location++;
	}
	if (symbol != transfer.get(transfer.rparent)) {
		handler.newError(row, col, handler.shouldBeRparent);
	}
	else getsym();
	SyntaxList.push_back(syn.get(syn.writestate));
}

void syntacticPraser::condition(errorHandler& handler)
{
	bool errmark = false;
	string operand1 = "", operand2 = "";
	string tempOp;
	if (exp(handler,operand1))
		errmark = true;
	if (symbol == transfer.get(transfer.less)
		|| symbol == transfer.get(transfer.le)
		|| symbol == transfer.get(transfer.greater)
		|| symbol == transfer.get(transfer.ge)
		|| symbol == transfer.get(transfer.ne)
		|| symbol == transfer.get(transfer.equal)) {
		tempOp = content;
		getsym();
		if (exp(handler,operand2))
			errmark = true;
	}
	if(errmark)
		handler.newError(row, col, handler.illegalTypesInCondition);
	vector<string> temp;
	intermediateCode.push_back(temp);
	intermediateCode[location].push_back("condition");
	intermediateCode[location].push_back(operand1);
	if (operand2 != "") {
		intermediateCode[location].push_back(tempOp);
		intermediateCode[location].push_back(operand2);
	}
	location++;
	SyntaxList.push_back(syn.get(syn.condition));
}

bool syntacticPraser::exp(errorHandler& handler,string &operand1)
{
	bool typeMark = true;//false means int
	int itemNum = 0;
	string tempOp = "";
	string operand2 = "";
	string varName = "\"t" + to_string(varNum);
	varNum++;
	if (symbol == transfer.get(transfer.plus)
		|| symbol == transfer.get(transfer.minus)) {
		tempOp = content;
		getsym();
	}
	bool firstmark = true;
	do {
		if (!firstmark) {
			typeMark = false;
			tempOp = content;
			getsym();
		}
		if (itemNum == 0) {
			operand1 = item(handler, typeMark);
			if (tempOp == "-") {
				vector<string> temp;
				intermediateCode.push_back(temp);
				intermediateCode[location].push_back("OpState");
				//intermediateCode[location].push_back("\"t" + to_string(varNum));
				intermediateCode[location].push_back(varName);
				intermediateCode[location].push_back("=");
				intermediateCode[location].push_back("0");
				intermediateCode[location].push_back("-");
				intermediateCode[location].push_back(operand1);
				location++;
				//varNum++;
				//operand1 = "\"t" + to_string(varNum - 1);
				operand1 = varName;
			}
		}
		else {
			operand2 = item(handler, typeMark);
		}
		itemNum++;
		firstmark = false;
		if (operand1 != "" && operand2 != "") {
			vector<string> temp;
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("OpState");
			//intermediateCode[location].push_back("\"t" + to_string(varNum));
			intermediateCode[location].push_back(varName);
			intermediateCode[location].push_back("=");
			//intermediateCode[location].push_back(varName);
			intermediateCode[location].push_back(operand1);
			intermediateCode[location].push_back(tempOp);
			intermediateCode[location].push_back(operand2);
			location++;
			//varNum++;
			//operand1 = "\"t" + to_string(varNum - 1);
			operand1 = varName;
		}
	} while (symbol == transfer.get(transfer.plus)
		||symbol == transfer.get(transfer.minus));
	SyntaxList.push_back(syn.get(syn.exp));
	return typeMark;
}

string syntacticPraser::item(errorHandler& handler,bool &mark)
{
	bool firstmark = true;
	int factorNum = 0;
	string varName = "\"t" + to_string(varNum);
	varNum++;
	string operand1 = "", operand2 = "",tempOp = "";
	do {
		if (!firstmark) {
			mark = false;
			tempOp = content;
			getsym();
		}
		if (factorNum == 0) {
			operand1 = factor(handler, mark);
		}
		else {
			operand2 = factor(handler, mark);
		}
		factorNum++;
		firstmark = false;
		if (operand1 != "" && operand2 != "") {
			vector<string> temp;
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("OpState");
			//intermediateCode[location].push_back("\"t" + to_string(varNum));
			intermediateCode[location].push_back(varName);
			intermediateCode[location].push_back("=");
			intermediateCode[location].push_back(operand1);
			intermediateCode[location].push_back(tempOp);
			intermediateCode[location].push_back(operand2);
			location++;
			//varNum++;
			//operand1 = "\"t" + to_string(varNum-1);
			operand1 = varName;
		}
	} while (symbol == transfer.get(transfer.multi)
		||symbol == transfer.get(transfer.div));
	SyntaxList.push_back(syn.get(syn.item));
	if(factorNum == 1)
		return operand1;
	return varName;
}

string syntacticPraser::factor(errorHandler& handler,bool &mark)
{
	string operand;
	if (symbol == transfer.get(transfer.identifier)) {
		std::string tempid = content;
		getsym();
		if (symbol == transfer.get(transfer.lbrack)) {
			getsym();
			if (exp(handler,operand))
				handler.newError(row, col, handler.indexOfArrayCanOnlyBeIntegerExpresion);
			if (symbol != transfer.get(transfer.rbrack)) {
				handler.newError(row, col, handler.shouldBeRbrack);
			}
			else getsym();
			int findRes = table.find(tempid, typeList);
			if (findRes == -1)
				handler.newError(row, col, handler.undefinedName);
			if (findRes != 2 && findRes != 4) { 
				mark = false; 
			}
			SyntaxList.push_back(syn.get(syn.factor));
			vector<string> temp;
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("assState");
			intermediateCode[location].push_back("\"t" + to_string(varNum));
			intermediateCode[location].push_back("=");
			intermediateCode[location].push_back(tempid);
			intermediateCode[location].push_back(operand);
			location++;
			varNum++;
			return ("\"t" + to_string(varNum - 1));
		}
		else if (symbol == transfer.get(transfer.lparent)) {
			getsym();
			vector<string> temp;
			/*intermediateCode.push_back(temp);
			intermediateCode[location].push_back("changeScope");
			intermediateCode[location].push_back(tempid);
			location++;*/
			vParaList(handler,tempid);
			if (symbol != transfer.get(transfer.rparent)) {
				handler.newError(row, col, handler.shouldBeRparent);
			}
			else getsym();
			SyntaxList.push_back(syn.get(syn.callre));
			int findRes = table.find(tempid, typeList);
			if (findRes == -1)
				handler.newError(row, col, handler.undefinedName);
			if (findRes == -2)
				handler.newError(row, col, handler.numberOfArgumentsNotMatch);
			if (findRes == -3)
				handler.newError(row, col, handler.typeOfArgumentsNotMatch);
			if (findRes != 6) mark = false;
			/*temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("release");
			location++;*/
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("call");
			intermediateCode[location].push_back(tempid);
			location++;
			temp.clear();
			intermediateCode.push_back(temp);
			intermediateCode[location].push_back("assState");
			intermediateCode[location].push_back("\"t" + to_string(varNum));
			intermediateCode[location].push_back("=");
			intermediateCode[location].push_back("$v0");
			location++;
			varNum++;
			return ("\"t" + to_string(varNum - 1));
		}
		else {
			int findRes = table.find(tempid, typeList);
			if (findRes == -1)
				handler.newError(row, col, handler.undefinedName);
			if (findRes != 2 && findRes != 4) { 
				mark = false; 
				//cout << tempid << endl;
			}
			SyntaxList.push_back(syn.get(syn.factor));
			if (localConstMap.find(tempid) != localConstMap.end()) {
				return localConstMap[tempid];
			}
			else if (localVarMap.find(tempid) == localVarMap.end()
				&&globalConstMap.find(tempid) != globalConstMap.end()) {
				return globalConstMap[tempid];
			}
			return tempid;
		}
	}
	else if (symbol == transfer.get(transfer.lparent)) {
		getsym();
		if (exp(handler, operand)) {}
		if (symbol != transfer.get(transfer.rparent)) {
			handler.newError(row, col, handler.shouldBeRparent);
		}
		else getsym();
		mark = false;
		SyntaxList.push_back(syn.get(syn.factor));
		return operand;
	}
	else if (symbol == transfer.get(transfer.plus)
		|| symbol == transfer.get(transfer.minus)) {
		string tempOp = content;
		getsym();
		string tempInt = content;
		if (symbol == transfer.get(transfer.intconst)) {	
			getsym();
			SyntaxList.push_back(syn.get(syn.unsignedInt));
			SyntaxList.push_back(syn.get(syn.integer));
		}
		mark = false;
		SyntaxList.push_back(syn.get(syn.factor));
		return (tempOp + tempInt);
	}  
	else if (symbol == transfer.get(transfer.intconst)) {
		string tempInt = content;
		getsym();
		SyntaxList.push_back(syn.get(syn.unsignedInt));
		SyntaxList.push_back(syn.get(syn.integer));
		mark = false;
		SyntaxList.push_back(syn.get(syn.factor));
		return tempInt;
	}
	else if (symbol == transfer.get(transfer.charconst)) {
		string tempChar = content;
		getsym();
		SyntaxList.push_back(syn.get(syn.factor));
		return "'"+tempChar+"'";
	}
	else { return ""; }
}

void syntacticPraser::vParaList(errorHandler& handler,string funcName)
{
	typeList.clear();
	vector<string> tempList;
	if (symbol == transfer.get(transfer.rparent)) {
		SyntaxList.push_back(syn.get(syn.vparaList));
	}
	else {
		bool firstmark = true;
		string operand;
		do {
			if (!firstmark) {
				getsym();
			}
			if (symbol == transfer.get(transfer.semicolon)) break;
			if (exp(handler,operand)) {
				typeList.push_back(table.character);
			}
			else {
				typeList.push_back(table.integer);
			}
			firstmark = false;
			tempList.push_back(operand);
		} while (symbol == transfer.get(transfer.comma));
		SyntaxList.push_back(syn.get(syn.vparaList));
	}
	vector<string> temp;
	intermediateCode.push_back(temp);
	intermediateCode[location].push_back("changeScope");
	intermediateCode[location].push_back(funcName);
	location++;
	for (auto iter = tempList.begin(); iter != tempList.end(); iter++)
	{
		temp.clear();
		intermediateCode.push_back(temp);
		intermediateCode[location].push_back("push");
		intermediateCode[location].push_back(*iter);
		location++;
	}
}

void syntacticPraser::getsym()
{
	if (index != 0) { SyntaxList.push_back(symbol + " " + content); }
	if (index < length) {
		symbol = TokenList[index].first;
		content = TokenList[index].second;
		row = PositionList[index].first;
		col = PositionList[index].second;
		index++;
	}
	//std::cout << symbol << " "<<index<<std::endl;
}

void syntacticPraser::outputSyn()
{
	outputHandler output = outputHandler(SyntaxList,targetfile);
}

void syntacticPraser::outputMid()
{
	vector<string> MidCode;
	for (int i = 0; i < intermediateCode.size(); i++) {
		string tempMidCode = "";
		for (int j = 0; j < intermediateCode[i].size(); j++) {
			tempMidCode = tempMidCode + intermediateCode[i][j] + " ";
		}
		MidCode.push_back(tempMidCode);
	}
	//outputHandler output = outputHandler(MidCode, "Mid_"+targetfile);
	outputHandler output = outputHandler(MidCode, "优化前.txt");
}

