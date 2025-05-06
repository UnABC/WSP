#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "parser.hpp"
#include "OperationType.hpp"

class Evaluator {
private:
	TokenPtr currentToken;	//現在のトークン
	//各種変数格納map
	//TODO:変数をすべてOperationTypeに統一する!!
	std::map<std::string, OperationType> var;
	//ユーザー定義関数
	std::map<std::string, std::pair<std::vector<AST*>, AST*>> user_func;
	//関数内のローカル変数
	std::map<AST *,OperationType> local_var;
	//数学定数
	std::map<std::string, OperationType> math_const;
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
public:
	Evaluator();
	void evaluate(AST* ast);
	void RegisterFunctions(AST* ast);
};
#endif // EVALUATOR_HPP

