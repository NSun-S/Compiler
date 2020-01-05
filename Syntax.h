#pragma once
#include<map>
#include<string>

class Syntax
{
public:
	Syntax();
	enum syntax {
		str,program,constdec,constdef,unsignedInt,integer,
		statehead,vardec,vardef,redef,noredef,compound,paraList,
		main,exp,item,factor,state,assstate,constate,condition,loopstate,
		stepsize,callre,callnore,vparaList,states,readstate,writestate,restate,
	};
	std::string get(syntax T);
private:
	std::map<syntax, std::string> transfer;
	void initmap();
};

