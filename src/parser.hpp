#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"


class Parser {
private:
	Lexer& lexer;	//字句解析器
	TokenPtr currentToken;	//現在のトークン
	std::vector<bool> returnValue;	//戻り値があるかどうか
	std::vector<bool> while_statement;	//while文のスタック

	AST* ExprTernary();
	AST* ExprUnary();
	AST* ExprBool();
	AST* ExprAdd();
	AST* ExprMul();
	AST* ExprPrimary();
	AST* ExprNumber(TokenPtr token);
	AST* ExprString(TokenPtr token);
	AST* ExprFunction(TokenPtr token);
	AST* ExprAssignment(TokenPtr token);
	AST* Statement(TokenPtr token);
	AST* BlockStatement(TokenPtr token);
	AST* Declaration(int type);
	AST* Argument(int type);
public:
	Parser(Lexer& lexer) : lexer(lexer) {};
	AST* ParseStatement(TokenPtr token);
	//DEBUG用関数
	void show(AST* ast);
};


#endif