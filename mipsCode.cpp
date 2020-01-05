#include "mipsCode.h"
#include <iostream>
mipsCode::mipsCode(vector<vector<string>> midCode, string filename,bool opMark)
{
	this->midCode = midCode;
	this->filename = filename;
	inlineOptimal();
	activeVariable();
	conflictMap();
	allocGlobalRge();
	addGlobalReg();
	map<string, int> firstScope;
	localVarAdd.push_back(make_pair(firstScope,0));
	//stackPosition = 0x10014000;
	dataPosition = 0x10010000;
	heapPosition = 0;
	scopeNum = 0;
	nowLabel = "";
	saveMark = true;
	initRegisters();
	if (opMark)
		transfer(newMidCode);
	else
		transfer(midCode);
	output();
}

void mipsCode::inlineOptimal()//inline
{
	for (int index = 0; index < midCode.size(); index++)//get no recursion funcs
	{
		if (midCode[index][0] == "funcLabel")
		{
			string funcName = midCode[index][1].substr(0,midCode[index][1].size()-1);
			bool recursionMark = false;
			while (midCode[index][0] != "back")
			{
				index++;
				if (midCode[index][0] == "call")
				{
					if (midCode[index][1] == funcName)
					{
						recursionMark = true;
					}
				}
			}
			if (!recursionMark)
			{
				noRecursionFuncs[funcName] = 1;
				cout << funcName << endl;
			}
		}
	}
	for (int index = 0; index < midCode.size(); index++)
	{
		if (midCode[index][0] == "funcLabel")
		{
			string funcName = midCode[index][1].substr(0, midCode[index][1].size() - 1);
			if (noRecursionFuncs.find(funcName) != noRecursionFuncs.end())
			{
				vector<vector<string>> tempFuncMidCode;
				map<string, int> localVarMap;
				//int returnMark = 0;
				while (midCode[index][0] != "back")
				{
					index++;
					for (int j = 1; j < midCode[index].size(); j++)
					{
						if (localVarMap.find(midCode[index][j]) != localVarMap.end())
						{
							midCode[index][j] = "$" + midCode[index][j] + "_" + funcName;
						}
					}
					if (midCode[index][0] == "para")
					{
						vector<string> temp;
						temp.push_back("para");
						temp.push_back(midCode[index][1]);
						temp.push_back("$" + midCode[index][2] + "_" + funcName);
						tempFuncMidCode.push_back(temp);
						localVarMap[midCode[index][2]] = 1;
					}
					else if (midCode[index][0] == "localVar")
					{
						vector<string> temp;
						temp.push_back("localVar");
						temp.push_back(midCode[index][1]);
						temp.push_back("$" + midCode[index][2] + "_" + funcName);
						temp.push_back(midCode[index][3]);
						tempFuncMidCode.push_back(temp);
						localVarMap[midCode[index][2]] = 1;
					}
					else if (midCode[index][0] == "ret")
					{
						vector<string> temp;
						temp.push_back("inlineRet");
						if (midCode[index].size() == 2)
						{
							temp.push_back(midCode[index][1]);
						}
						else
						{
							temp.push_back("");
						}
						temp.push_back(funcName+"_end");
						tempFuncMidCode.push_back(temp);
					}
					else if (midCode[index][0] == "back")
					{
						vector<string> temp;
						temp.push_back("funcEndLabel");
						temp.push_back(funcName + "_end:" );
						tempFuncMidCode.push_back(temp);
					}
					else if (midCode[index][0] == "changeScope")
					{
						//returnMark = 0;
						string tempFuncName = midCode[index][1];//the name of the function which is called
						if (noRecursionFuncs.find(tempFuncName) != noRecursionFuncs.end())
						{
							int i = 0;
							vector<string> temp;
							while (midCode[index][0] != "call")
							{
								index++;
								for (int j = 1; j < midCode[index].size(); j++)
								{
									if (localVarMap.find(midCode[index][j]) != localVarMap.end())
									{
										midCode[index][j] = "$" + midCode[index][j] + "_" + funcName;
									}
								}
								if (midCode[index][0] == "push")
								{
									temp.clear();
									temp.push_back("assState");
									temp.push_back(inlineFuncs[tempFuncName][i][2]);
									temp.push_back("=");
									temp.push_back(midCode[index][1]);
									tempFuncMidCode.push_back(temp);
									i++;
								}
							}
							/*for (; i < inlineFuncs[tempFuncName].size(); i++)
							{
								tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
							}*/
							for (; i < inlineFuncs[tempFuncName].size(); i++){
								if (inlineFuncs[tempFuncName][i][0] == "inlineRet")
								{
									inlineFuncs[tempFuncName][i][2] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][2];
									//if (inlineFuncs[tempFuncName][i][1] != "") returnMark++;
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
								else if (inlineFuncs[tempFuncName][i][0] == "funcEndLabel")
								{
									inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
								else if (inlineFuncs[tempFuncName][i][0] == "label")
								{
									inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
								else if (inlineFuncs[tempFuncName][i][0] == "jump")
								{
									inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
								else if (inlineFuncs[tempFuncName][i][0] == "branch")
								{
									inlineFuncs[tempFuncName][i][2] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][2];
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
								else
								{
									tempFuncMidCode.push_back(inlineFuncs[tempFuncName][i]);
								}
							}
						}
						else
						{
							tempFuncMidCode.push_back(midCode[index]);
						}
					}
					else
					{
						/*if (returnMark == 1 && midCode[index][0] == "assState" && midCode[index][3] == "$v0"
							&& midCode[index - 1][0] == "funcEndLabel" && midCode[index - 2][0] == "inlineRet")
						{
							returnMark = 0;
							tempFuncMidCode[tempFuncMidCode.size()-2][1] = 
						}*/
						tempFuncMidCode.push_back(midCode[index]);
					}
				}
				inlineFuncs[funcName] = tempFuncMidCode;
			}
			else
			{
				map<string, int> localVarMap;
				while (midCode[index][0] != "back")
				{
					index++;
					for (int j = 1; j < midCode[index].size(); j++)
					{
						if (localVarMap.find(midCode[index][j]) != localVarMap.end())
						{
							midCode[index][j] = "$" + midCode[index][j] + "_" + funcName;
							//cout << midCode[index][j] << endl;
						}
						//cout << midCode[index][1] << endl;
					}
					if (midCode[index][0] == "para")
					{
						localVarMap[midCode[index][2]] = 1;
						midCode[index][2] = "$" + midCode[index][2] + "_" + funcName;
					}
					else if (midCode[index][0] == "localVar")
					{
						localVarMap[midCode[index][2]] = 1;
						midCode[index][2] = "$" + midCode[index][2] + "_" + funcName;
					}
				}
			}
		}
	}
	int nowFuncPosition = 0;
	for (int index = 0; index < midCode.size(); index++)
	{
		if (midCode[index][0] == "funcLabel")
		{
			string funcName = midCode[index][1].substr(0, midCode[index][1].size() - 1);
			if (noRecursionFuncs.find(funcName) != noRecursionFuncs.end())
			{
				while (midCode[index][0] != "back")
				{
					index++;
					//cout << index << midCode[index][0] << endl;
				}
				if (midCode[index][0] == "back") continue;
			}
			else
			{
				newMidCode.push_back(midCode[index]);
				index++;
				while (midCode[index][0] == "para")
				{
					newMidCode.push_back(midCode[index]);
					index++;
				}
				nowFuncPosition = newMidCode.size();
			}
		}
		if (midCode[index][0] == "mainLabel")
		{
			nowFuncPosition = newMidCode.size() + 1;
		}
		if (midCode[index][0] != "changeScope")
		{
			newMidCode.push_back(midCode[index]);
		}
		else
		{
			string tempFuncName = midCode[index][1];//the name of the function which is called
			if (noRecursionFuncs.find(tempFuncName) != noRecursionFuncs.end())
			{
				int i = 0;
				vector<string> temp;
				while (midCode[index][0] != "call")
				{
					index++;
					if (midCode[index][0] == "push")
					{
						temp.clear();
						temp.push_back("assState");
						temp.push_back(inlineFuncs[tempFuncName][i][2]);
						temp.push_back("=");
						temp.push_back(midCode[index][1]);
						newMidCode.push_back(temp);
						i++;
					}
				}
				for (; i < inlineFuncs[tempFuncName].size(); i++)
				{
					if (inlineFuncs[tempFuncName][i][0] == "localVar")
					{
						//inlineFuncs[tempFuncName][i][2] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][2];
						newMidCode.insert(newMidCode.begin() + nowFuncPosition, inlineFuncs[tempFuncName][i]);
					}
					else if (inlineFuncs[tempFuncName][i][0] == "inlineRet")
					{
						inlineFuncs[tempFuncName][i][2] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][2];
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
					else if (inlineFuncs[tempFuncName][i][0] == "funcEndLabel")
					{
						inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
					else if (inlineFuncs[tempFuncName][i][0] == "label")
					{
						inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
					else if (inlineFuncs[tempFuncName][i][0] == "jump")
					{
						inlineFuncs[tempFuncName][i][1] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][1];
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
					else if (inlineFuncs[tempFuncName][i][0] == "branch")
					{
						inlineFuncs[tempFuncName][i][2] = "_" + to_string(index) + inlineFuncs[tempFuncName][i][2];
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
					else
					{
						newMidCode.push_back(inlineFuncs[tempFuncName][i]);
					}
						
				}
			}
			else
			{
				newMidCode.push_back(midCode[index]);
			}
		}
		//cout << midCode[index][0] <<index<< endl;
	}
	outputNewMidCode();
}

void mipsCode::outputNewMidCode()
{
	vector<string> MidCode;
	for (int i = 0; i < newMidCode.size(); i++)
	{
		string tempMidCode = "";
		for (int j = 0; j < newMidCode[i].size(); j++)
		{
			tempMidCode = tempMidCode + newMidCode[i][j] + " ";
		}
		MidCode.push_back(tempMidCode);
	}
	outputHandler output = outputHandler(MidCode, "优化后.txt");
}

void mipsCode::activeVariable()
{
	for (int index = 0; index < newMidCode.size(); index++)
	{
		if (newMidCode[index][0] == "funcLabel")
		{
			scopeNum++;
			map<string, vector<map<string, int>>> tempScopeSet;
			scopeSets[scopeNum] = tempScopeSet;
			nowLabel = newMidCode[index][1].substr(0, newMidCode[index][1].size() - 1);
			vector<map<string, int>> tempBlockSet;
			scopeSets[scopeNum][nowLabel] = tempBlockSet;
			addMaps(nowLabel);
		}
		else if (newMidCode[index][0] == "mainLabel")
		{
			scopeNum = 0;
			map<string, vector<map<string, int>>> tempScopeSet;
			scopeSets[scopeNum] = tempScopeSet;
			nowLabel = newMidCode[index][1].substr(0, newMidCode[index][1].size() - 1);
			vector<map<string, int>> tempBlockSet;
			scopeSets[scopeNum][nowLabel] = tempBlockSet;
			addMaps(nowLabel);
		}
		else if (newMidCode[index][0] == "label")
		{
			string temp = newMidCode[index][1].substr(0, newMidCode[index][1].size() - 1);
			string lastLabel = nowLabel;
			scopeSets[scopeNum][lastLabel][1][temp] = 1;
			nowLabel = temp;
			if (scopeSets[scopeNum].find(nowLabel) == scopeSets[scopeNum].end())
			{
				vector<map<string, int>> tempBlockSet;
				scopeSets[scopeNum][nowLabel] = tempBlockSet;
				addMaps(nowLabel);
			}
			scopeSets[scopeNum][nowLabel][0][lastLabel] = 1;
		}
		else if (newMidCode[index][0] == "jump" && newMidCode[index][1] != "main")
		{
			scopeSets[scopeNum][nowLabel][1][newMidCode[index][1]] = 1;
			if (scopeSets[scopeNum].find(newMidCode[index][1]) == scopeSets[scopeNum].end())
			{
				vector<map<string, int>> tempBlockSet;
				scopeSets[scopeNum][newMidCode[index][1]] = tempBlockSet;
				addMaps(newMidCode[index][1]);
			}
			scopeSets[scopeNum][newMidCode[index][1]][0][nowLabel] = 1;
		}
		else if (newMidCode[index][0] == "branch")
		{
			//cout << scopeSets[scopeNum][nowLabel].size() << endl;
			scopeSets[scopeNum][nowLabel][1][newMidCode[index][2]] = 1;
			if (scopeSets[scopeNum].find(newMidCode[index][2]) == scopeSets[scopeNum].end())
			{
				vector<map<string, int>> tempBlockSet;
				scopeSets[scopeNum][newMidCode[index][2]] = tempBlockSet;
				addMaps(newMidCode[index][2]);
			}
			scopeSets[scopeNum][newMidCode[index][2]][0][nowLabel] = 1;
			for (auto pre = scopeSets[scopeNum][nowLabel][0].begin(); pre != scopeSets[scopeNum][nowLabel][0].end(); pre++)
			{
				if (scopeSets[scopeNum].find(pre->first) == scopeSets[scopeNum].end())
				{
					vector<map<string, int>> tempBlockSet;
					scopeSets[scopeNum][pre->first] = tempBlockSet;
					addMaps(pre->first);
				}
				scopeSets[scopeNum][pre->first][1][newMidCode[index][2]] = 1;
				scopeSets[scopeNum][newMidCode[index][2]][0][pre->first] = 1;
			}
		}
		else if (newMidCode[index][0] == "assState")
		{
			if (newMidCode[index].size() == 4)
			{
				if (newMidCode[index][3][0] == '\''||newMidCode[index][3][0] == '-' || newMidCode[index][3][0] == '+' ||
					('0' <= newMidCode[index][3][0] && newMidCode[index][3][0] <= '9'))
				{
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
				else
				{
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][3])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][3]] = 1;
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
			}
			else
			{
				if (newMidCode[index][2] == "=")
				{
					//if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						//== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						//scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
					if (newMidCode[index][4][0] == '\'' || newMidCode[index][4][0] == '-' || newMidCode[index][4][0] == '+' ||
						('0' <= newMidCode[index][4][0] && newMidCode[index][4][0] <= '9'))
					{
					}else{
						if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][4])
							== scopeSets[scopeNum][nowLabel][2].end())//not define before use
							scopeSets[scopeNum][nowLabel][3][newMidCode[index][4]] = 1;
					}
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
				else
				{
					if (newMidCode[index][4][0] == '\'' || newMidCode[index][4][0] == '-' || newMidCode[index][4][0] == '+' ||
						('0' <= newMidCode[index][4][0] && newMidCode[index][4][0] <= '9'))
					{
					}
					else
					{
						if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][4])
							== scopeSets[scopeNum][nowLabel][2].end())//not define before use
							scopeSets[scopeNum][nowLabel][3][newMidCode[index][4]] = 1;
					}
					if (newMidCode[index][2][0] == '\'' || newMidCode[index][2][0] == '-' || newMidCode[index][2][0] == '+' ||
						('0' <= newMidCode[index][2][0] && newMidCode[index][2][0] <= '9'))
					{
					}else{
						if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][2])
							== scopeSets[scopeNum][nowLabel][2].end())//not define before use
							scopeSets[scopeNum][nowLabel][3][newMidCode[index][2]] = 1;
					}
				}
			}
		}
		else if (newMidCode[index][0] == "OpState")
		{
			if (newMidCode[index][3][0] == '\''||newMidCode[index][3][0] == '-' || newMidCode[index][3][0] == '+' ||
				('0' <= newMidCode[index][3][0] && newMidCode[index][3][0] <= '9'))
			{
				if (newMidCode[index][5][0] == '\''||newMidCode[index][5][0] == '-' || newMidCode[index][5][0] == '+' ||
					('0' <= newMidCode[index][5][0] && newMidCode[index][5][0] <= '9'))
				{
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
				else
				{
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][5])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][5]] = 1;
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
			}
			else
			{
				if (newMidCode[index][5][0] == '\''||newMidCode[index][5][0] == '-' || newMidCode[index][5][0] == '+' ||
					('0' <= newMidCode[index][5][0] && newMidCode[index][5][0] <= '9'))
				{
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][3])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][3]] = 1;
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
				else
				{
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][5])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][5]] = 1;
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][3])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][3]] = 1;
					if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][1])
						== scopeSets[scopeNum][nowLabel][3].end())//not use before define
						scopeSets[scopeNum][nowLabel][2][newMidCode[index][1]] = 1;
				}
			}
		}
		else if (newMidCode[index][0] == "condition")
		{
			if (newMidCode[index][1][0] == '\''||newMidCode[index][1][0] == '-' || newMidCode[index][1][0] == '+' ||
				('0' <= newMidCode[index][1][0] && newMidCode[index][1][0] <= '9'))
			{
			}
			else
			{
				if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][1])
					== scopeSets[scopeNum][nowLabel][2].end())//not define before use
					scopeSets[scopeNum][nowLabel][3][newMidCode[index][1]] = 1;
			}
			if (newMidCode[index].size() == 4)
			{
				if (newMidCode[index][3][0] == '\'' || newMidCode[index][3][0] == '-' || newMidCode[index][3][0] == '+' ||
					('0' <= newMidCode[index][3][0] && newMidCode[index][3][0] <= '9'))
				{
				}
				else
				{
					if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][3])
						== scopeSets[scopeNum][nowLabel][2].end())//not define before use
						scopeSets[scopeNum][nowLabel][3][newMidCode[index][3]] = 1;
				}
			}
		}
		else if(newMidCode[index][0] == "read"){
			if (scopeSets[scopeNum][nowLabel][3].find(newMidCode[index][2])
				== scopeSets[scopeNum][nowLabel][3].end())//not use before define
				scopeSets[scopeNum][nowLabel][2][newMidCode[index][2]] = 1;
		}
		else if(newMidCode[index][0] == "printInt"||newMidCode[index][0] == "printChar"){
			if (newMidCode[index][1][0] == '\''||newMidCode[index][1][0] == '-' || newMidCode[index][1][0] == '+' ||
				('0' <= newMidCode[index][1][0] && newMidCode[index][1][0] <= '9')){ 
			}
			else
			{
				if (scopeSets[scopeNum][nowLabel][2].find(newMidCode[index][1])
					== scopeSets[scopeNum][nowLabel][2].end())//not define before use
					scopeSets[scopeNum][nowLabel][3][newMidCode[index][1]] = 1;
			}
		}
		//cout << index << newMidCode[index][0] << endl;
	}
	/*for (auto iter = scopeSets.begin(); iter != scopeSets.end(); iter++)
	{
		cout << "now scopeNum is " << iter->first << endl;
		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			cout << "now block is " << iter2->first << endl;
			cout << "precursor: " << endl;
			for (auto iter3 = iter2->second[0].begin(); iter3 != iter2->second[0].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "succeed: " << endl;
			for (auto iter3 = iter2->second[1].begin(); iter3 != iter2->second[1].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl; 
			cout << "def: " << endl;
			for (auto iter3 = iter2->second[2].begin(); iter3 != iter2->second[2].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl; 
			cout << "use: " << endl;
			for (auto iter3 = iter2->second[3].begin(); iter3 != iter2->second[3].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl; 
			cout << "in: " << endl;
			for (auto iter3 = iter2->second[4].begin(); iter3 != iter2->second[4].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl; 
			cout << "out: " << endl;
			for (auto iter3 = iter2->second[5].begin(); iter3 != iter2->second[5].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
		}
	}*/
	bool changeMark = false;
	do
	{
		changeMark = false;
		for (auto block = scopeSets[0].begin(); block != scopeSets[0].end(); block++)
		{
			for (auto succeedBlock = block->second[1].begin(); succeedBlock != block->second[1].end(); succeedBlock++)
			{
				// update out
				for (auto inSucceed = scopeSets[0][succeedBlock->first][4].begin();
					inSucceed != scopeSets[0][succeedBlock->first][4].end(); inSucceed++)
				{//inSucced is the var name of succeed block
					if (block->second[5].find(inSucceed->first) == block->second[5].end())
					{
						changeMark = true;
						block->second[5][inSucceed->first] = 1;
					}
				}
				
			}
			// update in
			map<string,int> tempIn;
			for (auto out = block->second[5].begin(); out != block->second[5].end(); out++)
			{
				if (block->second[2].find(out->first) == block->second[2].end())
				{
					tempIn[out->first] = 1;
				}
			}
			for (auto use = block->second[3].begin(); use != block->second[3].end(); use++)
			{
				tempIn[use->first] = 1;
			}
			for (auto tempInIter = tempIn.begin();tempInIter!=tempIn.end();tempInIter++)
			{
				if (block->second[4].find(tempInIter->first) == block->second[4].end())
				{
					changeMark = true;
					block->second[4][tempInIter->first] = 1;
				}
			}
		}
	} while (changeMark);
	/*cout << "finish" << endl;
	for (auto iter = scopeSets.begin(); iter != scopeSets.end(); iter++)
	{
		cout << "now scopeNum is " << iter->first << endl;
		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			cout << "now block is " << iter2->first << endl;
			cout << "precursor: " << endl;
			for (auto iter3 = iter2->second[0].begin(); iter3 != iter2->second[0].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "succeed: " << endl;
			for (auto iter3 = iter2->second[1].begin(); iter3 != iter2->second[1].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "def: " << endl;
			for (auto iter3 = iter2->second[2].begin(); iter3 != iter2->second[2].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "use: " << endl;
			for (auto iter3 = iter2->second[3].begin(); iter3 != iter2->second[3].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "in: " << endl;
			for (auto iter3 = iter2->second[4].begin(); iter3 != iter2->second[4].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
			cout << "out: " << endl;
			for (auto iter3 = iter2->second[5].begin(); iter3 != iter2->second[5].end(); iter3++)
			{
				cout << iter3->first << "  ";
			}
			cout << endl;
		}
	}*/

}

void mipsCode::addMaps(string label)
{
	for (int i = 0; i < 6; i++)
	{
		map<string, int> emptyMap;
		scopeSets[scopeNum][label].push_back(emptyMap);
	}
	//cout << "________" << endl;
	//cout << label << endl;
	//cout << scopeSets[scopeNum][label].size() << endl;
}

void mipsCode::conflictMap()
{
	for (auto block = scopeSets[0].begin(); block != scopeSets[0].end(); block++)
	{
		for (auto def = block->second[2].begin(); def != block->second[2].end(); def++)
		{
			if (conflictVars.find(def->first) == conflictVars.end())
			{
				conflictVars[def->first] = 1;
			}
			else
			{
				conflictVars[def->first]++;
			}
		}
		for (auto use = block->second[3].begin(); use != block->second[3].end(); use++)
		{
			if (conflictVars.find(use->first) == conflictVars.end())
			{
				conflictVars[use->first] = 1;
			}
			else
			{
				conflictVars[use->first]++;
			}
		}
	}
	for (auto var = conflictVars.begin(); var != conflictVars.end(); var++)
	{
		if(var->first!= "$v0")
			conflictVarPair.push_back(make_pair(var->first, var->second));
	}
	//cout << conflictVarPair.size() << endl;
	std::sort(conflictVarPair.begin(), conflictVarPair.end(),mySort);
	/*for (auto varPair = conflictVarPair.begin(); varPair != conflictVarPair.end(); varPair++)
	{
		cout << varPair->first << "\t" << varPair->second << endl;
	}*/
}

int mipsCode::mySort(const pair<string, int> &var1,const pair<string, int> &var2)
{
	return var1.second > var2.second;
}

void mipsCode::allocGlobalRge()
{
	for (int i = 0; i < 8 && i < conflictVarPair.size(); i++)
	{
		globalReg[conflictVarPair[i].first] = "$s" + to_string(i);
	}
	for (int i = 0; i < 2 && i < conflictVarPair.size()-8; i++)
	{
		globalReg[conflictVarPair[i+8].first] = "$t" + to_string(i);
	}
	/*for (auto a = globalReg.begin(); a != globalReg.end(); a++)
	{
		cout << a->first << " " << a->second << endl;
	}*/
}

void mipsCode::addGlobalReg()
{
	for (auto globalVar = globalReg.begin(); globalVar != globalReg.end(); globalVar++)
	{
		variables[globalVar->first] = globalVar->second;
	}
}

void mipsCode::initRegisters()
{
	//registers["$t0"] = "";
	//registers["$t1"] = "";
	registers["$t2"] = "";
	registers["$t3"] = "";
	registers["$t4"] = "";
	registers["$t5"] = "";
	registers["$t6"] = "";
	registers["$t7"] = "";
	registers["$t8"] = "";
	registers["$t9"] = "";
	registers["$v1"] = "";
	registers["$a3"] = "";
	/*registers["$s0"] = "";
	registers["$s1"] = "";
	registers["$s2"] = "";
	registers["$s3"] = "";
	registers["$s4"] = "";
	registers["$s5"] = "";
	registers["$s6"] = "";
	registers["$s7"] = "";*/
	//freeRegs.push_back("$t0");
	//freeRegs.push_back("$t1");
	freeRegs.push_back("$t2");
	freeRegs.push_back("$t3");
	freeRegs.push_back("$t4");
	freeRegs.push_back("$t5");
	freeRegs.push_back("$t6");
	freeRegs.push_back("$t7");
	freeRegs.push_back("$t8");
	freeRegs.push_back("$t9");
	freeRegs.push_back("$v1");
	freeRegs.push_back("$a3");
	/*freeRegs.push_back("$s0");
	freeRegs.push_back("$s1");
	freeRegs.push_back("$s2");
	freeRegs.push_back("$s3");
	freeRegs.push_back("$s4");
	freeRegs.push_back("$s5");
	freeRegs.push_back("$s6");
	freeRegs.push_back("$s7");*/
}

void mipsCode::clearRegisters()
{
	//registers["$t0"] = "";
	//registers["$t1"] = "";
	registers["$t2"] = "";
	registers["$t3"] = "";
	registers["$t4"] = "";
	registers["$t5"] = "";
	registers["$t6"] = "";
	registers["$t7"] = "";
	registers["$t8"] = "";
	registers["$t9"] = "";
	registers["$v1"] = "";
	registers["$a3"] = "";
	/*registers["$s0"] = "";
	registers["$s1"] = "";
	registers["$s2"] = "";
	registers["$s3"] = "";
	registers["$s4"] = "";
	registers["$s5"] = "";
	registers["$s6"] = "";
	registers["$s7"] = "";*/
}

void mipsCode::output()
{
	outputHandler output = outputHandler(mipsCodes,filename);
}

void mipsCode::transfer(vector<vector<string>> midCode)
{
	for (int index = 0; index < midCode.size(); index++) {
		if (midCode[index][0] == "var") {
			int size = stoi(midCode[index][3]) * 4;
			variableAdd[midCode[index][2]] = heapPosition;
			heapPosition += size;
			mipsCodes.push_back(midCode[index][2] + ":.space " + to_string(size));
		}
		else if (midCode[index][0] == "jump") {
			mipsCodes.push_back("j " +midCode[index][1]);
		}
		else if (midCode[index][0] == "label") {
			mipsCodes.push_back(midCode[index][1]);
			nowLabel = midCode[index][1].substr(0, midCode[index][1].size() - 1);
		}
		else if (midCode[index][0] == "funcLabel")
		{
			map<string, int> newScope;
			localVarAdd.push_back(make_pair(newScope, -4));
			scopeNum++;
			funcScope[midCode[index][1]] = scopeNum;
			mipsCodes.push_back(midCode[index][1]);
		}
		else if (midCode[index][0] == "mainLabel")
		{
			scopeNum = 0;
			mipsCodes.push_back(midCode[index][1]);
			nowLabel = "main";
		}
		else if (midCode[index][0] == "assState") {
			
			if (variableAdd.find(midCode[index][1]) == variableAdd.end()
				&&localVarAdd[scopeNum].first.find(midCode[index][1]) == localVarAdd[scopeNum].first.end())
			{
				localVarAdd[scopeNum].second -= 4;
				localVarAdd[scopeNum].first[midCode[index][1]] = localVarAdd[scopeNum].second;
			}
			if (midCode[index][2] == "=") {	// var = **
				dirtyMap[midCode[index][1]] = 1;
				if (midCode[index][3] == "$v0")
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], false);
						}
						else
						{
							allocBusyReg(midCode[index][1], false);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					mipsCodes.push_back("move " + variables[midCode[index][1]]
						+ ", $v0");
				}
				else if (midCode[index][3][0] == '\'') {
					if (variables.find(midCode[index][1]) == variables.end()) {
						if (!freeRegs.empty()) {
							allocFreeReg(midCode[index][1],false);
						}
						else {
							allocBusyReg(midCode[index][1],false);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					mipsCodes.push_back("la " + variables[midCode[index][1]] 
								+ ", " + midCode[index][3]);
				}
				else if (midCode[index][3][0] == '-' || midCode[index][3][0] == '+' ||
					('0' <= midCode[index][3][0] && midCode[index][3][0] <= '9')) {
					if (variables.find(midCode[index][1]) == variables.end()) {
						//cout << midCode[index][1] << endl;
						if (!freeRegs.empty()) {
							allocFreeReg(midCode[index][1],false);
						}
						else {
							allocBusyReg(midCode[index][1],false);	
						}
						
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					mipsCodes.push_back("li " + variables[midCode[index][1]]
								+ ", " + midCode[index][3]);
				}
				//else if (midCode[index][3][0] == '\"') {
				else {
					if (midCode[index].size() == 4 && midCode[index][1] != midCode[index][3])
					{

						if (variables.find(midCode[index][3]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][3], true);
							}
							else
							{
								allocBusyReg(midCode[index][3], true);
							}
						}
						else
						{
							swap(variables[midCode[index][3]]);
						}
						if (variables.find(midCode[index][1]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][1], false);
							}
							else
							{
								allocBusyReg(midCode[index][1], false);
							}
						}
						else
						{
							swap(variables[midCode[index][1]]);
						}

						//cout << midCode[index][1] << " " << midCode[index][3] << endl;
						//cout << variables[midCode[index][1]] << " " << variables[midCode[index][3]] << endl;
						mipsCodes.push_back("move " + variables[midCode[index][1]]
							+ ", " + variables[midCode[index][3]]);

					}
					else {// var = array[*]
						if (variables.find(midCode[index][1]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][1], false);
							}
							else
							{
								allocBusyReg(midCode[index][1], false);
							}
						}
						else
						{
							swap(variables[midCode[index][1]]);
						}
						if (midCode[index][4][0] == '\'' || midCode[index][4][0] == '-' || midCode[index][4][0] == '+' ||
							('0' <= midCode[index][4][0] && midCode[index][4][0] <= '9'))
						{
							int position;
							if (midCode[index][4][0] == '+' ||
								('0' <= midCode[index][4][0] && midCode[index][4][0] <= '9'))
							{
								position = stoi(midCode[index][4]) * 4;
							}
							else if (midCode[index][4][0] == '\'')
							{
								position = (midCode[index][4][1]) * 4;
							}
							if (localVarAdd[scopeNum].first.find(midCode[index][3]) != localVarAdd[scopeNum].first.end())
								mipsCodes.push_back("lw "+ variables[midCode[index][1]] +", " + to_string(position + localVarAdd[scopeNum].first[midCode[index][3]]) + "($sp)");
							else
								//mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", " + to_string(position + variableAdd[midCode[index][3]]) + "($0)");
								mipsCodes.push_back("lw "+ variables[midCode[index][1]] +", " + midCode[index][3] + "+" + to_string(position));
						}
						else
						{
							if (midCode[index][4] == "$v0")
							{
								if (localVarAdd[scopeNum].first.find(midCode[index][3]) != localVarAdd[scopeNum].first.end())
								{
									mipsCodes.push_back("mul $a1, $v0, 4");
									//mipsCodes.push_back("addu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][3]]));
									mipsCodes.push_back("addiu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][3]]));
									mipsCodes.push_back("addu $a1, $a1, $sp");
									mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", 0($a1)");
								}
								else
								{
									mipsCodes.push_back("mul $a1, $v0, 4");
									//mipsCodes.push_back("addu $a1, $a1, " + to_string(variableAdd[midCode[index][3]]));
									mipsCodes.push_back("addiu $a1, $a1, " + to_string(variableAdd[midCode[index][3]]));
									//mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", 0($a1)");
									mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", "+midCode[index][3]+"($a1)");
								}
							}
							else
							{
								if (variables.find(midCode[index][4]) == variables.end())
								{
									if (!freeRegs.empty())
									{
										allocFreeReg(midCode[index][4], true);
									}
									else
									{
										allocBusyReg(midCode[index][4], true);
									}
								}
								else
								{
									swap(variables[midCode[index][4]]);
								}
								if (localVarAdd[scopeNum].first.find(midCode[index][3]) != localVarAdd[scopeNum].first.end())
								{
									mipsCodes.push_back("mul $a1, " + variables[midCode[index][4]] + ", 4");
									//mipsCodes.push_back("addu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][3]]));
									mipsCodes.push_back("addiu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][3]]));
									mipsCodes.push_back("addu $a1, $a1, $sp");
									mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", 0($a1)");
								}
								else
								{
									mipsCodes.push_back("mul $a1, " + variables[midCode[index][4]] + ", 4");
									//mipsCodes.push_back("addu $a1, $a1, " + to_string(variableAdd[midCode[index][3]]));
									//mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", 0($a1)");
									mipsCodes.push_back("lw " + variables[midCode[index][1]] + ", " + midCode[index][3] + "($a1)");
								}
							}
						}
					}
				}
			}
			else
			{// array[*] = var
				if (midCode[index][2][0] == '\'' || midCode[index][2][0] == '-' || midCode[index][2][0] == '+' ||
					('0' <= midCode[index][2][0] && midCode[index][2][0] <= '9'))
				{
					int position ;
					if (midCode[index][2][0] == '+' ||
						('0' <= midCode[index][2][0] && midCode[index][2][0] <= '9'))
					{
						position = stoi(midCode[index][2]) * 4;
					}
					else if (midCode[index][2][0] == '\'')
					{
						position = (midCode[index][2][1]) * 4;
					}
					if (midCode[index][4] == "$v0")
					{
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
							mipsCodes.push_back("sw $v0, " + to_string(position + localVarAdd[scopeNum].first[midCode[index][1]]) + "($sp)");
						else
							//mipsCodes.push_back("sw $v0, " + to_string(position + variableAdd[midCode[index][1]]) + "($0)");
							mipsCodes.push_back("sw $v0, " + midCode[index][1] + "+" + to_string(position));
					}
					else if (midCode[index][4][0] == '\'')
					{
						mipsCodes.push_back("la $a1, " + midCode[index][4]);
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
							mipsCodes.push_back("sw $a1, " + to_string(position + localVarAdd[scopeNum].first[midCode[index][1]]) + "($sp)");
						else
							//mipsCodes.push_back("sw $a1, " + to_string(position + variableAdd[midCode[index][1]]) + "($0)");
							mipsCodes.push_back("sw $a1, " + midCode[index][1] + "+" + to_string(position));
					}
					else if (midCode[index][4][0] == '-' || midCode[index][4][0] == '+' ||
						('0' <= midCode[index][4][0] && midCode[index][4][0] <= '9'))
					{
						mipsCodes.push_back("li $a1, " + midCode[index][4]);
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
							mipsCodes.push_back("sw $a1, " + to_string(position + localVarAdd[scopeNum].first[midCode[index][1]]) + "($sp)");
						else
							//mipsCodes.push_back("sw $a1, " + to_string(position + variableAdd[midCode[index][1]]) + "($0)");
							mipsCodes.push_back("sw $a1, " + midCode[index][1] + "+" + to_string(position));
					}
					else
					{
						if (variables.find(midCode[index][4]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][4], true);
							}
							else
							{
								allocBusyReg(midCode[index][4], true);
							}
						}
						else
						{
							swap(variables[midCode[index][4]]);
						}
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
							mipsCodes.push_back("sw " + variables[midCode[index][4]] + ", " + to_string(position + localVarAdd[scopeNum].first[midCode[index][1]]) + "($sp)");
						else
							//mipsCodes.push_back("sw " + variables[midCode[index][4]] + ", " + to_string(position + variableAdd[midCode[index][1]]) + "($0)");
							mipsCodes.push_back("sw " + variables[midCode[index][4]] + ", " + midCode[index][1] + "+" + to_string(position));
					}
				}
				else //var as index
				{
					if (midCode[index][2] == "$v0")
					{
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
						{
							mipsCodes.push_back("mul $a1, $v0, 4");
							//mipsCodes.push_back("addu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][1]]));
							mipsCodes.push_back("addiu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][1]]));
							mipsCodes.push_back("addu $a1, $a1, $sp");
						}
						else
						{
							mipsCodes.push_back("mul $a1, $v0, 4");
							//mipsCodes.push_back("addu $a1, $a1, " + to_string(variableAdd[midCode[index][1]] + dataPosition));
							mipsCodes.push_back("addiu $a1, $a1, " + to_string(variableAdd[midCode[index][1]]+dataPosition));
						}
					}
					else
					{
						if (variables.find(midCode[index][2]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][2], true);
							}
							else
							{
								allocBusyReg(midCode[index][2], true);
							}
						}
						else
						{
							swap(variables[midCode[index][2]]);
						}
						if (localVarAdd[scopeNum].first.find(midCode[index][1]) != localVarAdd[scopeNum].first.end())
						{
							mipsCodes.push_back("mul $a1, " + variables[midCode[index][2]] + ", 4");
							//mipsCodes.push_back("addu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][1]]));
							mipsCodes.push_back("addiu $a1, $a1, " + to_string(localVarAdd[scopeNum].first[midCode[index][1]]));
							mipsCodes.push_back("addu $a1, $a1, $sp");
						}
						else
						{
							mipsCodes.push_back("mul $a1, " + variables[midCode[index][2]] + ", 4");
							//mipsCodes.push_back("addu $a1, $a1, " + to_string(variableAdd[midCode[index][1]] + dataPosition));
							mipsCodes.push_back("addiu $a1, $a1, " + to_string(variableAdd[midCode[index][1]]+dataPosition));
						}
					}
					if (midCode[index][4] == "$v0")
					{
						mipsCodes.push_back("sw $v0, 0($a1)");
					}
					else if (midCode[index][4][0] == '\'')
					{
						mipsCodes.push_back("la $a2, " + midCode[index][4]);
						mipsCodes.push_back("sw $a2, 0($a1)");
					}
					else if (midCode[index][4][0] == '-' || midCode[index][4][0] == '+' ||
						('0' <= midCode[index][4][0] && midCode[index][4][0] <= '9'))
					{
						mipsCodes.push_back("li $a2, " + midCode[index][4]);
						mipsCodes.push_back("sw $a2, 0($a1)");
					}
					else
					{
						if (variables.find(midCode[index][4]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][4], true);
							}
							else
							{
								allocBusyReg(midCode[index][4], true);
							}
						}
						else
						{
							swap(variables[midCode[index][4]]);
						}
						mipsCodes.push_back("sw " + variables[midCode[index][4]] + ", 0($a1)");
					}
				}
			}
		}
		else if (midCode[index][0] == "localVar") {
			int size = stoi(midCode[index][3]) * 4;
			localVarAdd[scopeNum].second -= size;
			localVarAdd[scopeNum].first[midCode[index][2]] = localVarAdd[scopeNum].second;
		}
		else if (midCode[index][0] == "OpState") {
			dirtyMap[midCode[index][1]] = 1;
			if (variableAdd.find(midCode[index][1]) == variableAdd.end()
				&&localVarAdd[scopeNum].first.find(midCode[index][1]) == localVarAdd[scopeNum].first.end())
			{
				localVarAdd[scopeNum].second -= 4;
				localVarAdd[scopeNum].first[midCode[index][1]] = localVarAdd[scopeNum].second;
			}
			
			if (midCode[index][3][0] == '\'' || midCode[index][3][0] == '-' || midCode[index][3][0] == '+' ||
				('0' <= midCode[index][3][0] && midCode[index][3][0] <= '9')) {
				if (midCode[index][5][0] == '\'' || midCode[index][5][0] == '-' || midCode[index][5][0] == '+' ||
					('0' <= midCode[index][5][0] && midCode[index][5][0] <= '9'))
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], false);
						}
						else
						{
							allocBusyReg(midCode[index][1], false);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					if (midCode[index][3][0] == '\'')
						mipsCodes.push_back("la $a1, " + midCode[index][3]);
					else
						mipsCodes.push_back("li $a1, " + midCode[index][3]);
					if (midCode[index][4] == "+")
					{
						//mipsCodes.push_back("addu " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
						mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
					}
					else if (midCode[index][4] == "-")
					{
						//mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
						if(midCode[index][5][0] != '\'')
							mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", $a1, " + to_string(0-stoi(midCode[index][5])));
						else
							mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
					}
					else if (midCode[index][4] == "*")
					{
						mipsCodes.push_back("mul " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
					}
					else if (midCode[index][4] == "/")
					{
						mipsCodes.push_back("div " + variables[midCode[index][1]] + ", $a1, " + midCode[index][5]);
					}
				}
				else {
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], false);
						}
						else
						{
							allocBusyReg(midCode[index][1], false);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					if (variables.find(midCode[index].back()) != variables.end()) {
						if (midCode[index][4] == "+") {
							//mipsCodes.push_back("addu " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								//+ ", "+ midCode[index][3]);
							mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								+ ", " + midCode[index][3]);
						}
						else if (midCode[index][4] == "-") {
							if (midCode[index][3][0] == '\'')
								mipsCodes.push_back("la $a1, " + midCode[index][3]);
							else
								mipsCodes.push_back("li $a1, " + midCode[index][3]);
							mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", $a1, " + variables[midCode[index][5]]);
						}
						else if (midCode[index][4] == "*") {
							mipsCodes.push_back("mul " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								+ ", " + midCode[index][3]);
						}
						else if (midCode[index][4] == "/") {
							if (midCode[index][3][0] == '\'')
								mipsCodes.push_back("la $a1, " + midCode[index][3]);
							else
								mipsCodes.push_back("li $a1, " + midCode[index][3]);
							//mipsCodes.push_back("div " + variables[midCode[index][1]] + ", $a1, " + variables[midCode[index][5]]);
							mipsCodes.push_back("div $a1, " + variables[midCode[index][5]]);
							mipsCodes.push_back("mflo " + variables[midCode[index][1]]);
						}
					}
					else
					{

						if (variables.find(midCode[index].back()) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index].back(), true);
							}
							else
							{
								allocBusyReg(midCode[index].back(), true);
							}
						}
						else
						{
							swap(variables[midCode[index].back()]);
						}
						if (variables.find(midCode[index][1]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][1], false);
							}
							else
							{
								allocBusyReg(midCode[index][1], false);
							}
						}
						else
						{
							swap(variables[midCode[index][1]]);
						}

						if (midCode[index][4] == "+")
						{
							//mipsCodes.push_back("addu " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								//+ ", " + midCode[index][3]);
							mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								+ ", " + midCode[index][3]);
						}
						else if (midCode[index][4] == "-")
						{
							if (midCode[index][3][0] == '\'')
								mipsCodes.push_back("la $a1, " + midCode[index][3]);
							else
								mipsCodes.push_back("li $a1, " + midCode[index][3]);
							mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", $a1, " + variables[midCode[index][5]]);
						}
						else if (midCode[index][4] == "*")
						{
							mipsCodes.push_back("mul " + variables[midCode[index][1]] + ", " + variables[midCode[index][5]]
								+ ", " + midCode[index][3]);
						}
						else if (midCode[index][4] == "/")
						{
							if (midCode[index][3][0] == '\'')
								mipsCodes.push_back("la $a1, " + midCode[index][3]);
							else
								mipsCodes.push_back("li $a1, " + midCode[index][3]);
							mipsCodes.push_back("div " + variables[midCode[index][1]] + ", $a1, " + variables[midCode[index][5]]);
						}
					}
				}
			}
			else
			{
				
				if (midCode[index][5][0] == '\'' || midCode[index][5][0] == '-' || midCode[index][5][0] == '+' ||
					('0' <= midCode[index][5][0] && midCode[index][5][0] <= '9'))
				{

					if (variables.find(midCode[index][3]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][3], true);
						}
						else
						{
							allocBusyReg(midCode[index][3], true);
						}
					}
					else
					{
						swap(variables[midCode[index][3]]);
					}
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], false);
						}
						else
						{
							allocBusyReg(midCode[index][1], false);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}

					if (midCode[index][4] == "+")
					{
						//mipsCodes.push_back("addu " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index][5]);
						mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index][5]);
					}
					else if (midCode[index][4] == "-")
					{
						if (midCode[index][5][0] != '\'')
							mipsCodes.push_back("addiu " + variables[midCode[index][1]] + ", "+variables[midCode[index][3]]+", " + to_string(0 - stoi(midCode[index][5])));
						else
							mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", "+ variables[midCode[index][3]] +", " + midCode[index][5]);
						//mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index][5]);
					}
					else if (midCode[index][4] == "*")
					{
						mipsCodes.push_back("mul " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index][5]);
					}
					else if (midCode[index][4] == "/")
					{
						mipsCodes.push_back("div " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index][5]);
					}
					//cout << index << "now freeregs num is " << freeRegs.size() << endl;
				}
				else
				{

					//cout << "---------------------------\n";
					if (variables.find(midCode[index][5]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][5], true);
						}
						else
						{
							allocBusyReg(midCode[index][5], true);
						}
					}
					else
					{
						//cout << midCode[index][5]<<"   "<<variables[midCode[index][5]] << endl;
						//cout << busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
						swap(variables[midCode[index][5]]);
						//cout << busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
					}
					if (variables.find(midCode[index][3]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][3], true);
						}
						else
						{
							allocBusyReg(midCode[index][3], true);
						}
					}
					else
					{
						//cout << midCode[index][3] << "   " << variables[midCode[index][3]] << endl;
						//cout << busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
						swap(variables[midCode[index][3]]);
						//cout << busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
					}
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], false);
						}
						else
						{
							allocBusyReg(midCode[index][1], false);
						}
						//cout << midCode[index][1] << "   " << variables[midCode[index][1]] << endl;
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					//cout << midCode[index][1] << "   " << variables[midCode[index][1]] << endl;
					//cout << "---------------------------\n";
					if (midCode[index][4] == "+")
					{
						mipsCodes.push_back("addu " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + variables[midCode[index][5]]);
					}
					else if (midCode[index][4] == "-")
					{
						mipsCodes.push_back("subu " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + variables[midCode[index][5]]);
					}
					else if (midCode[index][4] == "*")
					{
						mipsCodes.push_back("mul " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + variables[midCode[index][5]]);
					}
					else if (midCode[index][4] == "/")
					{
						//mipsCodes.push_back("div " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + variables[midCode[index][5]]);
						mipsCodes.push_back("div "+ variables[midCode[index][3]] + ", " + variables[midCode[index][5]]);
						mipsCodes.push_back("mflo " + variables[midCode[index][1]]);
					}

				}
			}
		}
		else if (midCode[index][0] == "lineFeed") {
			mipsCodes.push_back("la $a0, '\\n'");
			mipsCodes.push_back("li $v0, 11");
			mipsCodes.push_back("syscall");
		}
		else if(midCode[index][0] == "str"){
			mipsCodes.push_back(midCode[index][1]+":.asciiz " + midCode[index][2]);
		}
		else if(midCode[index][0] == "printStr"){
			mipsCodes.push_back("la $a0, " + midCode[index][1]);
			mipsCodes.push_back("li $v0, 4");
			mipsCodes.push_back("syscall");
		}
		else if(midCode[index][0] == "printInt"){
			if(midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
				('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
			{
				mipsCodes.push_back("li $a0, " + midCode[index][1]);
			}
			else if (midCode[index][1][0] == '\'')
			{
				mipsCodes.push_back("la $a0, " + midCode[index][1]);
			}
			else
			{
				if (variables.find(midCode[index][1]) == variables.end()){
					if (!freeRegs.empty())
					{
						allocFreeReg(midCode[index][1],true);
					}
					else
					{
						allocBusyReg(midCode[index][1],true);
					}
				}
				else
				{
					swap(variables[midCode[index][1]]);
				}
				mipsCodes.push_back("move $a0, " + variables[midCode[index][1]]);
			}
			mipsCodes.push_back("li $v0, 1");
			mipsCodes.push_back("syscall");
		}
		else if(midCode[index][0] == "printChar"){
			if(midCode[index][1][0] == '\'')
			{
				mipsCodes.push_back("la $a0, " + midCode[index][1]);
			}
			else
			{
				if (variables.find(midCode[index][1]) == variables.end())
				{
					if (!freeRegs.empty())
					{
						allocFreeReg(midCode[index][1],true);
					}
					else
					{
						allocBusyReg(midCode[index][1],true);
					}
				}
				else
				{
					swap(variables[midCode[index][1]]);
				}
				mipsCodes.push_back("move $a0, " + variables[midCode[index][1]]);
			}
			mipsCodes.push_back("li $v0, 11");
			mipsCodes.push_back("syscall");
		}
		else if(midCode[index][0] == "read"){
			if(midCode[index][1] == "int"){
				mipsCodes.push_back("li $v0, 5");
				mipsCodes.push_back("syscall");
			}
			else
			{
				mipsCodes.push_back("li $v0, 12");
				mipsCodes.push_back("syscall");
			}
			dirtyMap[midCode[index][2]] = 1;
			if (variables.find(midCode[index][2]) == variables.end())
			{
				if (!freeRegs.empty())
				{
					allocFreeReg(midCode[index][2],false);
				}
				else
				{
					allocBusyReg(midCode[index][2],false);
				}
			}
			else
			{
				swap(variables[midCode[index][2]]);
			}
			mipsCodes.push_back("move " + variables[midCode[index][2]] + ", $v0");
		}
		else if(midCode[index][0] == "condition"){
			if(midCode[index].size() == 2){
				if (midCode[index][1][0] == '\'' || midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
					('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
				{
					if (midCode[index][1][0] == '0')
					{
						if (midCode[index + 1][1] == "BZ")
						{
							//if (midCode[index + 1].back() == "release")release();///////////////
							//if (midCode[index + 1].back() == "release")variables.clear(); ////////////
							mipsCodes.push_back("j " + midCode[index + 1][2]);
						}
					}
					else
					{
						if (midCode[index + 1][1] == "BNZ")
						{
							release();
							variables.clear();
							addGlobalReg();
							mipsCodes.push_back("j " + midCode[index + 1][2]);
						}
					}
				}
				else
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], true);
						}
						else
						{
							allocBusyReg(midCode[index][1], true);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					if (midCode[index + 1][1] == "BZ")
					{
						//if (midCode[index + 1].back() == "release")release();///////////////////
						mipsCodes.push_back("beqz " + variables[midCode[index][1]] + ", " + midCode[index + 1][2]);
						//if (midCode[index + 1].back() == "release")variables.clear();/////////////////
					}
					else if (midCode[index + 1][1] == "BNZ")
					{
						release();
						mipsCodes.push_back("bnez " + variables[midCode[index][1]] + ", " + midCode[index + 1][2]);
						variables.clear();
						addGlobalReg();
					}
				}
			}
			else
			{
				if (midCode[index][1][0] == '\'' || midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
					('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
				{
					if (midCode[index][3][0] == '\'' || midCode[index][3][0] == '-' || midCode[index][3][0] == '+' ||
						('0' <= midCode[index][3][0] && midCode[index][3][0] <= '9'))
					{
						if (midCode[index][3][0] == '\'')
						{
							mipsCodes.push_back("la $a1, " + midCode[index][3]);
						}
						else if (midCode[index][3][0] == '-' || midCode[index][3][0] == '+' ||
							('0' <= midCode[index][3][0] && midCode[index][3][0] <= '9'))
						{
							mipsCodes.push_back("li $a1, " + midCode[index][3]);
						}
						if (midCode[index + 1][1] == "BZ")
						{
						//	if (midCode[index + 1].back() == "release") release();//////////////////////
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("bne $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("beq $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("ble $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("blt $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("bge $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("bgt $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
						//	if (midCode[index + 1].back() == "release") variables.clear();/////////////////////
						}
						else if (midCode[index + 1][1] == "BNZ")
						{
							release();
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("beq $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("bne $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("bgt $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("bge $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("blt $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("ble $a1, " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							variables.clear();
							addGlobalReg();
						}
					}
					else
					{
						if (variables.find(midCode[index][3]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][3], true);
							}
							else
							{
								allocBusyReg(midCode[index][3], true);
							}
						}
						else
						{
							swap(variables[midCode[index][3]]);
						}
						if (midCode[index + 1][1] == "BZ")
						{
						//	if (midCode[index + 1].back() == "release") release();///////////////////
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("bne "+variables[midCode[index][3]]+", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("beq " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("ble " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("blt " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("bge " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("bgt " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
						//	if (midCode[index + 1].back() == "release") variables.clear();///////////////////
						}
						else if (midCode[index + 1][1] == "BNZ")
						{
							release();
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("beq " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("bne " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("bgt " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("bge " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("blt " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("ble " + variables[midCode[index][3]] + ", " + midCode[index][1] + ", " + midCode[index + 1][2]);
							}
							variables.clear();
							addGlobalReg();
						}
					}
				}
				else
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], true);
						}
						else
						{
							allocBusyReg(midCode[index][1], true);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					if (midCode[index][3][0] == '\'' || midCode[index][3][0] == '-' || midCode[index][3][0] == '+' ||
						('0' <= midCode[index][3][0] && midCode[index][3][0] <= '9'))
					{
						if (midCode[index + 1][1] == "BZ")
						{
							//if(midCode[index+1].back() == "release") release();////////////////////
							if (midCode[index][3] != "0")
							{
								if (midCode[index][2] == "==")
								{
									mipsCodes.push_back("bne " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "!=")
								{
									mipsCodes.push_back("beq " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "<")
								{
									mipsCodes.push_back("bge " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "<=")
								{
									mipsCodes.push_back("bgt " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == ">")
								{
									mipsCodes.push_back("ble " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == ">=")
								{
									mipsCodes.push_back("blt " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
								}
							}
							else
							{
								if (midCode[index][2] == "==")
								{
									mipsCodes.push_back("bne " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "!=")
								{
									mipsCodes.push_back("beq " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "<")
								{
									mipsCodes.push_back("bge " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == "<=")
								{
									mipsCodes.push_back("bgt " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == ">")
								{
									mipsCodes.push_back("ble " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
								else if (midCode[index][2] == ">=")
								{
									mipsCodes.push_back("blt " + variables[midCode[index][1]] + ", $0, " + midCode[index + 1][2]);
								}
							}
							//if (midCode[index + 1].back() == "release") variables.clear();
						}
						else if (midCode[index + 1][1] == "BNZ")
						{
							release();
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("beq " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("bne " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("blt " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("ble " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("bgt " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("bge " + variables[midCode[index][1]] + ", " + midCode[index][3] + ", " + midCode[index + 1][2]);
							}
							variables.clear();
							addGlobalReg();
						}
					}
					else
					{
						if (variables.find(midCode[index][3]) == variables.end())
						{
							if (!freeRegs.empty())
							{
								allocFreeReg(midCode[index][3], true);
							}
							else
							{
								allocBusyReg(midCode[index][3], true);
							}
						}
						else
						{
							swap(variables[midCode[index][3]]);
						}
						if (midCode[index + 1][1] == "BZ")
						{
							//if (midCode[index + 1].back() == "release") release();///////////////////
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("bne " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("beq " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("bge " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("bgt " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("ble " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("blt " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							//if (midCode[index + 1].back() == "release") variables.clear();///////////////////////
						}
						else if (midCode[index + 1][1] == "BNZ")
						{
							release();
							if (midCode[index][2] == "==")
							{
								mipsCodes.push_back("beq " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "!=")
							{
								mipsCodes.push_back("bne " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<")
							{
								mipsCodes.push_back("blt " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == "<=")
							{
								mipsCodes.push_back("ble " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">")
							{
								mipsCodes.push_back("bgt " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							else if (midCode[index][2] == ">=")
							{
								mipsCodes.push_back("bge " + variables[midCode[index][1]] + ", " + variables[midCode[index][3]] + ", " + midCode[index + 1][2]);
							}
							variables.clear();
							addGlobalReg();
						}
					}
				}
			}
			index++;
		}
		else if(midCode[index][0] == "back"){
			release();
			variables.clear();
			addGlobalReg();
			mipsCodes.push_back("jr $ra");
			//cout << freeRegs[0] << freeRegs[1] << freeRegs[2] << freeRegs.size()<< "   "<< busyRegs.size()<<endl;
		}
		else if(midCode[index][0] == "release"){
			release();
			variables.clear();
			addGlobalReg();
		}
		else if(midCode[index][0] == "changeScope"){
			//release();
			releaseAllChanged();
			//mipsCodes.push_back("addu $sp, $sp, " + to_string(localVarAdd[scopeNum].second));
			ParaNumList.push_back(1);
			//mipsCodes.push_back("sw $ra, -4($sp)");
			scopeSize = localVarAdd[scopeNum].second;
			variables.clear();
			addGlobalReg();
			saveMark = false;
		}
		else if(midCode[index][0] == "push"){
			int add = -((ParaNumList[ParaNumList.size()-1]+1)*4 - scopeSize);
			ParaNumList[ParaNumList.size() - 1] += 1;
			if (midCode[index][1][0] == '\'' || midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
				('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
			{
				if (midCode[index][1][0] == '\'')
					mipsCodes.push_back("la $a1, " + midCode[index][1]);
				else
					mipsCodes.push_back("li $a1, " + midCode[index][1]);
				mipsCodes.push_back("sw $a1, " + to_string(add) + "($sp)");
			}
			else
			{
				if (variables.find(midCode[index][1]) == variables.end())
				{
					if (!freeRegs.empty())
					{
						allocFreeReg(midCode[index][1], true);
					}
					else
					{
						allocBusyReg(midCode[index][1], true);
					}
				}
				else
				{
					swap(variables[midCode[index][1]]);
				}
				mipsCodes.push_back("sw " + variables[midCode[index][1]]+", " + to_string(add) + "($sp)");
			}
			//cout << localVarAdd[scopeNum].second<< endl;
		}
		else if (midCode[index][0] == "para")
		{
			localVarAdd[scopeNum].second -= 4;
			localVarAdd[scopeNum].first[midCode[index][2]] = localVarAdd[scopeNum].second;
			/*if (variables.find(midCode[index][2]) == variables.end())
			{
				if (!freeRegs.empty())
				{
					allocFreeReg(midCode[index][2], false);
				}
				else
				{
					allocBusyReg(midCode[index][2], false);
				}
			}
			else
			{
				swap(variables[midCode[index][2]]);
			}
			mipsCodes.push_back("lw " + variables[midCode[index][2]]+ ", "+ to_string(localVarAdd[scopeNum].second)+"($sp)");*/
		}
		else if (midCode[index][0] == "ret") 
		{
			if (midCode[index].size() == 2)
			{
				if (midCode[index][1][0] == '\'' || midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
					('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
				{
					if (midCode[index][1][0] == '\'')
						mipsCodes.push_back("la $v0, " + midCode[index][1]);
					else
						mipsCodes.push_back("li $v0, " + midCode[index][1]);
				}
				else
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], true);
						}
						else
						{
							allocBusyReg(midCode[index][1], true);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					mipsCodes.push_back("move $v0, " + variables[midCode[index][1]]);
				}
			}
			if (scopeNum != 0)
			{
				release();
				variables.clear();
				addGlobalReg();
				//ParaNumList.erase(ParaNumList.end());
				mipsCodes.push_back("jr $ra");
			}
			else
			{
				mipsCodes.push_back("li $v0, 10");
				mipsCodes.push_back("syscall");
			}
		}
		else if(midCode[index][0] == "inlineRet"){
			if (midCode[index][1] != "")
			{
				if (midCode[index][1][0] == '\'' || midCode[index][1][0] == '-' || midCode[index][1][0] == '+' ||
					('0' <= midCode[index][1][0] && midCode[index][1][0] <= '9'))
				{
					if (midCode[index][1][0] == '\'')
						mipsCodes.push_back("la $v0, " + midCode[index][1]);
				else
						mipsCodes.push_back("li $v0, " + midCode[index][1]);
				}
				else
				{
					if (variables.find(midCode[index][1]) == variables.end())
					{
						if (!freeRegs.empty())
						{
							allocFreeReg(midCode[index][1], true);
						}
						else
						{
							allocBusyReg(midCode[index][1], true);
						}
					}
					else
					{
						swap(variables[midCode[index][1]]);
					}
					mipsCodes.push_back("move $v0, " + variables[midCode[index][1]]);
				}
			}
			if (midCode[index + 1][0] == "funcEndLabel")
			{
				if(midCode[index+1][1] != midCode[index][2]+":")
					mipsCodes.push_back("j " + midCode[index][2]);
			}
			else
			{
				mipsCodes.push_back("j " + midCode[index][2]);
			}
		}
		else if (midCode[index][0] == "call")
		{
			releaseWithoutSave();
			saveMark = true;
			//release();
			//variables.clear();
			ParaNumList.pop_back();
			//mipsCodes.push_back("addu $sp, $sp, " + to_string(scopeSize));
			mipsCodes.push_back("addiu $sp, $sp, " + to_string(scopeSize));
			mipsCodes.push_back("sw $ra, -4($sp)");
			mipsCodes.push_back("jal " + midCode[index][1]);
			mipsCodes.push_back("lw $ra, -4($sp)");
			//mipsCodes.push_back("subu $sp, $sp, " + to_string(scopeSize));
			mipsCodes.push_back("addiu $sp, $sp, " + to_string(0-scopeSize));
			//releaseWithoutSave();
		}
		else if (midCode[index][0] == "funcEndLabel")
		{
			mipsCodes.push_back(midCode[index][1]);
		}
		else {
			mipsCodes.push_back(midCode[index][0]);
		}
		//cout << midCode[index][0] << endl;
		//cout << index << midCode[index][0]<< " "<<freeRegs.size() << " "<<busyRegs.size()<< " "<<variables.size()<<endl;
	}
}

void mipsCode::allocFreeReg(string varName, bool useMark)
{
	string tempReg = freeRegs.back();
	freeRegs.pop_back();
	busyRegs.insert(busyRegs.begin(), tempReg);
	//busyRegs.push_back(tempReg);
	variables[varName] = tempReg;
	registers[tempReg] = varName;
	if (useMark)
	{
		if (localVarAdd[scopeNum].first.find(varName)==localVarAdd[scopeNum].first.end())
		{
			//int add = variableAdd[varName];
			//mipsCodes.push_back("lw " + tempReg + ", " + to_string(add) + "($0)");
			mipsCodes.push_back("lw " + tempReg + ", " + varName);
			//cout << varName << endl;
		}
		else
		{
			int add = localVarAdd[scopeNum].first[varName];
			mipsCodes.push_back("lw " + tempReg + ", " + to_string(add) + "($sp)");
		}
	}
	//cout << "alloc free reg " << tempReg << " for " << varName << endl;
}

void mipsCode::allocBusyReg(string varName,bool useMark)
{
	string replaced = busyRegs.back();
	busyRegs.pop_back();
	//busyRegs.erase(busyRegs.begin());
	busyRegs.insert(busyRegs.begin(), replaced);
	//busyRegs.push_back(replaced);
	string reVarName = registers[replaced];
	if (varName != "" && saveMark)
	{
		if (localVarAdd[scopeNum].first.find(reVarName) != localVarAdd[scopeNum].first.end())
		{

			int reVarAdd;
			reVarAdd = localVarAdd[scopeNum].first[reVarName];
			mipsCodes.push_back("sw  " + replaced + ", " + to_string(reVarAdd) + "($sp)");
			//cout << "sw  " + replaced + ", " + to_string(reVarAdd) + "($sp)\n";
		}
		else if (variableAdd.find(reVarName) != variableAdd.end())
		{
			mipsCodes.push_back("sw " + replaced + ", " + reVarName);
		}
	}
	variables.erase(reVarName);
	variables[varName] = replaced;
	registers[replaced] = varName;
	//mipsCodes.push_back("li $v1, " + to_string(reVarAdd));
	//mipsCodes.push_back("sw " + replaced + ", 0($v1)");
	if (useMark)
	{
		if (localVarAdd[scopeNum].first.find(varName) == localVarAdd[scopeNum].first.end())
		{
			//int add = variableAdd[varName];
			//mipsCodes.push_back("lw " + replaced + ", " + to_string(add) + "($0)");
			mipsCodes.push_back("lw " + replaced + ", " + varName);
			//cout << varName << endl;
		}
		else
		{
			int add = localVarAdd[scopeNum].first[varName];
			mipsCodes.push_back("lw " + replaced + ", " + to_string(add) + "($sp)");
		}
	}
	//cout << "alloc busy reg " << replaced << " for " << varName << endl;
}

void mipsCode::release()
{
	string tempVarName = "";
	int add = 0;
	for (auto iter = variables.begin(); iter != variables.end(); iter++)
	{
		
		tempVarName = iter->first;
		if (globalReg.find(tempVarName) != globalReg.end()) continue;
		//freeRegs.push_back(iter->second);
		freeRegs.insert(freeRegs.begin(), iter->second);
		//cout << nowLabel<<"\t"<<tempVarName << endl;
		if (dirtyMap.find(tempVarName) != dirtyMap.end())
		{
			if (scopeNum != 0)
			{
				if (localVarAdd[scopeNum].first.find(tempVarName) != localVarAdd[scopeNum].first.end())
				{
					add = localVarAdd[scopeNum].first[tempVarName];
					mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($sp)");
				}
				else
				{
					add = variableAdd[tempVarName];
					//mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($0)");
					mipsCodes.push_back("sw " + variables[tempVarName] + ", " + tempVarName);
				}
			}
			else
			{
				if (scopeSets[0][nowLabel][5].find(tempVarName) != scopeSets[0][nowLabel][5].end())
				{
					if (localVarAdd[scopeNum].first.find(tempVarName) != localVarAdd[scopeNum].first.end())
					{
						add = localVarAdd[scopeNum].first[tempVarName];
						mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($sp)");
					}
					else
					{
						add = variableAdd[tempVarName];
						//mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($0)");
						mipsCodes.push_back("sw " + variables[tempVarName] + ", " + tempVarName);
					}
				}
				/*else
				{
					if (tempVarName[0] != '\"')
					{
						if (localVarAdd[scopeNum].first.find(tempVarName) != localVarAdd[scopeNum].first.end())
						{
							add = localVarAdd[scopeNum].first[tempVarName];
							mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($sp)");
						}
						else
						{
							add = variableAdd[tempVarName];
							//mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($0)");
							mipsCodes.push_back("sw " + variables[tempVarName] + ", " + tempVarName);
						}
					}
				}*/
			}
		}
	}
	busyRegs.clear();
	clearRegisters();
	dirtyMap.clear();
	//variables.clear();
	//cout << "_____________________________________\n";
}

void mipsCode::releaseAllChanged()
{
	string tempVarName = "";
	int add = 0;
	for (auto iter = variables.begin(); iter != variables.end(); iter++)
	{
		tempVarName = iter->first;
		if (globalReg.find(tempVarName) != globalReg.end()) continue;
		freeRegs.insert(freeRegs.begin(), iter->second);
		if (dirtyMap.find(tempVarName) != dirtyMap.end())
		{
			if (localVarAdd[scopeNum].first.find(tempVarName) != localVarAdd[scopeNum].first.end())
			{
				add = localVarAdd[scopeNum].first[tempVarName];
				mipsCodes.push_back("sw " + variables[tempVarName] + ", " + to_string(add) + "($sp)");
			}
			else
			{
				add = variableAdd[tempVarName];
				mipsCodes.push_back("sw " + variables[tempVarName] + ", " + tempVarName);
			}
		}
	}
	busyRegs.clear();
	clearRegisters();
	dirtyMap.clear();
}

void mipsCode::releaseWithoutSave()
{
	
	for (auto iter = variables.begin(); iter != variables.end(); iter++)
	{
		if (globalReg.find(iter->first) != globalReg.end()) continue;
		freeRegs.insert(freeRegs.begin(),iter->second);
	}
	clearRegisters();
	busyRegs.clear();
	variables.clear();
	addGlobalReg();
	dirtyMap.clear();
	//cout << "_____________________________________\n";
}

void mipsCode::swap(string reg)
{
	//cout << busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
	if (reg[1] == 's') return;
	for (auto iter = busyRegs.begin(); iter != busyRegs.end(); iter++)
	{
		if (*iter == reg)
		{
			busyRegs.erase(iter);
			break;
		}
	}
	busyRegs.insert(busyRegs.begin(), reg);
	//cout <<busyRegs[0] << busyRegs[1] << busyRegs[2] << endl;
	//cout << "____________________________________\n";
}
