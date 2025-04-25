#include "evaluator.hpp"
#include <iostream>
using namespace std;

int main(){
	try{
		Lexer lexer("test/if_test.txt");
		Parser parser(lexer);
		Evaluator evaluator;
		while (AST *ast = parser.ParseStatement(lexer.ExtractNextToken())) {
			//ASTを表示する
			//  parser.show(ast);
			//  printf("\n");
			evaluator.evaluate(ast);
		}
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
	}catch (const LexerException& e) {
		cerr << "Lexer error: " << e.what() << e.where() << endl;
	}catch (const ParserException& e) {
		cerr << "Parser error: " << e.what() << e.where() << endl;
	}catch (const EvaluatorException& e) {
		cerr <<  e.what() << endl;
	}catch (const std::exception& e) {
		cerr << "Error: " << e.what() << endl;
	} catch (...) {
		cerr << "Unknown error occurred." << endl;
	}
}

