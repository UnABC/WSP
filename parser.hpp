#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"


class Parser {
private:
	Lexer& lexer;	//字句解析器
	TokenPtr currentToken;	//現在のトークン

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
	AST* StaticVariable(TokenPtr token, int type);
public:
	Parser(Lexer& lexer) : lexer(lexer) {};
	AST* ParseStatement(TokenPtr token);
	void show(AST* ast);
};


#endif