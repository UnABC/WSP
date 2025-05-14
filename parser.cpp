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
		{"<<",true},{">>",true},{"&",true},
		{"^",true},{"|",true}
	};
	while ((currentToken->type == TokenType::Operator) && ((bool)operators.count(currentToken->value))) {
		string operatorType = currentToken->value;
		currentToken = lexer.ExtractNextToken(); //演算子をスキップ
		AST* right = ExprAdd();
		left = new BinaryOperatorNode(operatorType, left, right, currentToken->lineNumber, currentToken->columnNumber); //二項演算子ノードを作成
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
	while ((currentToken->type == TokenType::Operator) && (currentToken->value == "*" || currentToken->value == "/")) {
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
		} else if ((nextToken->type == TokenType::Operator) && (nextToken->value == "=")) {
			//代入演算子の処理
			return ExprAssignment(currentToken);
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
		} else {
			AST* ast = new VariableNode(currentToken->value, currentToken->lineNumber, currentToken->columnNumber); //変数ノードを作成
			currentToken = lexer.ExtractNextToken(); //トークンを進める
			return ast;
		}
	} else if (currentToken->type == TokenType::LParentheses) {
		currentToken = lexer.ExtractNextToken(); //左括弧をスキップ
		AST* left = ExprTernary();
		if (currentToken->type != TokenType::RParentheses) 
			throw ParserException("Not found right parenthesis.", currentToken->lineNumber, currentToken->columnNumber);
		currentToken = lexer.ExtractNextToken(); //右括弧をスキップ
		return left;
	}
	throw ParserException("Invalid token.\"" + currentToken->value + "\"", currentToken->lineNumber, currentToken->columnNumber);
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

AST* Parser::ExprAssignment(TokenPtr token) {
	//代入演算子を解析する
	string variableName = token->value;
	currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	currentToken = lexer.ExtractNextToken(); //演算子をスキップ
	AST* right = ExprTernary(); //式を解析する
	AST* variable = new VariableNode(variableName, currentToken->lineNumber, currentToken->columnNumber); //変数ノードを作成
	AST* ast = new AssignmentNode(variableName, variable, right, currentToken->lineNumber, currentToken->columnNumber); //代入ノードを作成
	return ast;
}

AST* Parser::ExprFunction(TokenPtr token) {
	//関数を解析する
	string functionName = token->value;
	currentToken = lexer.ExtractNextToken(); //関数名をスキップ
	vector<AST*> args;
	do {
		currentToken = lexer.ExtractNextToken(); //(or,をスキップ
		if (currentToken->type == TokenType::RParentheses) break; //引数がない場合は終了
		args.push_back(ExprTernary()); //引数を解析する
	} while (currentToken->type == TokenType::Symbol && currentToken->value == ",");
	currentToken = lexer.ExtractNextToken(); //)をスキップ
	AST* ast = new SystemFunctionNode(functionName, args, currentToken->lineNumber, currentToken->columnNumber); //関数ノードを作成
	return ast;
}

AST* Parser::Statement(TokenPtr token) {
	//文を解析する
	if (currentToken->type == TokenType::Identifier) {
		//TODO:文の処理

		//システムの予約語を処理する
		//If文
		if (currentToken->value == "if") {
			currentToken = lexer.ExtractNextToken(); //ifをスキップ
			AST* condition = ExprTernary(); //条件式を解析する
			AST* trueExpr = nullptr;
			//真の場合の文を解析する
			trueExpr = (currentToken->type == TokenType::LBrace) ? BlockStatement(currentToken) : Statement(currentToken);
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
			AST* expression = (currentToken->type == TokenType::EndOfLine) ? nullptr : ExprTernary(); //戻り値を解析する
			if ((currentToken->type != TokenType::EndOfLine) && (currentToken->type != TokenType::EndOfFile) && (currentToken->type != TokenType::RBrace))
				throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected EOL.", currentToken->lineNumber, currentToken->columnNumber);
			return new ReturnStatementNode(expression, currentToken->lineNumber, currentToken->columnNumber); //return文ノードを作成
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
	if ((type != 3) && (nextToken->type == TokenType::Operator) && (nextToken->value == "=")) {
		//静的変数の初期化
		AST* assignment = ExprAssignment(currentToken); //型の変数を宣言&代入
		return new StaticVariableNode(assignment, type, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
	} else if (nextToken->type == TokenType::LParentheses) {
		//関数の初期化
		string functionName = currentToken->value; //関数名を取得
		currentToken = lexer.ExtractNextToken(); //関数名をスキップ
		currentToken = lexer.ExtractNextToken(); //(をスキップ
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
		AST* ast = new UserFunctionNode(functionName, args, block, type, currentToken->lineNumber, currentToken->columnNumber); //関数ノードを作成
		return ast;
	}
	if (type == 3)
		throw ParserException("Invalid token.\"" + currentToken->value + "\"\nExpected function declaration.", currentToken->lineNumber, currentToken->columnNumber);
	//静的変数(初期値なし)
	string variableName = currentToken->value; //変数名を取得
	AST* ast = new StaticVarNodeWithoutAssignment(variableName, type, currentToken->lineNumber, currentToken->columnNumber); //静的変数ノードを作成
	currentToken = lexer.ExtractNextToken(); //識別子をスキップ
	return ast;
}

AST* Parser::Argument(int type) {
	currentToken = lexer.ExtractNextToken(); //型をスキップ
	TokenPtr nextToken = lexer.PeekTokens(0); //次のトークンを取得
	string variableName = currentToken->value; //変数名を取得
	AST* ast;
	bool isReference = (type == 3);
	if (type == 3) {
		//参照渡し
		ast = new VariableNode(variableName, currentToken->lineNumber, currentToken->columnNumber); //変数ノードを作成
		ast->SetType(0); //型をintで仮設定
	} else {
		//値渡し
		ast = new StaticVarNodeWithoutAssignment(variableName, type, currentToken->lineNumber, currentToken->columnNumber);
	}
	currentToken = lexer.ExtractNextToken(); //変数名をスキップ
	if ((nextToken->type == TokenType::Operator) && (nextToken->value == "=")) {
		//デフォルト引数有
		currentToken = lexer.ExtractNextToken(); //=をスキップ
		AST* assignment = ExprTernary(); //式を解析する
		return new ArgumentNode(ast, type, isReference, assignment, currentToken->lineNumber, currentToken->columnNumber); //引数ノードを作成
	} else {
		//デフォルト引数無
		return new ArgumentNode(ast, type, isReference, nullptr, currentToken->lineNumber, currentToken->columnNumber); //引数ノードを作成
	}
}

void Parser::show(AST* ast) {
	//ASTを表示する
	if (ast == nullptr) return;
	switch (ast->GetNodeType()) {
	case Node::Number: {
		NumberNode* node = static_cast<NumberNode*>(ast);
		cout << node->GetValue() << " ";
		break;
	}
	case Node::String: {
		StringNode* node = static_cast<StringNode*>(ast);
		cout << node->GetValue();
		break;
	}
	case Node::BinaryOperator: {
		BinaryOperatorNode* node = static_cast<BinaryOperatorNode*>(ast);
		cout << "( " << node->GetOperatorType();
		show(node->GetLeft());
		show(node->GetRight());
		printf(" )");
		break;
	}
	case Node::Function: {
		SystemFunctionNode* node = static_cast<SystemFunctionNode*>(ast);
		cout << node->GetFunctionName() << "(";
		for (int i = 0; i < node->GetArgumentSize(); i++) {
			if (i != 0) cout << ", ";
			show(node->GetArgument().at(i));
		}
		cout << ")";
		break;
	}
	case Node::Assignment: {
		AssignmentNode* node = static_cast<AssignmentNode*>(ast);
		cout << "{" << node->GetVariableName() << " = ";
		show(node->GetExpression());
		cout << " }";
		break;
	}
	case Node::StaticVarWithAssignment: {
		StaticVariableNode* node = static_cast<StaticVariableNode*>(ast);
		cout << "{" << node->GetAssignment() << " }";
		break;
	}
	case Node::StaticVarWithoutAssignment: {
		StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
		cout << "`" << node->GetVariableName() << " `";
		break;
	}
	case Node::Variable: {
		VariableNode* node = static_cast<VariableNode*>(ast);
		cout << "`" << node->GetVariableName() << "` ";
		break;
	}
	case Node::IfStatement: {
		IfStatementNode* node = static_cast<IfStatementNode*>(ast);
		cout << "if ( ";
		show(node->GetCondition());
		cout << " ) { ";
		show(node->GetTrueExpr());
		cout << " }";
		if (node->GetFalseExpr() != nullptr) {
			cout << " else { ";
			show(node->GetFalseExpr());
			cout << " }";
		}
		break;
	}
	case Node::Statement: {
		StatementNode* node = static_cast<StatementNode*>(ast);
		show(node->GetExpression());
		break;
	}
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		cout << "{ ";
		for (unsigned long long i = 0;AST* statement = node->ReadStatement(i);i++) {
			show(statement);
		}
		cout << " }";
		break;
	}
	default: {
		cout << "Unknown node type." << endl;
		break;
	}
	}
}


