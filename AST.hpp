/*仕様(メモ)
type;0:int,1:double,2:string,3:void,-1:不定,-2:未定義
*/
#ifndef AST_HPP
#define AST_HPP

#include "lexer.hpp"

enum class Node {
	Number,			            //数値リテラル
	String,			            //文字列リテラル
	UnaryOperator,	            //単項演算子
	BinaryOperator,	            //二項演算子
	TernaryOperator,            //三項演算子
	Assignment,		            //代入演算子
	Variable,		            //変数
	StaticVarWithAssignment,	//静的変数(初期化あり)
	StaticVarWithoutAssignment, //静的変数(初期化なし)
	Statement,		            //文
	BlockStatement,	            //ブロック文
	Function,		            //関数
	DefFunction,	            //関数定義
	ArgumentNode,	        	//関数の引数
	IfStatement,		        //If文
	ReturnStatement,	        //Return文 
};

class AST {
public:
	const virtual Node GetNodeType() = 0;	//ノードの種類を取得
	const virtual int GetType() { return -2; };	//式の型を取得
	virtual void SetType(int type) { throw std::runtime_error("Critical Error!(info:CheckType)"); };
};

class StatementNode : public AST {
private:
	AST* expr;
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	StatementNode(AST* expr, unsigned long long lineNumber, unsigned long long columnNumber)
		: expr(expr), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::Statement; };
	AST* GetExpression() { return expr; };
};

class BlockStatementNode : public AST {
private:
	std::vector<AST*> statements;	//文のリスト
	unsigned long long listNumber;	//リスト番号
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	BlockStatementNode(std::vector<AST*> statements, unsigned long long lineNumber, unsigned long long columnNumber)
		: statements(statements), listNumber(0), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::BlockStatement; };
	AST* ReadStatement() { return (listNumber < statements.size()) ? statements.at(listNumber++) : nullptr; };
	AST* PeekStatement() { return (listNumber < statements.size()) ? (statements.at(listNumber)) : nullptr; };
	void PushStatement(AST* ast) { statements.push_back(ast); };
	void ResetListNumber() { listNumber = 0; };
};

class NumberNode : public AST {
private:
	std::string value;	//数値リテラルの値
	//式の型(0:int,1:double)
	int type = -2;
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	NumberNode(std::string value, int type, unsigned long long lineNumber, unsigned long long columnNumber)
		: value(value), type(type), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::Number; };
	const std::string GetValue() { return value; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
};

class StringNode : public AST {
private:
	std::string value;	//文字列リテラルの値
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	StringNode(std::string value, unsigned long long lineNumber, unsigned long long columnNumber)
		: value(value), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::String; };
	const std::string GetValue() { return value; };
	const inline int GetType() { return 2; };
};

class UnaryOperatorNode : public AST {
private:
	AST* expression;	//式
	int type = -2;	//式の型(0:int,1:double)
	int operatorType = 0;	//演算子の種類(0:!,1:-)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	UnaryOperatorNode(AST* expression,int operatorType, unsigned long long lineNumber, unsigned long long columnNumber)
		: expression(expression),operatorType(operatorType), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::UnaryOperator; };
	AST* GetExpression() { return expression; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
	const int GetOperatorType() { return operatorType; };
};

class BinaryOperatorNode : public AST {
private:
	std::string operatorType;	//演算子の種類
	AST* left;	//左辺
	AST* right;	//右辺
	int type = -2;	//式の型(0:int,1:double)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	BinaryOperatorNode(std::string operatorType, AST* left, AST* right, unsigned long long lineNumber, unsigned long long columnNumber)
		: operatorType(operatorType), left(left), right(right), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::BinaryOperator; };
	const std::string GetOperatorType() { return operatorType; };
	AST* GetLeft() { return left; };
	AST* GetRight() { return right; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
};

class TernaryOperatorNode : public AST {
private:
	std::string operatorType;	//演算子の種類
	AST* condition;	//条件式
	AST* trueExpr;	//真の場合の式
	AST* falseExpr;	//偽の場合の式
	int type;	//式の型(0:int,1:double)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	TernaryOperatorNode(std::string operatorType, AST* condition, AST* trueExpr, AST* falseExpr, unsigned long long lineNumber, unsigned long long columnNumber)
		: operatorType(operatorType), condition(condition), trueExpr(trueExpr), falseExpr(falseExpr), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::TernaryOperator; };
	const std::string GetOperatorType() { return operatorType; };
	AST* GetCondition() { return condition; };
	AST* GetTrueExpr() { return trueExpr; };
	AST* GetFalseExpr() { return falseExpr; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
};

class AssignmentNode : public AST {
private:
	std::string variableName;	//変数名
	AST* variable;		//変数
	AST* expression;	//式
	int type;	//式の型(0:int,1:double)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	AssignmentNode(std::string variableName, AST* variable, AST* expression, unsigned long long lineNumber, unsigned long long columnNumber)
		: variableName(variableName), variable(variable), expression(expression), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::Assignment; };
	const std::string GetVariableName() { return variableName; };
	AST* GetExpression() { return expression; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
};

class VariableNode : public AST {
private:
	std::string variableName;	//変数名
	int type = -2;	//式の型(0:int,1:double)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	VariableNode(std::string variableName, unsigned long long lineNumber, unsigned long long columnNumber)
		: variableName(variableName), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::Variable; };
	const std::string GetVariableName() { return variableName; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
};

class SystemFunctionNode : public AST {
private:
	std::string functionName;	//関数名
	std::vector<AST*> argument;	//引数
	int type = -2;	//式の型(0:int,1:double,2:string,3:void)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	SystemFunctionNode(std::string functionName, std::vector<AST*> argument, unsigned long long lineNumber, unsigned long long columnNumber)
		: functionName(functionName), argument(argument), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::Function; };
	const std::string GetFunctionName() { return functionName; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
	std::vector<AST*> GetArgument() { return argument; };
	const int GetArgumentSize() { return argument.size(); };
};

class UserFunctionNode : public AST {
private:
	std::string functionName;	//関数名
	std::vector<AST*> argument;	//引数
	AST* blockStatement;	//関数の中身
	int type = -2;	//式の型(0:int,1:double,2:string,3:void)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	UserFunctionNode(std::string functionName, std::vector<AST*> argument, AST* blockStatement, int type, unsigned long long lineNumber, unsigned long long columnNumber)
		: functionName(functionName), argument(argument), blockStatement(blockStatement), type(type), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::DefFunction; };
	const std::string GetFunctionName() { return functionName; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
	AST* GetBlockStatement() { return blockStatement; };
	std::vector<AST*> GetArgument() { return argument; };
	const int GetArgumentSize() { return argument.size(); };
};

class ArgumentNode : public AST {
private:
	AST* variable;	//変数
	int type = -2;	//式の型(0:int,1:double,2:string,3:var)
	bool isAssigned = false;	//デフォルト値があるかどうか
	AST* defaultValue;	//デフォルト値
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	ArgumentNode(AST* variable, int type, bool isAssigned, AST* defaultValue, unsigned long long lineNumber, unsigned long long columnNumber)
		: variable(variable), type(type), isAssigned(isAssigned), defaultValue(defaultValue), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::ArgumentNode; };
	const int GetType() override { return type; };
	void SetType(int type) override { this->type = type; };
	bool IsAssigned() { return isAssigned; };
	AST* GetVariable() { return variable; };
	AST* GetDefaultValue() { return defaultValue; };
};

class IfStatementNode : public AST {
private:
	AST* condition;	//条件式
	AST* trueStatement;	//真の場合の文
	AST* falseStatement;	//偽の場合の文
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	IfStatementNode(AST* condition, AST* trueStatement, AST* falseStatement, unsigned long long lineNumber, unsigned long long columnNumber)
		: condition(condition), trueStatement(trueStatement), falseStatement(falseStatement), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::IfStatement; };
	AST* GetCondition() { return condition; };
	AST* GetTrueExpr() { return trueStatement; };
	AST* GetFalseExpr() { return falseStatement; };
};

class StaticVariableNode : public AST {
private:
	AST* assignment;
	int type = -2;	//式の型(0:int,1:double)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	StaticVariableNode(AST* assignment, int type, unsigned long long lineNumber, unsigned long long columnNumber)
		: assignment(assignment), type(type), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::StaticVarWithAssignment; };
	const int GetType() override { return type; };
	AST* GetAssignment() { return assignment; };
};

class StaticVarNodeWithoutAssignment : public AST {
private:
	std::string variableName;	//変数名
	int type = -2;	//式の型
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	StaticVarNodeWithoutAssignment(std::string variableName, int type, unsigned long long lineNumber, unsigned long long columnNumber)
		: variableName(variableName), type(type), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::StaticVarWithoutAssignment; };
	const std::string GetVariableName() { return variableName; };
	const int GetType() override { return type; };
};

class ReturnStatementNode : public AST {
private:
	AST* expression;	//式(戻り値)
public:
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
	ReturnStatementNode(AST* expression, unsigned long long lineNumber, unsigned long long columnNumber)
		: expression(expression), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
	const Node GetNodeType() override { return Node::ReturnStatement; };
	AST* GetExpression() { return expression; };
};

#endif