#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"

class Parser {
private:
	Lexer& lexer;	//字句解析器
	TokenPtr currentToken;	//現在のトークン
	std::vector<bool> returnValue;	//戻り値があるかどうか
	std::vector<bool> while_statement;	//while文のスタック

	//<行数,命令+引数>
	//命令一覧
	//0: スタックに引数を積む,1:スタックを複製する,2:スタックを交換する
	//3:スタックを削除する,4:スタックをコピーする,5:スタックを複数削除する
	//6:スタックをシャッフルする
	//7:加算,8:減算,9:乗算,10:除算,11:剰余
	//12:ヒープ書き込み,13:ヒープ読み込み
	std::map<unsigned long long, std::vector<std::pair<int, int>>> whitespace;

	AST* ExprTernary();
	AST* ExprUnary();
	AST* ExprBool();
	AST* ExprAdd();
	AST* ExprMul();
	AST* ExprPrimary();
	AST* ExprNumber(TokenPtr token);
	AST* ExprString(TokenPtr token);
	AST* ExprFunction(TokenPtr token);
	AST* ExprAssignment(TokenPtr token, std::string, AST* veriable);
	AST* ExprVariable(TokenPtr token, std::string& variableName);
	AST* Statement(TokenPtr token);
	AST* BlockStatement(TokenPtr token);
	AST* Declaration(int type);
	AST* Argument(int type);

	int AnalysisArg(std::string& whitespaceData, unsigned long long& position, unsigned long long& lineNumber);
public:
	Parser(Lexer& lexer) : lexer(lexer) {};
	AST* ParseStatement(TokenPtr token);
	std::map<unsigned long long, std::vector<std::pair<int, int>>> Whitespace();
};

#endif