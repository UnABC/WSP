#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "parser.hpp"
#include "Var.hpp"

class Evaluator {
private:
	TokenPtr currentToken;	//現在のトークン
	//各種変数格納map
	std::vector<std::map<std::string, Var>> var;
	std::vector<std::map<std::string, StaticVar>> static_var;
	//ユーザー定義関数
	std::map<std::string, std::pair<std::vector<AST*>, AST*>> user_func;
	//数学定数
	std::map<std::string, Var> math_const;
	Var CalcExpr(AST* ast);
	Var ProcessBinaryOperator(AST* left_node, AST* right, std::string operatorType, BinaryOperatorNode* node);
	void IfStatement(AST* node);
	void VoidFunction(AST* ast);
	void ProcessVariables(AST* ast,bool is_static,int type);
	void ProcessStaticVar(AST* ast);
	std::pair<Var,bool> ProcessFunction(AST* ast);
	//スコープ変数の管理
	inline void EnterScope() { var.emplace_back();static_var.emplace_back(); };
	inline void ExitScope() { var.pop_back();static_var.pop_back(); };
public:
	Evaluator();
	~Evaluator();
	void evaluate(AST* ast);
	void RegisterFunctions(AST* ast);
};
#endif // EVALUATOR_HPP

