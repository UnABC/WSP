#include "parser.hpp"

class Evaluator{
	private:
	TokenPtr currentToken;	//現在のトークン
	//各種変数格納map
	std::map<std::string,long double> varld;
	std::map<std::string,long long> varll;
	std::map<std::string,std::string> vars;
	template<typename T>
	T CalcExpr(AST *ast);
	void IfStatement(AST *node);
	void VoidFunction(AST *ast);
	void ProcessVariables(AST *ast);

	public:
	void evaluate(AST *ast);
};

