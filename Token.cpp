#include "Token.h"

Token::Token()
{
	initmap();
}

std::string Token::id2str(token T, std::string str)
{
	return transfer[T] + " " + str;
}

std::string Token::get(token T)
{
	return transfer[T];
}

void Token::initmap()
{
	transfer[identifier] = "IDENFR";
	transfer[intconst] = "INTCON";
	transfer[charconst] = "CHARCON";
	transfer[str] = "STRCON";
	transfer[_const] = "CONSTTK";
	transfer[_int] = "INTTK";
	transfer[_char] = "CHARTK";
	transfer[_void] = "VOIDTK";
	transfer[_main] = "MAINTK";
	transfer[_if] = "IFTK";
	transfer[_else] = "ELSETK";
	transfer[_do] = "DOTK";
	transfer[_while] = "WHILETK";
	transfer[_for] = "FORTK";
	transfer[_scanf] = "SCANFTK";
	transfer[_printf] = "PRINTFTK";
	transfer[_return] = "RETURNTK";
	transfer[plus] = "PLUS";
	transfer[minus] = "MINU";
	transfer[multi] = "MULT";
	transfer[div] = "DIV";
	transfer[less] = "LSS";
	transfer[le] = "LEQ";
	transfer[greater] = "GRE";
	transfer[ge] = "GEQ";
	transfer[equal] = "EQL";
	transfer[ne] = "NEQ";
	transfer[assign] = "ASSIGN";
	transfer[semicolon] = "SEMICN";
	transfer[comma] = "COMMA";
	transfer[lparent] = "LPARENT";
	transfer[rparent] = "RPARENT";
	transfer[lbrack] = "LBRACK";
	transfer[rbrack] = "RBRACK";
	transfer[lbrace] = "LBRACE";
	transfer[rbrace] = "RBRACE";
}
