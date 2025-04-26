#ifndef CHECKTYPE_HPP
#define CHECKTYPE_HPP

#include "AST.hpp"

class VerifyType{
	private:
	std::map<std::string,int> var_type;	//変数
	public:
	void CheckType(AST *ast);
};

#endif

