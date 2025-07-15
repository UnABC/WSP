# pragma GCC target("avx2")
# pragma GCC optimize("O3")
# pragma GCC optimize("unroll-loops")

#include "evaluator.hpp"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
	try {
		string file_name = "test/3D.wsp";
		if (argc == 2)file_name = argv[1];
		Lexer lexer(file_name);
		//Lexer実行
		Parser parser(lexer);
		Evaluator evaluator;
		vector<AST*> statements;
		//Parser実行
		while (AST* ast = parser.ParseStatement(lexer.ExtractNextToken())) {
			//parser.show(ast);
			//printf("\n");
			//evaluator.evaluate(ast);
			statements.push_back(ast); //ASTを保存
			evaluator.RegisterFunctions(ast); //関数を登録
		}
		//Evaluator実行
		for (AST* ast : statements)
			evaluator.evaluate(ast); //評価実行

		evaluator.Stop(); //ウィンドウは表示したまま
		//以下Lexer testの残骸//////////////////////////////////////////////////////////////////
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
		cerr << "Lexer error: " << e.what() << "\n" << e.where() << endl;
	}
	catch (const ParserException& e) {
		cerr << "Parser error: " << e.what() << "\n" << e.where() << endl;
	}
	catch (const EvaluatorException& e) {
		cerr << "Evaluator error:" << e.what() << endl;
	}
	catch (const RuntimeException& e) {
		cerr << "Runtime error: " << e.what() << "\n" << e.where() << endl;
	}
	catch (const WindowException& e) {
		cerr << "Graphic error: " << e.what() << endl;
	}
	catch (const FontException& e) {
		cerr << "Font error: " << e.what() << endl;
	}
	catch (const ShaderException& e) {
		cerr << "Graphic error: " << e.what() << endl;
	}
	catch (const ShapeException& e) {
		cerr << "Shape error: " << e.what() << endl;
	}
	catch (const ImageException& e) {
		cerr << "Image error: " << e.what() << endl;
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

