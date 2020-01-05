#include "Syntax.h"

Syntax::Syntax()
{
	initmap();
}

std::string Syntax::get(syntax T)
{
	return transfer[T];
}

void Syntax::initmap()
{
	transfer[str] = "<字符串>";
	transfer[program] = "<程序>";
	transfer[constdec] = "<常量说明>";
	transfer[constdef] = "<常量定义>";
	transfer[unsignedInt] = "<无符号整数>";
	transfer[integer] = "<整数>";
	transfer[statehead] = "<声明头部>";
	transfer[vardec] = "<变量说明>";
	transfer[vardef] = "<变量定义>";
	transfer[redef] = "<有返回值函数定义>";
	transfer[noredef] = "<无返回值函数定义>";
	transfer[compound] = "<复合语句>";
	transfer[paraList] = "<参数表>";
	transfer[main] = "<主函数>";
	transfer[exp] = "<表达式>";
	transfer[item] = "<项>";
	transfer[factor] = "<因子>";
	transfer[state] = "<语句>";
	transfer[assstate] = "<赋值语句>";
	transfer[constate] = "<条件语句>";
	transfer[condition] = "<条件>";
	transfer[loopstate] = "<循环语句>";
	transfer[stepsize] = "<步长>";
	transfer[callre] = "<有返回值函数调用语句>";
	transfer[callnore] = "<无返回值函数调用语句>";
	transfer[vparaList] = "<值参数表>";
	transfer[states] = "<语句列>";
	transfer[readstate] = "<读语句>";
	transfer[writestate] = "<写语句>";
	transfer[restate] = "<返回语句>";
}
