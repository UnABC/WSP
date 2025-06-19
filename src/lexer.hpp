#ifndef LEXER_HPP
#define LEXER_HPP

#define DEBUG_MODE	//デバッグモード

#include <bits/stdc++.h>
#include "exception.hpp"

enum class TokenType {
	Identifier,		//識別子	
	Operator,		//演算子
	Symbol,			//記号
	LParentheses,	//左括弧
	RParentheses,	//右括弧
	LBrace,			//左波括弧
	RBrace,			//右波括弧
	LSquareBracket,	//左角括弧
	RSquareBracket,	//右角括弧
	StringLiteral,	//文字列リテラル
	NumberLiteral,	//数値リテラル
	Escape,			//エスケープシーケンス
	EndOfFile,		//EOF
	EndOfLine		//行末
};

struct Token {
	TokenType type;
	std::string value;
	unsigned long long lineNumber;		//行番号
	unsigned long long columnNumber;	//列番号

	Token(TokenType type, const std::string& value, unsigned long long lineNumber, unsigned long long columnNumber)
		: type(type), value(value), lineNumber(lineNumber), columnNumber(columnNumber) {
	};
};

//トークンポインタ
typedef std::shared_ptr<Token> TokenPtr;

class Lexer {
private:
	std::ifstream fileStream;	//ファイルストリーム
	std::vector<Token> queue;	//トークンキュー
	bool blockComment;	//ブロックコメントフラグ
	unsigned long long lineNumber;	//行番号

	bool LoadTokens(int ReadTokensNum);
	bool LoadNextLine();

	//EOFトークン
	TokenPtr EOFToken = std::make_shared<Token>(TokenType::EndOfFile, "", 0, 0);
public:
	explicit Lexer(std::string file_name) : fileStream(file_name), blockComment(false), lineNumber(0) {
		if (!fileStream.is_open()) throw std::runtime_error("File not found: " + file_name);
	};
	~Lexer() { fileStream.close(); };
	TokenPtr ExtractNextToken();
	TokenPtr PeekTokens(int ReadTokensNum);
#ifdef DEBUG_MODE
	TokenPtr PeekToken(int ReadTokensNum) {
		if (ReadTokensNum >= queue.size()) LoadTokens(ReadTokensNum);
		return std::make_shared<Token>(queue[ReadTokensNum]);
	};
	const std::string TokenTypeToString(TokenType type);
#endif
};

class AnalysisLine {
private:
	std::string line;	//行内容
	unsigned long long lineNumber;	//行番号
	unsigned long long column;		//列番号
	friend class Lexer;
	explicit AnalysisLine(std::string line, unsigned long long lineNumber) : line(line), column(0), lineNumber(lineNumber) {};
	char NextChar(int gap) { return line[column + gap]; };
	char top() { return line[column]; };

	//各種検出関数
	//コメント検出
	bool isLineComment() { return (line[column] == '/') && (column + 1 < line.length()) && (line[column + 1] == '/'); };
	bool isBlockComment() { return (line[column] == '/') && (column + 1 < line.length()) && (line[column + 1] == '*'); };
	bool isEndBlockComment() { return (line[column] == '*') && (column + 1 < line.length()) && (line[column + 1] == '/'); };
	//Space検出
	bool isWhiteSpace() { return (line[column] == ' ') || (line[column] == '\t'); };
	//String検出
	bool isStringLiteral(char quotaion) { return (line[column] == quotaion); };
	//Number検出
	bool isNumberLiteral() {
		//数値リテラルは数字または小数点で始まる
		return ('0' <= line[column] && line[column] <= '9') || (line[column] == '.');
	};
	//演算子検出
	const bool isOperator() { return strchr("+-*/<=>!%&|^", line[column]); };
	//エスケープシーケンス検出
	const bool isEscape() { return (line[column] == '\\') && (column + 1 < line.length()) && ((bool)strchr("\\ntr0\'\"?afv", line[column + 1])); };
	//丸括弧検出
	const bool isLParentheses() { return (line[column] == '('); };
	const bool isRParentheses() { return (line[column] == ')'); };
	//波括弧検出
	const bool isLBrace() { return (line[column] == '{'); };
	const bool isRBrace() { return (line[column] == '}'); };
	//角括弧検出
	const bool isLSquareBracket() { return (line[column] == '['); };
	const bool isRSquareBracket() { return (line[column] == ']'); };
	//各種記号検出
	const bool isSymbol() { return strchr(";,:()[]{}#$~@?\\", line[column]); };

	//各種処理関数
	//コメント等の処理
	inline void SkipWhiteSpace() { while (isWhiteSpace()) column++; };
	inline void SkipLineComment() { column = (long long)line.size(); };
	inline void SkipBlockComment(bool& blockComment);
	//String等の処理
	std::string ReadStringLiteral(char quotaion);
	inline const bool isEndStringLiteral(char quotaion) { return isStringLiteral(quotaion); };
	std::string ReadCharLiteral();
	//数値の処理
	std::string ReadNumberLiteral();
	//演算子の処理
	std::string ReadOperator();
	//エスケープシーケンスの処理
	inline std::string ReadEscape() { column += 2;return line.substr(column - 2, 2); };
	//丸括弧の処理
	inline std::string ReadLParentheses() { return ReadOperator(); };
	inline std::string ReadRParentheses() { return ReadOperator(); };
	//波括弧の処理
	inline std::string ReadLBrace() { return ReadOperator(); };
	inline std::string ReadRBrace() { return ReadOperator(); };
	//角括弧の処理
	inline std::string ReadLSquareBracket() { return ReadOperator(); };
	inline std::string ReadRSquareBracket() { return ReadOperator(); };
	//その他記号の処理
	inline std::string ReadSymbol() { return ReadOperator(); };
	//識別子の処理
	std::string ReadIdentifier();
	inline bool isIdentifier();
	//改行
	inline const std::string ReadEndOfLine()const noexcept { return "EOL"; };
};

#endif // LEXER_HPP