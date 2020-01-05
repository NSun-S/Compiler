#pragma once
#include<vector>
#include<string>
#include<map>
#include<utility>
#include<algorithm>
#include "outputHandler.h"
using namespace std;
class mipsCode
{
public:
	mipsCode(vector<vector<string>> midCode,string filename,bool opMark);
private:
	vector<vector<string>> midCode;
	vector<string> mipsCodes;
	string filename;
	vector<string> freeRegs;
	vector<string> busyRegs;
	map<string, string> registers;//register, variable
	map<string, string> variables;//variable, register
	map<string, int> variableAdd;//global variable, address
	vector<pair<map<string, int>,int>> localVarAdd;
	map<string, int> funcScope;
	int dataPosition;
	int heapPosition;
	int scopeNum;
	int scopeSize;
	bool saveMark;
	vector<int> ParaNumList;
	//use to mark if it is the first parameter when we call a function
	
	void inlineOptimal();
	void outputNewMidCode();
	vector<vector<string>> newMidCode;
	map<string,int> noRecursionFuncs;
	map<string, vector<vector<string>>> inlineFuncs;

	void activeVariable();
	map<int, map<string, vector<map<string, int>>>> scopeSets;
	//scopeNum, (blockName, (precursor,succeed,def,use,in,out))
	string nowLabel;
	void addMaps(string label);

	void conflictMap();
	map<string, int> conflictVars;
	vector<pair<string, int>> conflictVarPair;
	static int mySort(const pair<string,int> &var1, const pair<string,int> &var2);
	void allocGlobalRge();
	void addGlobalReg();
	map<string, string> globalReg;
	
	map<string, int> dirtyMap;

	void initRegisters();
	void clearRegisters();
	void output();
	void transfer(vector<vector<string>> midCode);
	void allocFreeReg(string varName, bool useMark);
	void allocBusyReg(string varName, bool useMark);
	void release();
	void releaseAllChanged();
	void releaseWithoutSave();
	void swap(string reg);
};

