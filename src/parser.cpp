#include "parser.hpp"
using namespace std;

//文の解析
AST* Parser::ParseStatement(TokenPtr token) {
	currentToken = token;
	if (currentToken->type == TokenType::EndOfFile) return nullptr; //EOFなら終了
	return Statement(currentToken);
}
//三項演算子を解析する
AST* Parser::ExprTernary() {
	AST* left = ExprBool();
	if (currentToken->type == TokenType::Symbol && currentToken->value == "?") {
		currentToken = lexer.ExtractNextToken(); //?をスキップ
		AST* trueExpr = ExprBool();
		if (currentToken->type != TokenType::Symbol || currentToken->value != ":") {
			throw ParserException("Not found \":\".", currentToken->lineNumber, currentToken->columnNumber);
		}
		currentToken = lexer.ExtractNextToken(); //:をスキップ
		AST* falseExpr = ExprBool();
		return new TernaryOperatorNode("?", left, trueExpr, falseExpr, currentToken->lineNumber, currentToken->columnNumber); //三項演算子ノードを作成
	}
	return left;
}

//二項演算子を解析する
AST* Parser::ExprBool() {
	AST* left = ExprAdd();
	map<string, bool> operators = {
		{">" ,true},{"<" ,true},
		{"==",true},{"!=",true},{"<=",true},
		{">=",true},{"&&",true},{"||",true},
		{"<<",true},{">>",true},{"+=",true},
		{"-=",true},{"*=",true},{"/=",true},
		{"%=",true},{"&=",true},{"|=",true},
		{"++",true},{"--",true},
		{"<<=",true},{">>=",true},
		{"^=",true},{"&",true},
		{"^",true},{"|",true}
	};
	while ((currentToken->type == TokenType::Operator) && ((bool)operators.count(currentToken->value))) {
		if (currentToken->value == "++" || currentToken->value == "--") {
			//インクリメント・デクリメント演算子の処理
			string operatorType = (currentToken->value == "++") ? "+=" : "-=";
			currentToken = lexer.ExtractNextToken(); //演算子をスキップ
			AST* right = new NumberNode("1", 0, currentToken->lineNumber, currentToken->columnNumber);
			left = new BinaryOperatorNode(operatorType, left, right, currentToken->lineNumber, currentToken->columnNumber);
		} else {
			string operatorType = currentToken->value;
			currentToken = lexer.ExtractNextToken(); //演算子をスキップ
			AST* right = ExprAdd();
			left = new BinaryOperatorNode(operatorType, left, right, currentToken->lineNumber, currentToken->columnNumber); //二項演算子ノードを作成
		}
	}
	return left;
}

AST* Parser::ExprAdd() {
	//加算式を解析する
	AST* left = ExprMul();
	while ((currentToken->type == TokenType::Operator) && (currentToken->value == "+" || currentToken->value == "-")) {
		string operatorType = currentToken->value;
		currentToken = lexer.ExtractNextToken(); //演算子をスキップ
		AST* right = ExprMul();
		left = new BinaryOperatorNode(operatorType, left, right, currentToken->lineNumber, currentToken->columnNumber); //二項演算子ノードを作成
	}
	return left;
}

AST* Parser::ExprMul() {
	//乗算式を解析する
	AST* left = ExprUnary();
	while ((currentToken->type == TokenType::Operator) && (currentToken->value == "*" || currentToken->value == "/" || currentToken->value == "%")) {
		string operatorType = currentToken->value;
		currentToken = lexer.ExtractNextToken(); //演算子をスキップ
		AST* right = ExprUnary();
		left = new BinaryOperatorNode(operatorType, left, right, currentToken->lineNumber, currentToken->columnNumber); //二項演算子ノードを作成
	}
	return left;
}

AST* Parser::ExprUnary() {
	//単項演算子を解析する
	if ((currentToken->type == TokenType::Operator) && (currentToken->value == "!" || currentToken->value == "-")) {
		string operatorType = currentToken->value;
		currentToken = lexer.ExtractNextToken(); //演算子をスキップ
		AST* expression = ExprPrimary();
		return new UnaryOperatorNode(expression, (operatorType == "-"), currentToken->lineNumber, currentToken->columnNumber); //単項演算子ノードを作成
	}
	return ExprPrimary();
}

AST* Parser::ExprPrimary() {
	TokenPtr nextToken = lexer.PeekTokens(0); //次のトークンを取得
	//基本式を解析する
	if (currentToken->type == TokenType::NumberLiteral) {
		return ExprNumber(currentToken);
	} else if (currentToken->type == TokenType::StringLiteral) {
		return ExprString(currentToken);
	} else if (currentToken->type == TokenType::Identifier) {
		if (nextToken->type == TokenType::LParentheses) {
			//関数の処理
			return ExprFunction(currentToken);
		} else if (nextToken->type == TokenType::Identifier) {
			if (currentToken->value == "int") {
				//int型の変数の処理
				return Declaration(0);
			} else if (currentToken->value == "double") {
				//double型の変数の処理
				return Declaration(1);
			} else if (currentToken->value == "string") {
				//string型の変数の処理
				return Declaration(2);
			}
		}
		string variableName;
		AST* ast = ExprVariable(currentToken, variableName); //変数を解析する
		//代入演算子の処理
		return ((currentToken->type == TokenType::Operator) && (currentToken->value == "=")) ? ExprAssignment(currentToken, variableName, ast) : ast;
	} else if (currentToken->type == TokenType::LParentheses) {
		currentToken = lexer.ExtractNextToken(); //左括弧をスキップ
		AST* left = ExprTernary();
		if (currentToken->type != TokenType::RParentheses)
			throw ParserException("Not found right parenthesis.", currentToken->lineNumber, currentToken->columnNumber);
		currentToken = lexer.ExtractNextToken(); //右括弧をスキップ
		return left;
		//Array型
	} else if (currentToken->type == TokenType::LSquareBracket) {
		currentToken = lexer.ExtractNextToken(); //左角括弧をスキップ
		vector<AST*> elements;
		while (currentToken->type != TokenType::RSquareBracket) {
			if (currentToken->type == TokenType::EndOfLine) {
				currentToken = lexer.ExtractNextToken(); //行末をスキップ
				continue;
			}
			if (currentToken->type == TokenType::EndOfFile)
				throw ParserException("Not found right square bracket.", currentToken->lineNumber, currentToken->columnNumber);
			elements.push_back(ExprTernary()); //要素を解析する
			if (currentToken->type == TokenType::Symbol && currentToken->value == ",") {
				currentToken = lexer.ExtractNextToken(); //,をスキップ
			} else if (currentToken->type == TokenType::EndOfLine) {
				currentToken = lexer.ExtractNextToken(); //行末をスキップ
			} else if (currentToken->type != TokenType::RSquareBracket) {
				throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpect ] or ,", currentToken->lineNumber, currentToken->columnNumber);
			}
		}
		currentToken = lexer.ExtractNextToken(); //右角括弧をスキップ
		return new ArrayNode(elements, currentToken->lineNumber, currentToken->columnNumber); //配列ノードを作成
	}
	throw ParserException("Invalid token.\"" + currentToken->value + "\"", currentToken->lineNumber, currentToken->columnNumber);
}

AST* Parser::ExprVariable(TokenPtr token, string& variableName) {
	//変数を解析する
	unsigned long long tmp_lineNumber = token->lineNumber;
	unsigned long long tmp_columnNumber = token->columnNumber;
	variableName = token->value;
	currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	AST* ast;
	if (currentToken->type == TokenType::LSquareBracket) {
		vector<AST*> arrayIndex; //配列のインデックスを格納するベクター
		//配列のインデックスを解析する
		while (currentToken->type == TokenType::LSquareBracket) {
			currentToken = lexer.ExtractNextToken(); //左角括弧をスキップ
			if (currentToken->type == TokenType::RSquareBracket) {
				throw ParserException("Not found array index.", currentToken->lineNumber, currentToken->columnNumber);
			} else {
				arrayIndex.push_back(ExprTernary()); //インデックスを解析する
			}
			if (currentToken->type != TokenType::RSquareBracket)
				throw ParserException("Not found right square bracket.", currentToken->lineNumber, currentToken->columnNumber);
			currentToken = lexer.ExtractNextToken(); //右角括弧をスキップ
		}
		ast = new VariableNode(variableName, arrayIndex, tmp_lineNumber, tmp_columnNumber); //変数ノードを作成
	} else {
		ast = new VariableNode(variableName, {}, tmp_lineNumber, tmp_columnNumber); //変数ノードを作成
	}
	return ast;
}

AST* Parser::ExprNumber(TokenPtr token) {
	//数値リテラルを解析する
	int type = (token->value.find(".") == string::npos) ? 0 : 1;
	AST* ast = new NumberNode(token->value, type, token->lineNumber, token->columnNumber);
	currentToken = lexer.ExtractNextToken(); //トークンを進める
	return ast;
}

AST* Parser::ExprString(TokenPtr token) {
	//文字列リテラルを解析する
	AST* ast = new StringNode(token->value, token->lineNumber, token->columnNumber);
	currentToken = lexer.ExtractNextToken(); //トークンを進める
	return ast;
}

AST* Parser::ExprAssignment(TokenPtr token, string variableName, AST* variable) {
	currentToken = lexer.ExtractNextToken(); //演算子をスキップ
	AST* right = ExprTernary(); //式を解析する
	return new AssignmentNode(variableName, variable, right, currentToken->lineNumber, currentToken->columnNumber); //代入ノードを作成
}

AST* Parser::ExprFunction(TokenPtr token) {
	//関数を解析する
	string functionName = token->value;
	currentToken = lexer.ExtractNextToken(); //関数名をスキップ
	vector<AST*> args;
	do {
		currentToken = lexer.ExtractNextToken(); //(or,をスキップ
		if (currentToken->type == TokenType::RParentheses) break; //引数がない場合は終了
		if (currentToken->type == TokenType::Symbol && currentToken->value == ",") {
			args.push_back(nullptr); //引数なし
			continue;
		}
		args.push_back(ExprTernary()); //引数を解析する
	} while (currentToken->type == TokenType::Symbol && currentToken->value == ",");
	currentToken = lexer.ExtractNextToken(); //)をスキップ
	AST* ast = new SystemFunctionNode(functionName, args, currentToken->lineNumber, currentToken->columnNumber); //関数ノードを作成
	return ast;
}

AST* Parser::Statement(TokenPtr token) {
	//文を解析する
	if (currentToken->type == TokenType::Identifier) {
		//システムの予約語を処理する
		//If文
		if (currentToken->value == "if") {
			currentToken = lexer.ExtractNextToken(); //ifをスキップ
			AST* condition = ExprTernary(); //条件式を解析する
			//真の場合の文を解析する
			AST* trueExpr = (currentToken->type == TokenType::LBrace) ? BlockStatement(currentToken) : Statement(currentToken);
			AST* falseExpr = nullptr;
			//else以降が存在すれば処理する
			if (currentToken->type == TokenType::Identifier && currentToken->value == "else") {
				currentToken = lexer.ExtractNextToken(); //elseをスキップ
				falseExpr = (currentToken->type == TokenType::LBrace) ? BlockStatement(currentToken) : Statement(currentToken);
			}
			return new IfStatementNode(condition, trueExpr, falseExpr, currentToken->lineNumber, currentToken->columnNumber); //if文ノードを作成
		} else if (currentToken->value == "int") {
			return Declaration(0); //int型の静的変数を解析する
		} else if (currentToken->value == "double") {
			return Declaration(1); //double型の静的変数を解析する
		} else if (currentToken->value == "string") {
			return Declaration(2); //string型の静的変数を解析する
		} else if (currentToken->value == "void") {
			return Declaration(3); //void型の関数を解析する
		} else if (currentToken->value == "return") {
			currentToken = lexer.ExtractNextToken(); //returnをスキップ
			AST* expression = ((currentToken->type == TokenType::EndOfLine) || (currentToken->type == TokenType::RBrace)) ? nullptr : ExprTernary(); //戻り値を解析する
			if ((currentToken->type != TokenType::EndOfLine) && (currentToken->type != TokenType::EndOfFile) && (currentToken->type != TokenType::RBrace))
				throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected EOL.", currentToken->lineNumber, currentToken->columnNumber);
			if (returnValue.size() == 0)
				throw ParserException("Return Statement should be in function.", currentToken->lineNumber, currentToken->columnNumber);
			if (returnValue.back() && (expression == nullptr))
				throw ParserException("Return value is required.", currentToken->lineNumber, currentToken->columnNumber);
			if (!returnValue.back() && (expression != nullptr))
				throw ParserException("Void function should not return value.", currentToken->lineNumber, currentToken->columnNumber);
			return new ReturnStatementNode(expression, currentToken->lineNumber, currentToken->columnNumber); //return文ノードを作成
		} else if ((currentToken->value == "while") || (currentToken->value == "repeat")) {
			int mode = (currentToken->value == "repeat"); //モードを設定
			currentToken = lexer.ExtractNextToken(); //whileをスキップ
			AST* condition = ExprTernary(); //条件式を解析する
			while_statement.push_back(true); //while文のスタックに追加
			AST* block = (currentToken->type == TokenType::LBrace) ? BlockStatement(currentToken) : Statement(currentToken);
			while_statement.pop_back(); //while文のスタックから削除
			return new WhileStatementNode(condition, block, mode, currentToken->lineNumber, currentToken->columnNumber); //while文ノードを作成
		} else if ((currentToken->value == "break") || (currentToken->value == "continue")) {
			//ループ内にあるかどうか
			if (while_statement.empty())
				throw ParserException("Break or Continue statement should be in loop.", currentToken->lineNumber, currentToken->columnNumber);
			//break文またはcontinue文を解析する
			int statementType = 2 + (currentToken->value == "continue");
			currentToken = lexer.ExtractNextToken(); //breakまたはcontinueをスキップ
			if (currentToken->type != TokenType::EndOfLine && currentToken->type != TokenType::EndOfFile && currentToken->type != TokenType::RBrace)
				throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected EOL or EOF.", currentToken->lineNumber, currentToken->columnNumber);
			return new JumpStatementNode(statementType, currentToken->lineNumber, currentToken->columnNumber); //break,continueノードを作成
		}
	} else if (currentToken->type == TokenType::LBrace) {
		//ブロック文を解析する
		return BlockStatement(token);
	}
	//式の処理
	AST* ast = ExprTernary(); //式を解析する
	if ((currentToken->type != TokenType::EndOfFile) && (currentToken->type != TokenType::EndOfLine) && (currentToken->type != TokenType::RBrace))
		throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected EOF or EOL.", currentToken->lineNumber, currentToken->columnNumber);
	return ast;
}

AST* Parser::BlockStatement(TokenPtr token) {
	//ブロック文を解析する
	currentToken = token;
	currentToken = lexer.ExtractNextToken(); //左波括弧をスキップ
	if (currentToken->type == TokenType::EndOfLine)
		currentToken = lexer.ExtractNextToken(); //行末をスキップ
	vector<AST*> statements;
	while (currentToken->type != TokenType::RBrace) {
		if (currentToken->type == TokenType::EndOfFile)
			throw ParserException("Not found right brace.", currentToken->lineNumber, currentToken->columnNumber);
		if (AST* ast = Statement(currentToken)) statements.push_back(ast);
		if (currentToken->type == TokenType::EndOfLine)
			currentToken = lexer.ExtractNextToken(); //行末をスキップ
	}
	currentToken = lexer.ExtractNextToken(); //右波括弧をスキップ
	return new BlockStatementNode(statements, currentToken->lineNumber, currentToken->columnNumber);
}

AST* Parser::Declaration(int type) {
	//静的変数or関数を解析する
	currentToken = lexer.ExtractNextToken(); //型をスキップ
	TokenPtr nextToken = lexer.PeekTokens(0); //次のトークンを取得
	vector<AST*> arrayIndex; //配列のインデックスを格納するベクター
	if ((type != 3) && (nextToken->type == TokenType::Operator) && (nextToken->value == "=")) {
		//静的変数の初期化
		string variableName;
		AST* variable = ExprVariable(currentToken, variableName); //変数を解析する
		AST* assignment = ExprAssignment(currentToken, variableName, variable); //型の変数を宣言&代入
		return new StaticVariableNode(assignment, type, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
	} else if (nextToken->type == TokenType::LParentheses) {
		//関数の初期化
		string functionName = currentToken->value; //関数名を取得
		currentToken = lexer.ExtractNextToken(); //関数名をスキップ
		currentToken = lexer.ExtractNextToken(); //(をスキップ
		returnValue.push_back(type != 3); //戻り値の有無を記録
		vector<AST*> args;
		while (currentToken->type != TokenType::RParentheses) {
			if (currentToken->type == TokenType::Identifier) {
				if (currentToken->value == "int") {
					args.push_back(Argument(0)); //int型の引数を解析する
				} else if (currentToken->value == "double") {
					args.push_back(Argument(1)); //double型の引数を解析する
				} else if (currentToken->value == "string") {
					args.push_back(Argument(2)); //string型の引数を解析する
				} else if (currentToken->value == "var") {
					//参照渡し
					args.push_back(Argument(3)); //参照渡し	
				} else {
					throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected int, double, string or var.", currentToken->lineNumber, currentToken->columnNumber);
				}
			} else if (currentToken->type == TokenType::Symbol && currentToken->value == ",")
				currentToken = lexer.ExtractNextToken(); //,をスキップ
			else throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected int, double, string or var.", currentToken->lineNumber, currentToken->columnNumber);
		}
		currentToken = lexer.ExtractNextToken(); //)をスキップ
		if (currentToken->type != TokenType::LBrace)
			throw ParserException("Not found function contents.", currentToken->lineNumber, currentToken->columnNumber);
		AST* block = BlockStatement(currentToken); //関数の内容を解析する
		returnValue.pop_back(); //戻り値の有無を削除
		return new UserFunctionNode(functionName, args, block, type, currentToken->lineNumber, currentToken->columnNumber); //関数ノードを作成
	} else if (nextToken->type == TokenType::LSquareBracket) {
		string variableName = currentToken->value; //変数名を取得
		type += 10;
		currentToken = lexer.ExtractNextToken(); //識別子をスキップ
		while (currentToken->type == TokenType::LSquareBracket) {
			currentToken = lexer.ExtractNextToken(); //左角括弧をスキップ
			if (currentToken->type == TokenType::RSquareBracket) {
				throw ParserException("Not found array index.", currentToken->lineNumber, currentToken->columnNumber);
			} else {
				arrayIndex.push_back(ExprTernary()); //インデックスを解析する
			}
			if (currentToken->type != TokenType::RSquareBracket)
				throw ParserException("Not found right square bracket.", currentToken->lineNumber, currentToken->columnNumber);
			currentToken = lexer.ExtractNextToken(); //右角括弧をスキップ
		}
		if (currentToken->type == TokenType::Operator && currentToken->value == "=") {
			//静的変数の初期化
			AST* variable = new VariableNode(variableName, arrayIndex, currentToken->lineNumber, currentToken->columnNumber); //変数ノードを作成
			AST* assignment = ExprAssignment(currentToken, variableName, variable); //型の変数を宣言&代入
			return new StaticVariableNode(assignment, type, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
		}
		return new StaticVarNodeWithoutAssignment(variableName, type, arrayIndex, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
	}
	if (type == 3)
		throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected function declaration.", currentToken->lineNumber, currentToken->columnNumber);
	//静的変数(初期値なし)
	vector<string> variableNames; //変数名を格納するベクター
	variableNames.push_back(currentToken->value); //変数名を取得
	currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	while ((currentToken->type == TokenType::Symbol) && (currentToken->value == ",")) {
		currentToken = lexer.ExtractNextToken(); //,をスキップ
		variableNames.push_back(currentToken->value); //変数名を取得
		currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	}
	AST* ast = new StaticVarNodeWithoutAssignment(variableNames, type, arrayIndex, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
	//currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	return ast;
}

AST* Parser::Argument(int type) {
	currentToken = lexer.ExtractNextToken(); //型をスキップ
	TokenPtr nextToken = lexer.PeekTokens(0); //次のトークンを取得
	string variableName = currentToken->value; //変数名を取得
	AST* ast;
	bool isReference = (type == 3);
	//if (type == 3) {
		//参照渡し
	ast = new VariableNode(variableName, vector<AST*>(), currentToken->lineNumber, currentToken->columnNumber); //変数ノードを作成
	ast->SetType(type); //型をintで仮設定
	// } else {
	// 	//値渡し
	// 	ast = new StaticVarNodeWithoutAssignment(variableName, type, vector<AST*>(), currentToken->lineNumber, currentToken->columnNumber);
	// }
	currentToken = lexer.ExtractNextToken(); //変数名をスキップ
	if ((currentToken->type == TokenType::Operator) && (currentToken->value == "=")) {
		//デフォルト引数有
		currentToken = lexer.ExtractNextToken(); //=をスキップ
		AST* assignment = ExprTernary(); //式を解析する
		return new ArgumentNode(ast, type, true, isReference, assignment, currentToken->lineNumber, currentToken->columnNumber); //引数ノードを作成
	} else {
		//デフォルト引数無
		return new ArgumentNode(ast, type, false, isReference, nullptr, currentToken->lineNumber, currentToken->columnNumber); //引数ノードを作成
	}
}

int Parser::AnalysisArg(string& whitespaceData, unsigned long long& position, unsigned long long& lineNumber) {
	string arg = "0";
	bool Negative = (whitespaceData[position++] == '\t');
	while (whitespaceData[position] != '\n') 
		arg += (whitespaceData[position++] == ' ') ? '0' : '1';
	lineNumber++;
	position++;
	return (Negative) ? -stoi(arg, nullptr, 2) : stoi(arg, nullptr, 2);
}

std::map<unsigned long long, std::vector<std::pair<int, int>>> Parser::Whitespace() {
	//whitepaceの解析スタート！！
	unsigned long long lineNumber = 0;
	unsigned long long position = 0;
	string whitespaceData = lexer.GetWhitespaceData();
	while (position < whitespaceData.size()) {
		char IMP = whitespaceData[position++];
		//スタック操作
		if (IMP == ' ') {
			//スタックに積む
			if (whitespaceData[position] == ' ') {
				position++;
				whitespace[lineNumber].push_back({ 0, AnalysisArg(whitespaceData, position, lineNumber) });
				//複製
			} else if (whitespaceData.substr(position, 2) == "\n ") {
				position += 2;
				whitespace[lineNumber].push_back({ 1, 0 });
				lineNumber++;
				//交換
			} else if (whitespaceData.substr(position, 2) == "\n\t") {
				position += 2;
				whitespace[lineNumber].push_back({ 2, 0 });
				lineNumber++;
				//削除
			} else if (whitespaceData.substr(position, 2) == "\n\n") {
				position += 2;
				whitespace[lineNumber].push_back({ 3, 0 });
				lineNumber += 2;
				//コピー
			} else if (whitespaceData.substr(position, 2) == "\t ") {
				position += 2;
				whitespace[lineNumber].push_back({ 4, AnalysisArg(whitespaceData, position, lineNumber) });
				//ジェノサイド
			} else if (whitespaceData.substr(position, 2) == "\t\n") {
				position += 2;
				whitespace[lineNumber].push_back({ 5, AnalysisArg(whitespaceData, position, lineNumber) });
				lineNumber++;
				//shuffle
			} else if (whitespaceData.substr(position, 3) == "\t\t ") {
				position += 3;
				whitespace[lineNumber].push_back({ 6, 0 });
			}
			//数値演算およびヒープ操作
		} else if (IMP == '\t') {
			IMP = whitespaceData[position++];
			//数値演算
			if (IMP == ' ') {
				//加算
				if (whitespaceData.substr(position, 2) == "  ") {
					position += 2;
					whitespace[lineNumber].push_back({ 7, 0 });
					//減算
				} else if (whitespaceData.substr(position, 2) == " \t") {
					position += 2;
					whitespace[lineNumber].push_back({ 8, 0 });
					//乗算
				} else if (whitespaceData.substr(position, 2) == " \n") {
					position += 2;
					whitespace[lineNumber].push_back({ 9, 0 });
					lineNumber++;
					//除算
				} else if (whitespaceData.substr(position, 2) == "\t ") {
					position += 2;
					whitespace[lineNumber].push_back({ 10, 0 });
					//剰余
				} else if (whitespaceData.substr(position, 2) == "\t\t") {
					position += 2;
					whitespace[lineNumber].push_back({ 11, 0 });
				} else {
					//解析終了
					break;
				}
				//ヒープ操作
			} else if (IMP == '\t') {
				//ヒープ書き込み
				if (whitespaceData[position] == ' ') {
					position++;
					whitespace[lineNumber].push_back({ 12, AnalysisArg(whitespaceData, position, lineNumber) });
				} else if (whitespaceData[position] == '\t') {
					position++;
					whitespace[lineNumber].push_back({ 13, 0 });
				} else {
					//解析終了
					break;
				}
			} else {
				//解析終了
				break;
			}
		} else {
			//解析終了
			break;
		}
	}
	return whitespace;
}