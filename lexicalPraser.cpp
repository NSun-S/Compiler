#include "lexicalPraser.h"
#include <iostream>

lexicalPraser::lexicalPraser(std::string code,std::string filename, errorHandler &handler)
{
	row = 1;
	col = 1;
	this->code = code;
	targetfile = filename;
	initReserverMap();
	lexicalPrase(handler);
	//output();
}

void lexicalPraser::lexicalPrase(errorHandler& handler)
{
	Token transfer;
	std::string symbol;
	index = 0;
	while (index < int(code.length())-1) 
	{
		symbol = "";
		ch = code[index];
		while (isspace(ch))
		{
			if (ch == '\n') {
				row++;
				col = 1;
			}
			else {
				col++;
			}
			index++; 
			if (index < int(code.length())-1) ch = code[index]; 
			else { break; };
		}
		if (index >= int(code.length()) - 1) break;
		if ((('a' <= ch) && (ch <= 'z')) || (('A' <= ch) && (ch <= 'Z'))||(ch == '_')) 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			while ((('a' <= ch) && (ch <= 'z')) || (('A' <= ch) && (ch <= 'Z'))
				|| (('0' <= ch) && (ch <= '9')) || (ch == '_')) 
			{
				symbol += ch;
				index++; 
				col++;
				if (index < int(code.length()) - 1) ch = code[index]; else { break; };
			}
			if (reserverMap.count(symbol) > 0) 
			{
				switch (reserverMap[symbol])
				{
				case 0:
					TokenList.push_back(make_pair(transfer.get(transfer._const), symbol));
					outputList.push_back(transfer.id2str(transfer._const, symbol));
					break;
				case 1:
					TokenList.push_back(make_pair(transfer.get(transfer._int), symbol));
					outputList.push_back(transfer.id2str(transfer._int, symbol));
					break;
				case 2:
					TokenList.push_back(make_pair(transfer.get(transfer._char), symbol));
					outputList.push_back(transfer.id2str(transfer._char, symbol));
					break;
				case 3:
					TokenList.push_back(make_pair(transfer.get(transfer._void), symbol));
					outputList.push_back(transfer.id2str(transfer._void, symbol));
					break;
				case 4:
					TokenList.push_back(make_pair(transfer.get(transfer._main), symbol));
					outputList.push_back(transfer.id2str(transfer._main, symbol));
					break;
				case 5:
					TokenList.push_back(make_pair(transfer.get(transfer._if), symbol));
					outputList.push_back(transfer.id2str(transfer._if, symbol));
					break;
				case 6:
					TokenList.push_back(make_pair(transfer.get(transfer._else), symbol));
					outputList.push_back(transfer.id2str(transfer._else, symbol));
					break;
				case 7:
					TokenList.push_back(make_pair(transfer.get(transfer._do), symbol));
					outputList.push_back(transfer.id2str(transfer._do, symbol));
					break;
				case 8:
					TokenList.push_back(make_pair(transfer.get(transfer._while), symbol));
					outputList.push_back(transfer.id2str(transfer._while, symbol));
					break;
				case 9:
					TokenList.push_back(make_pair(transfer.get(transfer._for), symbol));
					outputList.push_back(transfer.id2str(transfer._for, symbol));
					break;
				case 10:
					TokenList.push_back(make_pair(transfer.get(transfer._scanf), symbol));
					outputList.push_back(transfer.id2str(transfer._scanf, symbol));
					break;
				case 11:
					TokenList.push_back(make_pair(transfer.get(transfer._printf), symbol));
					outputList.push_back(transfer.id2str(transfer._printf, symbol));
					break;
				case 12:
					TokenList.push_back(make_pair(transfer.get(transfer._return), symbol));
					outputList.push_back(transfer.id2str(transfer._return, symbol));
					break;
				default:
					break;
				}
				PositionList.push_back(std::make_pair(row, col));
			}
			else 
			{
				TokenList.push_back(make_pair(transfer.get(transfer.identifier), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.identifier, symbol));
			}
		}
		else if ((('0' <= ch) && (ch <= '9'))) 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			while ((('0' <= ch) && (ch <= '9')))
			{
				symbol += ch;
				index++; 
				col++;
				if (index < int(code.length()) - 1) ch = code[index]; else { break; };
			}
			TokenList.push_back(make_pair(transfer.get(transfer.intconst), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.intconst, symbol));
		} else if (ch == '+') 
		{
			symbol += ch;
			index++; 
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.plus), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.plus, symbol));
		}
		else if (ch == '-') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.minus), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.minus, symbol));
		}
		else if (ch == '*') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.multi), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.multi, symbol));
		}
		else if (ch == '/') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.div), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.div, symbol));
		}
		else if (ch == ';') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.semicolon), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.semicolon, symbol));
		}
		else if (ch == ',')
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.comma), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.comma, symbol));
		}
		else if (ch == '(') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.lparent), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.lparent, symbol));
		}
		else if (ch == ')') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.rparent), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.rparent, symbol));
		}
		else if (ch == '[') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.lbrack), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.lbrack, symbol));
		}
		else if (ch == ']') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.rbrack), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.rbrack, symbol));
		}
		else if (ch == '{') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.lbrace), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.lbrace, symbol));
		}
		else if (ch == '}') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			TokenList.push_back(make_pair(transfer.get(transfer.rbrace), symbol));
			PositionList.push_back(std::make_pair(row, col));
			outputList.push_back(transfer.id2str(transfer.rbrace, symbol));
		}
		else if (ch == '=') 
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			if (ch == '=') 
			{
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};
				TokenList.push_back(make_pair(transfer.get(transfer.equal), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.equal, symbol));
			}
			else 
			{
				TokenList.push_back(make_pair(transfer.get(transfer.assign), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.assign, symbol));
			}
		}
		else if (ch == '!')
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			if (ch == '=') 
			{
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};
				TokenList.push_back(make_pair(transfer.get(transfer.ne), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.ne, symbol));
			}
			/*else {
				handler.newError(row, col, handler.illegalSymbolOrLexicalInconformity);
				index++;
			}*/
		}
		else if (ch == '>')
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			if (ch == '=') 
			{
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};
				TokenList.push_back(make_pair(transfer.get(transfer.ge), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.ge, symbol));
			}
			else
			{
				TokenList.push_back(make_pair(transfer.get(transfer.greater), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.greater, symbol));
			}
		}
		else if (ch == '<')
		{
			symbol += ch;
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			if (ch == '=') 
			{
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};
				TokenList.push_back(make_pair(transfer.get(transfer.le), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.le, symbol));
			}
			else
			{
				TokenList.push_back(make_pair(transfer.get(transfer.less), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.less, symbol));
			}
		}
		else if(ch == '\'')
		{
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			if ((ch == '+')|| (ch == '-')|| (ch == '/') || (ch == '*') || ('0' <= ch && ch <= '9')
				||('a'<=ch && ch <='z')||('A'<=ch && ch<='Z')||(ch=='_'))
			{
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};
				if (ch != '\'') {
					handler.newError(row, col, handler.illegalSymbolOrLexicalInconformity);
					
				}
				else {
					index++;
					col++;
					if (index < int(code.length()) - 1) ch = code[index]; else {};
				}
				TokenList.push_back(make_pair(transfer.get(transfer.charconst), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.charconst, symbol));
			}
			else {
				handler.newError(row, col, handler.illegalSymbolOrLexicalInconformity);
				symbol += ch;
				index++;
				col++;
				if (index < int(code.length()) - 1) ch = code[index]; else {};
				if (ch == '\'') {
					TokenList.push_back(make_pair(transfer.get(transfer.charconst), symbol));
					PositionList.push_back(std::make_pair(row, col));
					outputList.push_back(transfer.id2str(transfer.charconst, symbol));
					index++;
					col++;
					if (index < int(code.length()) - 1) ch = code[index]; else {};
				}
				//localization();
			}
		}
		else if(ch == '\"')
		{
			index++; 
			col++;
			if(index < int(code.length())-1) ch = code[index]; else{};
			while ((ch == 32) || (ch == 33) || (35 <= ch && ch <= 126))
			{			
				symbol += ch;
				index++; 
				col++;
				if(index < int(code.length())-1) ch = code[index]; else{};	
			}
			if (ch != '\"') {
				handler.newError(row, col, handler.illegalSymbolOrLexicalInconformity);
				TokenList.push_back(make_pair(transfer.get(transfer.str), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.str, symbol));
				TokenList.push_back(make_pair(transfer.get(transfer.rparent), ")"));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.rparent, ")"));
				TokenList.push_back(make_pair(transfer.get(transfer.semicolon), ";"));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.semicolon, ";"));
			}
			else {
				TokenList.push_back(make_pair(transfer.get(transfer.str), symbol));
				PositionList.push_back(std::make_pair(row, col));
				outputList.push_back(transfer.id2str(transfer.str, symbol));
				index++;
				col++;
			}
			
			if (index < int(code.length())-1) ch = code[index]; else {};
		}
		else 
		{
			handler.newError(row, col, handler.illegalSymbolOrLexicalInconformity);
			index++;
			col++;
			//localization();
		}
	}
}

void lexicalPraser::initReserverMap()
{
	reserverMap["const"] = 0;
	reserverMap["int"] = 1;
	reserverMap["char"] = 2;
	reserverMap["void"] = 3;
	reserverMap["main"] = 4;
	reserverMap["if"] = 5;
	reserverMap["else"] = 6;
	reserverMap["do"] = 7;
	reserverMap["while"] = 8;
	reserverMap["for"] = 9;
	reserverMap["scanf"] = 10;
	reserverMap["printf"] = 11;
	reserverMap["return"] = 12;
}

void lexicalPraser::output()
{
	outputHandler outputStream(outputList,targetfile);
}

void lexicalPraser::localization()
{
	while (!TokenList.empty() && TokenList.back().first != "SEMICN" 
		&& TokenList.back().first != "RBRACE"
		&& TokenList.back().first != "LBRACE" ) {
		TokenList.pop_back();
		PositionList.pop_back();
		outputList.pop_back();
	}
	while (ch != ';' && index < int(code.length())-1) { 
		if (ch == '\n') {
			row++;
			col = 1;
		}
		else {
			col++;
		}
		index++;
		if (index < int(code.length())-1) ch = code[index]; else { break; };
	}
	if (ch == ';') {
		index++;
		if (index < int(code.length())-1) ch = code[index]; else {};
	}
}

std::vector<std::pair<std::string,std::string>> lexicalPraser::getTokenList()
{
	return TokenList;
}

std::vector<std::pair<int, int>> lexicalPraser::getPositionList()
{
	return PositionList;
}
