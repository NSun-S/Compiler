#pragma once
#include<map>
#include<string>

class Token
{
public:
	Token();
	enum token {
		identifier,intconst,charconst,str,_const,_int,_char,_void,_main,
		_if,_else,_do,_while,_for,_scanf,_printf,_return,plus,
		minus,multi,div,less,le,greater,ge,equal,ne,
		assign,semicolon,comma,lparent,rparent,lbrack,rbrack,lbrace,rbrace
	};
	std::string id2str(token T, std::string str);
	std::string get(token T);
private:
	std::map<token, std::string> transfer;
	void initmap();
};

