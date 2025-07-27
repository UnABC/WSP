#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "parser.hpp"
#include "Var.hpp"
#include "graphic.hpp"
#include "Audio.hpp"
#include <SDL3/SDL_oldnames.h>
#include <fstream>
#include <filesystem>

class Evaluator {
private:
	TokenPtr currentToken;	//現在のトークン
	//各種変数格納map
	std::vector<std::map<std::string, Var>> var;
	std::vector<std::map<std::string, StaticVar>> static_var;
	std::vector<std::map<std::string, Var*>> ref_var;
	std::vector<std::map<std::string, StaticVar*>> ref_static_var;
	//システム変数
	long long start_time;	//プログラム開始時間
	long long last_fps_time = 0;	//前回の時間
	long long frame_count;
	long double fps = 60.0;	//フレームレート
	float mousex, mousey;	//マウス座標
	bool whitespace_mode = false;	//ホワイトスペースモード
	std::map<unsigned long long, std::vector<std::pair<int, int>>> whitespace;	//ホワイトスペースのデータ
	std::vector<int> whitespace_stack;	//ホワイトスペースのスタック
	unsigned long long last_ws_lineNumber = 0;
	//ユーザー定義関数
	std::map<std::string, std::pair<std::vector<AST*>, AST*>> user_func;
	//システム関数
	std::unordered_map<std::string, std::function<void(std::vector<AST*> args, unsigned long long lineNumber, unsigned long long columnNumber)>> system_functions;
	//数学定数
	std::map<std::string, Var> math_const;
	//キーコード
	std::map<std::string, SDL_Scancode> keycode;
	//各種計算等
	Var CalcExpr(AST* ast);
	Var ProcessBinaryOperator(AST* left_node, AST* right, std::string operatorType, BinaryOperatorNode* node);
	Var EvaluateFunction(UserFunctionNode* node);
	std::pair<Var, int> IfStatement(AST* node);
	std::pair<Var, int> WhileStatement(AST* ast);
	void VoidFunction(AST* ast);
	Var ProcessVariables(AST* ast, bool is_static, int& type);
	Var ProcessStaticVar(AST* ast);
	Var AssignVariable(Var expression, VariableNode* variable, bool is_static, int& type, unsigned long long lineNumber, unsigned long long columnNumber);
	std::pair<Var, int> ProcessFunction(AST* ast);
	inline long long GetTime();
	long double GetFPS();
	void EvaluateWhitespace(unsigned long long lineNumber);
	void AutoEvalWS(unsigned long long now_lineNumber);
	//Graphic関数
	Graphic graphic;
	Audio audio;
	//スコープ変数の管理
	inline void EnterScope() { var.emplace_back();static_var.emplace_back();ref_var.emplace_back();ref_static_var.emplace_back(); };
	inline void ExitScope() { var.pop_back();static_var.pop_back();ref_var.pop_back();ref_static_var.pop_back(); };

	void init_keycode();
	void init_system_functions();
public:
	Evaluator();
	~Evaluator();
	std::pair<Var, int> evaluate(AST* ast);
	void RegisterFunctions(AST* ast);
	void RegisterWhitespace(std::map<unsigned long long, std::vector<std::pair<int, int>>> whitespaceData);
	void Stop() { graphic.Stop(); };
};
#endif // EVALUATOR_HPP

