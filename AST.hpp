#ifndef AST_HPP
#define AST_HPP

#include "lexer.hpp"

enum class Node{
	Number,			//数値リテラル
	String,			//文字列リテラル
	BinaryOperator,	//二項演算子
	TernaryOperator,//三項演算子
	Assignment,		//代入演算子
	Variable,		//変数
	Statement,		//文
	BlockStatement,	//ブロック文
	Function,		//関数
	IfStatement		//If文 
};

class AST{
public:
	const virtual Node GetNodeType() = 0;	//ノードの種類を取得
};

class StatementNode : public AST{
private:
	AST *expr;
public:
	StatementNode(AST *expr) : expr(expr) {};
	const Node GetNodeType() override {return Node::Statement;};
	AST* GetExpression() {return expr;};
};

class BlockStatementNode : public AST{
private:
	std::vector<AST*> statements;	//文のリスト
	unsigned long long listNumber;	//リスト番号
public:
	BlockStatementNode(std::vector<AST*> statements) : statements(statements),listNumber(0) {};
	const Node GetNodeType() override {return Node::BlockStatement;};
	AST *ReadStatement() {return (listNumber < statements.size())?statements.at(listNumber++):nullptr;};
	AST *PeekStatement() {return (listNumber < statements.size())?statements.at(listNumber):nullptr;};
	void PushStatement(AST *ast) {statements.push_back(ast);};
};

class NumberNode : public AST{
private:
	std::string value;	//数値リテラルの値
public:
	NumberNode(std::string value) : value(value) {};
	const Node GetNodeType() override {return Node::Number;};
	const std::string GetValue() {return value;};
};

class StringNode : public AST{
private:
	std::string value;	//文字列リテラルの値
public:
	StringNode(std::string value) : value(value) {};
	const Node GetNodeType() override {return Node::String;};
	const std::string GetValue() {return value;};
};

class BinaryOperatorNode : public AST{
private:
	std::string operatorType;	//演算子の種類
	AST* left;	//左辺
	AST* right;	//右辺
public:
	BinaryOperatorNode(std::string operatorType, AST* left, AST* right) : operatorType(operatorType), left(left), right(right) {};
	const Node GetNodeType() override {return Node::BinaryOperator;};
	const std::string GetOperatorType() {return operatorType;};
	AST* GetLeft() {return left;};
	AST* GetRight() {return right;};
};

class TernaryOperatorNode : public AST{
private:
	std::string operatorType;	//演算子の種類
	AST* condition;	//条件式
	AST* trueExpr;	//真の場合の式
	AST* falseExpr;	//偽の場合の式
public:
	TernaryOperatorNode(std::string operatorType, AST* condition, AST* trueExpr, AST* falseExpr) : operatorType(operatorType), condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) {};
	const Node GetNodeType() override {return Node::TernaryOperator;};
	const std::string GetOperatorType() {return operatorType;};
	AST* GetCondition() {return condition;};
	AST* GetTrueExpr() {return trueExpr;};
	AST* GetFalseExpr() {return falseExpr;};
};

class AssignmentNode : public AST{
private:
	std::string variableName;	//変数名
	AST* expression;	//式
public:
	AssignmentNode(std::string variableName, AST* expression) : variableName(variableName), expression(expression) {};
	const Node GetNodeType() override {return Node::Assignment;};
	const std::string GetVariableName() {return variableName;};
	AST* GetExpression() {return expression;};
};

class VariableNode : public AST{
private:
	std::string variableName;	//変数名
public:
	VariableNode(std::string variableName) : variableName(variableName) {};
	const Node GetNodeType() override {return Node::Variable;};
	const std::string GetVariableName() {return variableName;};
};

class FunctionNode : public AST{
private:
	std::string functionName;	//関数名
	AST* argument;	//引数
public:
	FunctionNode(std::string functionName, AST* argument) : functionName(functionName), argument(argument) {};
	const Node GetNodeType() override {return Node::Function;};
	const std::string GetFunctionName() {return functionName;};
	AST* GetArgument() {return argument;};
};

class IfStatementNode : public AST{
private:
	AST* condition;	//条件式
	AST* trueExpr;	//真の場合の式
	AST* falseExpr;	//偽の場合の式
public:
	IfStatementNode(AST* condition, AST* trueExpr, AST* falseExpr) : condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) {};
	const Node GetNodeType() override {return Node::IfStatement;};
	AST* GetCondition() {return condition;};
	AST* GetTrueExpr() {return trueExpr;};
	AST* GetFalseExpr() {return falseExpr;};
};

#endif