#ifndef CHECKTYPE_HPP
#define CHECKTYPE_HPP

#include "AST.hpp"

class VerifyType{
	private:
	std::map<std::string,int> var_type;	//変数

	std::map<std::string,int> system_var_type;	//システム定数
	std::map<std::string,int> system_func_type;	//関数
	public:
	VerifyType();
	void CheckType(AST *ast);
};

#endif

