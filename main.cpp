#include "evaluator.hpp"
#include "CheckType.hpp"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
	try {
		Lexer lexer("test/type.txt");
		Parser parser(lexer);
		Evaluator evaluator;
		//vector<AST*> statements;
		VerifyType checkType;
		while (AST* ast = parser.ParseStatement(lexer.ExtractNextToken())) {
			//parser.show(ast);
			//printf("\n");
			checkType.CheckType(ast); //型チェック
			evaluator.evaluate(ast);
			//statements.push_back(ast); //ASTを保存
		}
		// for (AST *ast : statements) {
		// 	//ASTを表示する
		// 	//  parser.show(ast);
		// 	//  printf("\n");

		// }
		// parser.show(ast);
		// printf("\n");
		// TokenPtr token;
		// printf("[\n");
		// while (true) {
		// 	token = lexer.ExtractNextToken();
		// 	if (token->type == TokenType::EndOfLine) continue; //行末はスキップ
		// 	cout << "\t(\"" << token->value << "\", " << lexer.TokenTypeToString(token->type) << ")," << endl;
		// 	if (token->type == TokenType::EndOfFile) break; //EOFなら終了
		// }
		// printf("]\n");
	}
	catch (const LexerException& e) {
		cerr << "Lexer error: " << e.what() << e.where() << endl;
	}
	catch (const ParserException& e) {
		cerr << "Parser error: " << e.what() << e.where() << endl;
	}
	catch (const EvaluatorException& e) {
		cerr << "Evaluator error:" << e.what() << endl;
	}
	catch (const CheckTypeException& e) {
		cerr << "CheckType error: " << e.what() << e.where() << endl;
	}
	catch (const RuntimeException& e) {
		cerr << "Runtime error: " << e.what() << e.where() << endl;
	}
	catch (const std::logic_error& e) {
		cerr << "Logic error: " << e.what() << endl;
	}
	catch (const std::exception& e) {
		cerr << "Error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown error occurred." << endl;
	}
}

