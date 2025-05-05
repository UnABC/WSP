#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "parser.hpp"

class OperationType;

class Evaluator {
private:
	TokenPtr currentToken;	//現在のトークン
	//各種変数格納map
	std::map<std::string, long double> varld;
	std::map<std::string, long long> varll;
	std::map<std::string, std::string> vars;
	//数学定数
	std::map<std::string, long double> math_const;
	template<typename T>
	T CalcExpr(AST* ast);
	template<typename T>
	T ProcessBinaryOperator(AST* left_node, AST* right, std::string operatorType, BinaryOperatorNode* node);
	template<typename T, typename S>
	T ReturnProperType(S value);
	void IfStatement(AST* node);
	void VoidFunction(AST* ast);
	void ProcessVariables(AST* ast);
	void ProcessStaticVar(AST* ast);
protected:
	long long my_stoll(const std::string str) const;
	long double my_stold(const std::string str) const;
public:
	Evaluator();
	void evaluate(AST* ast);
};
#endif // EVALUATOR_HPP

