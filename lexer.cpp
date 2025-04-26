#include "lexer.hpp"
using namespace std;

TokenPtr Lexer::ExtractNextToken() {
	//トークンを取り出す
	//トークンが空なら読み込み、これ以上のトークンがない場合はEOFを返す
	if (queue.empty() && (!LoadTokens(1)))return EOFToken;

	TokenPtr token = std::make_shared<Token>(queue.front());
	queue.erase(queue.begin()); //先頭のトークンを削除
	return token;
}

TokenPtr Lexer::PeekTokens(int ReadTokensNum) {
	//トークンを指定数取り出し、これ以上のトークンがない場合はEOFを返す
	if ((ReadTokensNum >= queue.size()) && (!LoadTokens(ReadTokensNum))) return EOFToken;

	TokenPtr token = std::make_shared<Token>(queue[ReadTokensNum]);
	return token;
}

bool Lexer::LoadTokens(int ReadTokensNum) {
	//トークンを指定数読み込む
	while (ReadTokensNum >= queue.size()){
		if (!LoadNextLine())return false;
	}
	return true;
}

bool Lexer::LoadNextLine() {
	//次の行を読み込む
	string currentLine;
	lineNumber++;
	if (!getline(fileStream, currentLine)){
		queue.emplace_back(Token(TokenType::EndOfFile, "", lineNumber, 0)); 
		return false;
	} 
	if (currentLine.empty()) return true; //空行はスキップ

	AnalysisLine line(currentLine, lineNumber);
	//トークンを抽出する
	#define PUSH_BACK(TYPE) queue.emplace_back(Token(TokenType::TYPE, line.Read##TYPE(), lineNumber , line.column))
	#define PUSH_BACK_STR(quotaion) queue.emplace_back(Token(TokenType::StringLiteral, line.ReadStringLiteral(quotaion), lineNumber , line.column))

	while (line.column < currentLine.length()) {
		if (blockComment) {
			//ブロックコメント中
			line.SkipBlockComment(blockComment);
		}else if (line.isBlockComment()) {
			//ブロックコメント開始
			blockComment = true;
			line.SkipBlockComment(blockComment);
		}else if (line.isLineComment()) {
			//行コメント
			line.SkipLineComment();
		}else if (line.isWhiteSpace()) {
			//空白文字をスキップ
			line.SkipWhiteSpace();
		}else if (line.isStringLiteral('\"')) {
			//文字列リテラルを読み込む
			PUSH_BACK_STR('\"');
		}else if (line.isStringLiteral('\'')) {
			//文字リテラルを読み込む
			PUSH_BACK_STR('\'');
		}else if (line.isNumberLiteral()) {
			//数値リテラルを読み込む
			PUSH_BACK(NumberLiteral);
		}else if (line.isOperator()) {
			//演算子を読み込む
			PUSH_BACK(Operator);
		}else if (line.isEscape()) {
			//エスケープシーケンスを読み込む
			PUSH_BACK(Escape);
		}else if (line.isLParentheses()) {
			//左括弧を読み込む
			PUSH_BACK(LParentheses);
		}else if (line.isRParentheses()) {
			//右括弧を読み込む
			PUSH_BACK(RParentheses);
		}else if (line.isLBrace()) {
			//左波括弧を読み込む
			PUSH_BACK(LBrace);
		}else if (line.isRBrace()) {
			//右波括弧を読み込む
			PUSH_BACK(RBrace);
		}else if (line.isSymbol()) {
			//記号を読み込む
			PUSH_BACK(Symbol);
		}else{
			//識別子を読み込む
			PUSH_BACK(Identifier);
		}
	}
	//改行
	if ((!blockComment) && (!queue.empty()) && (queue.back().type != TokenType::EndOfLine)) {
		PUSH_BACK(EndOfLine);
	}
	#undef PUSH_BACK_STR
	#undef PUSH_BACK
	return true;
}

void AnalysisLine::SkipBlockComment(bool &blockComment) {
	//ブロックコメントをスキップ
	while (column < line.length()) {
		if (isEndBlockComment()) {
			column += 2; //*/をスキップ
			blockComment = false;
			break;
		}
		column++;
	}
	return;
}

string AnalysisLine::ReadStringLiteral(char quotaion) {
	//文字列リテラルを読み込む
	string value;
	column++; // "をスキップ
	while (!isEndStringLiteral(quotaion)) {
		value += line[column];
		column++;
	}
	if (column < line.length()) column++; // "をスキップ
	return value;
}

const bool AnalysisLine::isEndStringLiteral(char quotaion) {
	if (column >= line.length()){
		throw LexerException("String literal not closed.\n", lineNumber, column);
		return true;
	}
	return isStringLiteral(quotaion);
}

string AnalysisLine::ReadNumberLiteral() {
	//数値リテラルを読み込む
	string value;
	while (isNumberLiteral()) {
		value += line[column];
		column++;
	}
	return value;
}

string AnalysisLine::ReadOperator(){
	if (column+2 > line.length())return std::string(1, line[column++]);
	map<std::string,bool> operators = {
		{"==",true},{"!=",true},{"<=",true},
		{">=",true},{"&&",true},{"||",true},
		{"<<",true},{">>",true}
	};
	string double_operator = line.substr(column, 2);
	if (operators.count(double_operator)){
		column += 2; //演算子をスキップ
		return double_operator;
	}
	return std::string(1, line[column++]);
}

string AnalysisLine::ReadIdentifier() {
	//識別子を読み込む
	string value;
	while (isIdentifier()) {
		value += line[column];
		column++;
	}
	return value;
}

inline bool AnalysisLine::isIdentifier() {
	//識別子は演算子、記号、空白文字、文字列リテラル、数値リテラル、行コメント、ブロックコメント、ブロックコメント終了のいずれかでない場合
	return !(isWhiteSpace() || isOperator() || isSymbol()  || isNumberLiteral() || isLineComment() || isBlockComment() || isEndBlockComment() || isEscape() || isEndStringLiteral('\"') || isEndStringLiteral('\''));
}


#ifdef DEBUG_MODE
	const std::string Lexer::TokenTypeToString(TokenType type) {
		switch (type) {
			case TokenType::Identifier: return "Identifier";
			case TokenType::Operator: return "Operator";
			case TokenType::Symbol: return "Symbol";
			case TokenType::StringLiteral: return "String";
			case TokenType::NumberLiteral: return "Int";
			case TokenType::Escape: return "Escape";
			case TokenType::LParentheses: return "LParentheses";
			case TokenType::RParentheses: return "RParentheses";
			case TokenType::EndOfFile: return "EOF";
			case TokenType::EndOfLine: return "EOL";
			default: return "Unknown";
		}
	}
#endif