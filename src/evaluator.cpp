#include "evaluator.hpp"
using namespace std;

//TODO:Debug用
std::map<Node, std::string> nodeTypeStr = {
	{Node::Number, "Number"},
	{Node::String, "String"},
	{Node::Array, "Array"},
	{Node::UnaryOperator, "UnaryOperator"},
	{Node::BinaryOperator, "BinaryOperator"},
	{Node::TernaryOperator, "TernaryOperator"},
	{Node::Assignment, "Assignment"},
	{Node::Variable, "Variable"},
	{Node::StaticVarWithAssignment, "StaticVarWithAssignment"},
	{Node::StaticVarWithoutAssignment, "StaticVarWithoutAssignment"},
	{Node::Statement, "Statement"},
	{Node::BlockStatement, "BlockStatement"},
	{Node::Function, "Function"},
	{Node::DefFunction, "DefFunction"},
	{Node::ArgumentNode, "ArgumentNode"},
	{Node::IfStatement, "IfStatement"},
	{Node::ReturnStatement, "ReturnStatement"},
	{Node::WhileStatement, "WhileStatement"},
	{Node::JumpStatement, "JumpStatement"},
};


Evaluator::Evaluator() {
	//数学定数を指定
	math_const["M_PI"] = (long double)3.14159265358979323846;
	math_const["M_E"] = (long double)2.71828182845904523536;
	math_const["M_LOG2E"] = (long double)1.44269504088896340736;
	math_const["M_LOG10E"] = (long double)0.43429448190325182765;
	math_const["M_LN2"] = (long double)0.69314718055994530942;
	math_const["M_LN10"] = (long double)2.30258509299404568402;
	math_const["M_PI_2"] = (long double)1.57079632679489661923;
	math_const["M_PI_4"] = (long double)0.78539816339744830962;
	math_const["M_1_PI"] = (long double)0.31830988618379067154;
	math_const["M_2_PI"] = (long double)0.63661977236758134308;
	math_const["M_2_SQRTPI"] = (long double)1.12837916709551257390;
	math_const["M_SQRT2"] = (long double)1.41421356237309504880;
	math_const["M_SQRT1_2"] = (long double)0.70710678118654752440;
	math_const["M_SQRT3"] = (long double)1.73205080756887729352;
	//キーコードの初期化
	init_keycode();
	//グローバル変数作成
	EnterScope();
	//システム変数の初期化
	start_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	srand((unsigned int)time(nullptr));
	return;
}

Evaluator::~Evaluator() {
	//スコープを抜ける
	ExitScope();
	return;
}

pair<Var, int> Evaluator::evaluate(AST* ast) {
	if (ast == nullptr) return make_pair(Var(), 1);
	bool is_static = false;
	int type;
	switch (ast->GetNodeType()) {
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		EnterScope();
		for (unsigned long long i = 0;AST * stmt = node->ReadStatement(i); i++) {
			pair<Var, int> retval = evaluate(stmt);
			if (retval.second != 1) {
				ExitScope();
				return retval;
			}
		}
		ExitScope();
		return make_pair(Var(), 1);
	}
	case Node::IfStatement: {
		EnterScope();
		pair<Var, int> retval = IfStatement(ast);
		ExitScope();
		return retval;
	}
	case Node::Function: {
		//void関数
		EnterScope();
		VoidFunction(ast);
		ExitScope();
		return make_pair(Var(), 1);
	}
	case Node::DefFunction: {
		return make_pair(Var(), 1);
	}
	case Node::StaticVarWithoutAssignment: {
		ProcessStaticVar(ast);
		return make_pair(Var(), 1);
	}
	case Node::StaticVarWithAssignment:
		type = static_cast<StaticVariableNode*>(ast)->GetType();
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
		is_static = true;
	case Node::Assignment: {
		//変数定義、計算等
		ProcessVariables(ast, is_static, type);
		return make_pair(Var(), 1);
	}
	case Node::BinaryOperator: {
		BinaryOperatorNode* node = static_cast<BinaryOperatorNode*>(ast);
		ProcessBinaryOperator(node->GetLeft(), node->GetRight(), node->GetOperatorType(), node);
		return make_pair(Var(), 1);
	}
	case Node::ReturnStatement: {
		ReturnStatementNode* node = static_cast<ReturnStatementNode*>(ast);
		if (node->GetExpression() == nullptr)
			return make_pair(Var(), 0);
		return make_pair(CalcExpr(node->GetExpression()), 0);
	}
	case Node::WhileStatement: {
		//while文
		return WhileStatement(ast);
	}
	case Node::JumpStatement: {
		JumpStatementNode* node = static_cast<JumpStatementNode*>(ast);
		return make_pair(Var(), node->GetJumpType());
	}
	default:
		throw EvaluatorException("RuntimeError: Invaild Statement.Type:" + nodeTypeStr[ast->GetNodeType()]);
	}
}

void Evaluator::RegisterFunctions(AST* ast) {
	//関数登録
	if (ast == nullptr) return;
	if (ast->GetNodeType() == Node::DefFunction) {
		UserFunctionNode* node = static_cast<UserFunctionNode*>(ast);
		string functionName = node->GetFunctionName();
		if (user_func.count(functionName))
			throw EvaluatorException("Redefinition of function: " + functionName + ".");
		user_func[functionName] = make_pair(node->GetArgument(), node->GetBlockStatement());
		return;
	}
	return;
}


Var Evaluator::CalcExpr(AST* ast) {
	//各種演算
	bool is_static = false;
	int type;
	switch (ast->GetNodeType()) {
	case Node::Number: {
		NumberNode* node = static_cast<NumberNode*>(ast);
		string number = node->GetValue();
		if (number.find('.') != string::npos) {
			//小数点が含まれている場合
			return Var(stold(number));
		} else {
			//整数の場合
			return Var(stoll(number));
		}
	}
	case Node::String: {
		StringNode* node = static_cast<StringNode*>(ast);
		string str = node->GetValue();
		return Var(str);
	}
	case Node::UnaryOperator: {
		UnaryOperatorNode* node = static_cast<UnaryOperatorNode*>(ast);
		if (node->GetOperatorType() == 0) {
			//単項演算子(!)
			return Var(!CalcExpr(node->GetExpression()));
		} else if (node->GetOperatorType() == 1) {
			//単項演算子(-)
			return Var(-CalcExpr(node->GetExpression()));
		}
	}
	case Node::BinaryOperator: {
		BinaryOperatorNode* node = static_cast<BinaryOperatorNode*>(ast);
		return ProcessBinaryOperator(node->GetLeft(), node->GetRight(), node->GetOperatorType(), node);
	}
	case Node::TernaryOperator: {
		//三項演算子
		TernaryOperatorNode* node = static_cast<TernaryOperatorNode*>(ast);
		if (CalcExpr(node->GetCondition()).GetValue<bool>()) {
			return (CalcExpr(node->GetTrueExpr()));
		} else {
			return (CalcExpr(node->GetFalseExpr()));
		}
	}
	case Node::Function: {
		SystemFunctionNode* node = static_cast<SystemFunctionNode*>(ast);
		string functionName = node->GetFunctionName();
		vector<AST*> args = node->GetArgument();
		//各種組み込み関数
		if (args.size() == 0) {
			//引数なし
			if (functionName == "unixtime") {
				return Var((long long)time(nullptr));
			} else if (functionName == "gettime") {
				return Var(GetTime());
			} else if (functionName == "getfps") {
				return Var((long double)GetFPS());
			} else if (functionName == "mouseclick") {
				return Var((long long)SDL_GetMouseState(&mousex, &mousey));
			}
		} else if (args.size() == 1) {
			//引数1つ
			//各種数学関数(引数は1つ)
#define MATH_FUNC(func) if (functionName == #func)\
							return func(CalcExpr(args.at(0)).GetValue<long double>()); 
			MATH_FUNC(abs);
			MATH_FUNC(sqrt);
			MATH_FUNC(sin);
			MATH_FUNC(cos);
			MATH_FUNC(tan);
			MATH_FUNC(asin);
			MATH_FUNC(acos);
			MATH_FUNC(atan);
			MATH_FUNC(exp);
			MATH_FUNC(log);
			MATH_FUNC(log10);
#undef MATH_FUNC
			//その他組み込み関数(!=void,引数は1つ)
			if (functionName == "double") {
				return StaticVar(CalcExpr(args.at(0)).GetValue<long double>());
			} else if (functionName == "int") {
				return StaticVar(CalcExpr(args.at(0)).GetValue<long long>());
			} else if (functionName == "string") {
				return StaticVar(CalcExpr(args.at(0)).GetValue<string>());
			} else if (functionName == "rnd") {
				return Var((long long)(rand() % (CalcExpr(args.at(0)).GetValue<long long>())));
			} else if (functionName == "size") {
				return Var((long long)CalcExpr(args.at(0)).GetPointer()->size());
			} else if (functionName == "strlen") {
				return Var((long long)CalcExpr(args.at(0)).GetValue<string>().size());
			} else if (functionName == "getkey") {
				string key_name = CalcExpr(args.at(0)).GetValue<string>();
				if (keycode.count(key_name)) {
					const bool* state = SDL_GetKeyboardState(nullptr);
					return Var((long long)state[keycode[key_name]]);
				} else {
					throw EvaluatorException("Unknown key name: " + key_name);
				}
			} else if (functionName == "mmseek") {
				return Var((long double)audio.GetMusicPosition(CalcExpr(args.at(0)).GetValue<long long>()));
			} else if (functionName == "mmplaying") {
				return Var(audio.IsMusicPlaying(CalcExpr(args.at(0)).GetValue<long long>()));
			} else if (functionName == "mmvolinfo") {
				return Var((long long)audio.GetVolume(CalcExpr(args.at(0)).GetValue<long long>()));
			} else if (functionName == "mousepos") {
				SDL_GetMouseState(&mousex, &mousey);
				if (CalcExpr(args.at(0)).GetValue<bool>()) {
					return Var((long double)mousex);
				} else {
					return Var((long double)mousey);
				}
			} else if (functionName == "mousegpos") {
				SDL_GetGlobalMouseState(&mousex, &mousey);
				if (CalcExpr(args.at(0)).GetValue<bool>()) {
					return Var((long double)mousex);
				} else {
					return Var((long double)mousey);
				}
			}
		} else if (args.size() == 2) {
			//引数2つ
			//各種数学関数(引数は2つ)
#define MATH_FUNC(func) if (functionName == #func)\
							return func(CalcExpr(args.at(0)).GetValue<long double>(),CalcExpr(args.at(1)).GetValue<long double>());
			MATH_FUNC(pow);
			MATH_FUNC(atan2);
#undef MATH_FUNC
			//その他組み込み関数(!=void,引数は2つ)
			if (functionName == "min") {
				Var left = CalcExpr(args.at(0));
				Var right = CalcExpr(args.at(1));
				return (left < right) ? left : right;
			} else if (functionName == "max") {
				Var left = CalcExpr(args.at(0));
				Var right = CalcExpr(args.at(1));
				return (left > right) ? left : right;
			} else if (functionName == "findstr") {
				size_t pos = CalcExpr(args.at(0)).GetValue<string>().find(CalcExpr(args.at(1)).GetValue<string>());
				return Var((long long)(pos == string::npos ? -1 : pos));
			} else if (functionName == "strmid") {
				return Var(CalcExpr(args.at(0)).GetValue<string>().substr(CalcExpr(args.at(1)).GetValue<long long>()));
			}
		} else if (args.size() == 3) {
			//引数3つ
			if (functionName == "strmid") {
				return Var(CalcExpr(args.at(0)).GetValue<string>().substr(
					CalcExpr(args.at(1)).GetValue<long long>(),
					CalcExpr(args.at(2)).GetValue<long long>()
				));
			} else if (functionName == "limit") {
				//引数1つ目:値,2つ目:最小値,3つ目:最大値
				Var value = CalcExpr(args.at(0));
				Var min_value = CalcExpr(args.at(1));
				Var max_value = CalcExpr(args.at(2));
				if (value < min_value) return min_value;
				if (value > max_value) return max_value;
				return value;
			}
		}
		//ユーザー定義関数
		if (user_func.count(functionName)) return EvaluateFunction(static_cast<UserFunctionNode*>(ast));
		throw EvaluatorException("Unknown function:" + functionName);
	}
	case Node::StaticVarWithAssignment:
		type = static_cast<StaticVariableNode*>(ast)->GetType();
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
		is_static = true;
	case Node::Assignment:return ProcessVariables(ast, is_static, type);
	case Node::StaticVarWithoutAssignment: {
		return ProcessStaticVar(ast);
	}
	case Node::Variable: {
		VariableNode* node = static_cast<VariableNode*>(ast);
		VariableNode* dynamic_casted_node = dynamic_cast<VariableNode*>(ast);
		if (dynamic_casted_node == nullptr)
			throw EvaluatorException("Segmentation fault: Invalid node type.");
		string variableName = node->GetVariableName();
		//各種数学定数
		if (math_const.count(variableName)) {
			return math_const[variableName];
		}
		//システム変数
		if (variableName == "dir_cur") {
			filesystem::path current_path = filesystem::current_path();
			return Var(current_path.string());
		} else if (variableName == "true") {
			return Var(1LL);
		} else if (variableName == "false") {
			return Var(0LL);
		}
		//変数の存在を確認
		if (node->GetArrayIndex().empty()) {
			//通常変数
			for (auto& ref_static : ref_static_var | views::reverse) {
				if (ref_static.count(variableName))
					return *ref_static[variableName];
			}
			for (auto& ref : ref_var | views::reverse) {
				if (ref.count(variableName))
					return *ref[variableName];
			}
			for (auto& scoped_static_var : static_var | views::reverse) {
				if (scoped_static_var.count(variableName)) {
					return scoped_static_var[variableName];
				}
			}
			for (auto& scoped_var : var | views::reverse) {
				if (scoped_var.count(variableName)) {
					return scoped_var[variableName];
				}
			}
			//未定義の場合0で初期化
			return var.back()[variableName] = Var(0LL);
		} else {
			for (auto& array : var | views::reverse) {
				if (array.count(variableName)) {
					//配列変数
					Var* retval = &array[variableName];
					for (AST* index_node : node->GetArrayIndex()) {
						long long index = CalcExpr(index_node).GetValue<long long>();
						//配列のインデックスが範囲外の場合
						if (index < 0 || index >= retval->GetPointer()->size())
							throw RuntimeException("Array index out of range.:" + variableName + "(which is " + to_string(index) + ") >= " + to_string(retval->GetPointer()->size())\
								, node->lineNumber, node->columnNumber);
						retval = &(retval->GetPointer()->at(index));
					}
					return *retval;
				}
			}
			for (auto& array : static_var | views::reverse) {
				if (array.count(variableName)) {
					//静的配列変数
					StaticVar* retval = &array[variableName];
					for (AST* index_node : node->GetArrayIndex()) {
						long long index = CalcExpr(index_node).GetValue<long long>();
						//配列のインデックスが範囲外の場合
						if (index < 0 || index >= retval->GetPointer()->size())
							throw RuntimeException("Array index out of range:" + variableName + "(which is " + to_string(index) + ") >= " + to_string(retval->GetPointer()->size())\
								, node->lineNumber, node->columnNumber);
						retval = &(retval->GetPointer()->at(index));
					}
					return *retval;
				}
			}
			//配列変数が存在しない場合はエラー
			throw RuntimeException("Undefined array: " + variableName + ".", node->lineNumber, node->columnNumber);
		}

	}
	case Node::Array: {
		//配列リテラル
		ArrayNode* node = static_cast<ArrayNode*>(ast);
		vector<Var> array;
		bool first_loop = true;
		int type;
		for (AST* element : node->GetElements()) {
			if (first_loop) {
				array.push_back(CalcExpr(element));
				type = array.back().GetType();
				first_loop = false;
			} else {
				switch (type) {
				case 0: //int
					array.push_back(CalcExpr(element).GetValue<long long>());
					break;
				case 1: //double
					array.push_back(CalcExpr(element).GetValue<long double>());
					break;
				case 2: //string
					array.push_back(CalcExpr(element).GetValue<string>());
					break;
				case 10:
				case 11:
				case 12: //配列
					array.push_back(CalcExpr(element).GetValue<vector<Var>>());
					break;
				default:
					throw RuntimeException("Invalid array element type.\nERROR CODE:" + to_string(type), node->lineNumber, node->columnNumber);
					break;
				}
			}

		}
		node->SetType(10 + type % 10);
		return Var(array);
	}
	case Node::ReturnStatement: {
		throw EvaluatorException("Return statement should not be evaluated.");
	}
	default:throw EvaluatorException("Unknown node type.");
	}
	throw EvaluatorException("Fatal Error:This message should not be displayed.(´・ω・｀)");
}

Var Evaluator::ProcessBinaryOperator(AST* left_node, AST* right_node, string operatorType, BinaryOperatorNode* node) {
	//演算子の処理
	Var Left(CalcExpr(left_node));
	//短絡評価
	if (operatorType == "&&") {
		if (Left.GetValue<bool>() == false) return Left;
	} else if (operatorType == "||") {
		if (Left.GetValue<bool>() == true) return Left;
	}
	//各種演算子の処理
	Var Right(CalcExpr(right_node));
	if (operatorType == "+")return (Left + Right);
	if (operatorType == "*")return (Left * Right);
	if (operatorType == "==") return (Left == Right);
	if (operatorType == "!=") return (Left != Right);
	if (operatorType == "&&")return (Left && Right);
	if (operatorType == "||") return (Left || Right);
	if (operatorType == "-")return (Left - Right);
	if (operatorType == "/") {
		if (Right.IsZero(Left.GetType()))
			throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		return (Left / Right);
	}
	if (operatorType == "<")  return (Left < Right);
	if (operatorType == "<=") return (Left <= Right);
	if (operatorType == ">")  return (Left > Right);
	if (operatorType == ">=") return (Left >= Right);
	if (operatorType == "&")  return (Left & Right);
	if (operatorType == "|")  return (Left | Right);
	if (operatorType == "^")  return (Left ^ Right);
	if (operatorType == "<<") return (Left << Right);
	if (operatorType == ">>") return (Left >> Right);
	if ((operatorType == "+=") || (operatorType == "-=") || (operatorType == "*=") || (operatorType == "/=") || (operatorType == "%=") || (operatorType == "&=") || (operatorType == "|=") || (operatorType == "^=") || (operatorType == "<<=") || (operatorType == ">>=")) {
		//+=, -=, *=, /=, %=, &=, |=, ^= , <<=, >>=
		if ((operatorType == "/=") || (operatorType == "%=")) {
			if (Right.IsZero(Left.GetType()))
				throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		}
		return BinaryAssignmentOperator(left_node, Left, Right, operatorType, node);
	}
	if (operatorType == "%") {
		if (Right.IsZero(Left.GetType()))
			throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		return (Left % Right);
	}
	throw RuntimeException("Unknown Operator.\"" + operatorType + "\"", node->lineNumber, node->columnNumber);
}

//+=, -=, *=, /=, %=, &=, |=, ^=
Var Evaluator::BinaryAssignmentOperator(AST* left_node, Var Left, Var Right, string operatorType, BinaryOperatorNode* node) {
	if (left_node->GetNodeType() != Node::Variable)
		throw RuntimeException("Invalid left operand type.", node->lineNumber, node->columnNumber);
	string variableName = static_cast<VariableNode*>(left_node)->GetVariableName();
	//変数の存在を確認
#define AssignOperator(op) 	{for (auto& ref_static : ref_static_var | views::reverse) {\
		if (ref_static.count(variableName))return *ref_static[variableName] = StaticVar(Left op Right);\
	}\
	for (auto& ref : ref_var | views::reverse) {\
		if (ref.count(variableName))return *ref[variableName] = Var(Left op Right);\
	}\
	for (auto& scoped_static_var : static_var | views::reverse) {\
		if (scoped_static_var.count(variableName))return scoped_static_var[variableName] = StaticVar(Left op Right);\
	}\
	for (auto& scoped_var : var | views::reverse) {\
		if (scoped_var.count(variableName))return scoped_var[variableName] = Var(Left op Right);\
	}\
	return var.back()[variableName] = Var(Left op Right);}

	if (operatorType == "+=") AssignOperator(+);
	if (operatorType == "-=") AssignOperator(-);
	if (operatorType == "*=") AssignOperator(*);
	if (operatorType == "/=") AssignOperator(/ );
	if (operatorType == "%=") AssignOperator(%);
	if (operatorType == "&=") AssignOperator(&);
	if (operatorType == "|=") AssignOperator(| );
	if (operatorType == "^=") AssignOperator(^);
	if (operatorType == "<<=") AssignOperator(<< );
	if (operatorType == ">>=") AssignOperator(>> );
	throw RuntimeException("Unknown Operator.\"" + operatorType + "\"", node->lineNumber, node->columnNumber);
#undef AssignOperator
}

std::pair<Var, int> Evaluator::IfStatement(AST* ast) {
	//if文の処理
	IfStatementNode* node = static_cast<IfStatementNode*>(ast);
	AST* falseExpr = node->GetFalseExpr();
	if (CalcExpr(node->GetCondition()).GetValue<bool>()) {
		return evaluate(node->GetTrueExpr());
	} else if (falseExpr != nullptr) {
		return evaluate(falseExpr);
	}
	return make_pair(Var(), 1);
}

//戻り値のない関数の処理
void Evaluator::VoidFunction(AST* ast) {
	//void関数の処理
	SystemFunctionNode* node = static_cast<SystemFunctionNode*>(ast);
	string functionName = node->GetFunctionName();
	vector<AST*> args = node->GetArgument();
	//組み込み関数
	if (functionName == "console") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		Var result = CalcExpr(args.at(0));
		switch (result.GetType()) {
		case 0: {
			cout << result.GetValue<long long>() << endl;
			break;
		}
		case 1: {
			cout << result.GetValue<long double>() << endl;
			break;
		}
		case 2: {
			cout << result.GetValue<string>() << endl;
			break;
		}
		case 3:throw RuntimeException("Void function should not return value.", node->lineNumber, node->columnNumber);
		default:throw RuntimeException("Unknown argument type.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "randomize") {
		if (args.size() > 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		srand((unsigned int)((args.size() == 0) ? time(NULL) : CalcExpr(args.at(0)).GetValue<long long>()));
		return;
	} else if (functionName == "print") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		//グラフィック関数
		graphic.printText(CalcExpr(args.at(0)).GetValue<string>());
		return;
	} else if (functionName == "pos") {
		if (args.size() != 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.SetPos(CalcExpr(args.at(0)).GetValue<long double>(), CalcExpr(args.at(1)).GetValue<long double>());
		return;
	} else if (functionName == "cls") {
		if (args.size() == 0) {
			graphic.Clear();
		} else if (args.size() == 3) {
			graphic.Clear(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>(), CalcExpr(args.at(2)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "color") {
		if (args.size() == 3) {
			graphic.SetColor(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>());
		} else if (args.size() == 4) {
			graphic.SetColor(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>());
		} else if (args.size() == 5) {
			graphic.SetColors(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>(),
				CalcExpr(args.at(4)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "hsvcolor") {
		if (args.size() == 3) {
			graphic.SetHSVColor(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>());
		} else if (args.size() == 4) {
			graphic.SetHSVColor(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>());
		} else if (args.size() == 5) {
			graphic.SetHSVColors(CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>(),
				CalcExpr(args.at(4)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "gmode") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.SetGmode(CalcExpr(args.at(0)).GetValue<long long>());
		return;
	} else if (functionName == "texture") {
		if (args.size() == 1) {
			graphic.SetTexture(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 3) {
			graphic.SetTexture(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>(), CalcExpr(args.at(2)).GetValue<long long>());
		} else if (args.size() == 5) {
			graphic.SetTexture(
				CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>(),
				CalcExpr(args.at(4)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "dialog") {
		if (args.size() > 3)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		Var message = (args.size() < 1) ? "" : CalcExpr(args.at(0));
		Var title = (args.size() < 2) ? "" : CalcExpr(args.at(1));
		Var type = (args.size() < 3) ? 0LL : CalcExpr(args.at(2));
		graphic.CallDialog(title.GetValue<string>(), message.GetValue<string>(), type.GetValue<long long>());
		return;
	} else if (functionName == "wait") {
		if (args.size() > 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		if (args.size() == 0) {
			if (!graphic.Wait())
				exit(0);
		} else {
			if (!graphic.Wait(CalcExpr(args.at(0)).GetValue<long long>()))
				exit(0);
		}
		return;
	} else if (functionName == "await") {
		if (args.size() > 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		if (args.size() == 0) {
			if (!graphic.AWait())
				exit(0);
		} else {
			if (!graphic.AWait(CalcExpr(args.at(0)).GetValue<long long>()))
				exit(0);
		}
		return;
	} else if (functionName == "end") {
		if (args.size() > 0)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.End();
		exit(0);
	} else if (functionName == "font") {
		if (args.size() > 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		if (args.size() == 0)
			graphic.SetFont();
		else if (args.size() == 1)
			graphic.SetFont(CalcExpr(args.at(0)).GetValue<long long>());
		else if (args.size() == 2)
			graphic.SetFont(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<string>());
		return;
	} else if (functionName == "redraw") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.SetRedraw(CalcExpr(args.at(0)).GetValue<bool>());
		return;
	} else if (functionName == "triangle") {
		if (args.size() != 6)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.DrawTriangle(
			CalcExpr(args.at(0)).GetValue<long double>(),
			CalcExpr(args.at(1)).GetValue<long double>(),
			CalcExpr(args.at(2)).GetValue<long double>(),
			CalcExpr(args.at(3)).GetValue<long double>(),
			CalcExpr(args.at(4)).GetValue<long double>(),
			CalcExpr(args.at(5)).GetValue<long double>()
		);
		return;
	} else if (functionName == "quad") {
		if (args.size() != 8)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.DrawRectangle(
			CalcExpr(args.at(0)).GetValue<long double>(),
			CalcExpr(args.at(1)).GetValue<long double>(),
			CalcExpr(args.at(2)).GetValue<long double>(),
			CalcExpr(args.at(3)).GetValue<long double>(),
			CalcExpr(args.at(4)).GetValue<long double>(),
			CalcExpr(args.at(5)).GetValue<long double>(),
			CalcExpr(args.at(6)).GetValue<long double>(),
			CalcExpr(args.at(7)).GetValue<long double>()
		);
		return;
	} else if (functionName == "rect") {
		if (args.size() != 4)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		long long x = CalcExpr(args.at(0)).GetValue<long double>();
		long long y = CalcExpr(args.at(1)).GetValue<long double>();
		long long width = CalcExpr(args.at(2)).GetValue<long double>();
		long long height = CalcExpr(args.at(3)).GetValue<long double>();
		graphic.DrawRectangle(
			x, y,
			x + width, y,
			x + width, y + height,
			x, y + height
		);
		return;
	} else if (functionName == "roundrect") {
		if (args.size() != 5)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.DrawRoundRect(
			CalcExpr(args.at(0)).GetValue<long double>(),
			CalcExpr(args.at(1)).GetValue<long double>(),
			CalcExpr(args.at(2)).GetValue<long double>(),
			CalcExpr(args.at(3)).GetValue<long double>(),
			CalcExpr(args.at(4)).GetValue<long double>()
		);
		return;
	} else if (functionName == "line") {
		if (args.size() == 2) {
			graphic.DrawLine(
				CalcExpr(args.at(0)).GetValue<long double>(),
				CalcExpr(args.at(1)).GetValue<long double>()
			);
		} else if (args.size() == 4) {
			graphic.DrawLine(
				CalcExpr(args.at(0)).GetValue<long double>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>(),
				CalcExpr(args.at(3)).GetValue<long double>()
			);
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "ellipse") {
		if (args.size() == 4) {
			graphic.DrawEllipse(
				CalcExpr(args.at(0)).GetValue<long double>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>(),
				CalcExpr(args.at(3)).GetValue<long double>()
			);
		} else if (args.size() == 5) {
			graphic.DrawEllipse(
				CalcExpr(args.at(0)).GetValue<long double>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>(),
				CalcExpr(args.at(3)).GetValue<long double>(),
				CalcExpr(args.at(4)).GetValue<long double>()
			);
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "circle") {
		if (args.size() != 3)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		long double radius = CalcExpr(args.at(2)).GetValue<long double>() * 2;
		graphic.DrawEllipse(
			CalcExpr(args.at(0)).GetValue<long double>(),
			CalcExpr(args.at(1)).GetValue<long double>(),
			radius, radius
		);
		return;
	} else if (functionName == "LoadImage") {
		if (args.size() == 2) {
			//画像を読み込む
			graphic.Load_Image(
				CalcExpr(args.at(0)).GetValue<string>(),
				CalcExpr(args.at(1)).GetValue<long long>()
			);
		} else if (args.size() == 3) {
			graphic.Load_Image(
				CalcExpr(args.at(0)).GetValue<string>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>()
			);
		} else if (args.size() == 4) {
			graphic.Load_Image(
				CalcExpr(args.at(0)).GetValue<string>(),
				CalcExpr(args.at(1)).GetValue<long long>(),
				CalcExpr(args.at(2)).GetValue<long long>(),
				CalcExpr(args.at(3)).GetValue<long long>()
			);
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "DrawImage") {
		if (args.size() == 1) {
			graphic.DrawImage(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 2) {
			long double scaled_size = CalcExpr(args.at(1)).GetValue<long double>();
			graphic.DrawImage(
				CalcExpr(args.at(0)).GetValue<long long>(),
				scaled_size, scaled_size
			);
		} else if (args.size() == 3) {
			graphic.DrawImage(
				CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>()
			);
		} else if (args.size() == 4) {
			graphic.DrawImage(
				CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>(),
				CalcExpr(args.at(3)).GetValue<long double>()
			);
		} else if (args.size() == 8) {
			graphic.DrawImage(
				CalcExpr(args.at(0)).GetValue<long long>(),
				CalcExpr(args.at(1)).GetValue<long double>(),
				CalcExpr(args.at(2)).GetValue<long double>(),
				CalcExpr(args.at(3)).GetValue<long double>(),
				CalcExpr(args.at(4)).GetValue<long double>(),
				CalcExpr(args.at(5)).GetValue<long double>(),
				CalcExpr(args.at(6)).GetValue<long double>(),
				CalcExpr(args.at(7)).GetValue<long double>()
			);
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "ResizeScreen") {
		if (args.size() != 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.ResizeWindow(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>());
		return;
	} else if (functionName == "title") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.SetWindowTitle(CalcExpr(args.at(0)).GetValue<string>());
		return;
	} else if (functionName == "mmload") {
		if (args.size() != 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.LoadMusic(CalcExpr(args.at(0)).GetValue<string>(), CalcExpr(args.at(1)).GetValue<long long>());
		return;
	} else if (functionName == "dmmload") {
		if (args.size() != 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.LoadSound(CalcExpr(args.at(0)).GetValue<string>(), CalcExpr(args.at(1)).GetValue<long long>());
		return;
	} else if (functionName == "mmplay") {
		if (args.size() == 1) {
			audio.PlayMusic(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 2) {
			audio.PlayMusic(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "dmmplay") {
		if (args.size() == 1) {
			audio.PlaySound(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 2) {
			audio.PlaySound(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "mmstop") {
		if (args.size() != 0)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.StopMusic();
		return;
	} else if (functionName == "mmpause") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.PauseMusic(CalcExpr(args.at(0)).GetValue<long long>());
		return;
	} else if (functionName == "mmresume") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.ResumeMusic(CalcExpr(args.at(0)).GetValue<long long>());
		return;
	} else if (functionName == "mmvol") {
		if (args.size() != 2)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		audio.SetVolume(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>());
		return;
	} else if (functionName == "screen") {
		if (args.size() == 1) {
			graphic.CreateScreen(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 2) {
			graphic.CreateScreen(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<string>());
		} else if (args.size() == 3) {
			graphic.CreateScreen(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<string>(), CalcExpr(args.at(2)).GetValue<long long>());
		} else if (args.size() == 4) {
			graphic.CreateScreen(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<string>(), CalcExpr(args.at(2)).GetValue<long long>(), CalcExpr(args.at(3)).GetValue<long long>());
		} else if (args.size() == 5) {
			graphic.CreateScreen(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<string>(), CalcExpr(args.at(2)).GetValue<long long>(), CalcExpr(args.at(3)).GetValue<long long>(), CalcExpr(args.at(4)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "gsel") {
		if (args.size() == 1) {
			graphic.MakeCurrentWindow(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (args.size() == 2) {
			graphic.MakeCurrentWindow(CalcExpr(args.at(0)).GetValue<long long>(), CalcExpr(args.at(1)).GetValue<long long>());
		} else {
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		}
		return;
	} else if (functionName == "ShowScreen") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.HideWindow(CalcExpr(args.at(0)).GetValue<long long>());
		return;
	} else if (functionName == "HideScreen") {
		if (args.size() != 1)
			throw RuntimeException("Invalid argument size.", node->lineNumber, node->columnNumber);
		graphic.ShowWindow(CalcExpr(args.at(0)).GetValue<long long>());
		return;
	}
	if (user_func.count(functionName)) {
		//関数の中身を実行
		EvaluateFunction(static_cast<UserFunctionNode*>(ast));
		return;
	} else {
		throw EvaluatorException("Unknown function:" + functionName);
	}
	return;
}

Var Evaluator::EvaluateFunction(UserFunctionNode* node) {
	string functionName = node->GetFunctionName();
	vector<AST*> args = node->GetArgument();
	if (user_func[functionName].first.size() < args.size())
		throw RuntimeException("Argument size is too large.", node->lineNumber, node->columnNumber);
	EnterScope();
	for (int i = 0; i < user_func[functionName].first.size(); i++) {
		AST* arg = (i >= args.size()) ? nullptr : args.at(i);
		ArgumentNode* arg_def = static_cast<ArgumentNode*>(user_func[functionName].first.at(i));
		//引数の変数名を取得
		string variableName = static_cast<VariableNode*>(arg_def->GetVariable())->GetVariableName();
		if (arg != nullptr) {
			//引数の変数を定義
			switch (arg_def->GetType()) {
			case 0:
				static_var.back()[variableName] = StaticVar(CalcExpr(arg).GetValue<long long>());
				break;
			case 1:
				static_var.back()[variableName] = StaticVar(CalcExpr(arg).GetValue<long double>());
				break;
			case 2:
				static_var.back()[variableName] = StaticVar(CalcExpr(arg).GetValue<string>());
				break;
			case 3: {
				//参照渡し
				if (arg->GetNodeType() != Node::Variable)
					throw RuntimeException("Invalid argument type.Expected variable.", node->lineNumber, node->columnNumber);
				bool break_flag = false;
				string rootVariableName = static_cast<VariableNode*>(arg)->GetVariableName();
				for (auto& scoped_static_var : static_var | views::reverse) {
					if (scoped_static_var.count(rootVariableName)) {
						ref_static_var.back()[variableName] = &scoped_static_var[rootVariableName];
						break_flag = true;
						break;
					}
				}
				for (auto& scoped_var : var | views::reverse) {
					if (scoped_var.count(rootVariableName)) {
						ref_var.back()[variableName] = &scoped_var[rootVariableName];
						break_flag = true;
						break;
					}
				}
				if (!break_flag)
					throw RuntimeException("Variable \"" + rootVariableName + "\" is not defined.", node->lineNumber, node->columnNumber);
				break;
			}
			default:
				throw EvaluatorException("Unknown type:" + to_string(arg_def->GetType()));
			}
		} else {
			//引数を省略した場合
			if (!arg_def->IsAssigned())
				throw RuntimeException("Argument is not assigned at " + functionName, node->lineNumber, node->columnNumber);
			AST* default_value = arg_def->GetDefaultValue();
			switch (arg_def->GetType()) {
			case 0:
				static_var.back()[variableName] = StaticVar(CalcExpr(default_value).GetValue<long long>());
				break;
			case 1:
				static_var.back()[variableName] = StaticVar(CalcExpr(default_value).GetValue<long double>());
				break;
			case 2:
				static_var.back()[variableName] = StaticVar(CalcExpr(default_value).GetValue<string>());
				break;
			default:
				throw RuntimeException("Invalid argument type.", node->lineNumber, node->columnNumber);
			}
		}
	}
	//関数の中身を実行
	Var retval = ProcessFunction(user_func[functionName].second).first;
	ExitScope();
	return retval;
}

Var Evaluator::ProcessVariables(AST* ast, bool is_static, int& type) {
	AssignmentNode* node = static_cast<AssignmentNode*>(ast);
	string variableName = node->GetVariableName();
	AST* expression = node->GetExpression();
	VariableNode* variable = static_cast<VariableNode*>(node->GetVariable());
	//静的変数の場合、同一スコープ内での再定義は不可
	if (is_static) {
		if (ref_static_var.back().count(variableName))
			throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		if (ref_var.back().count(variableName))
			throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		if (static_var.back().count(variableName))
			throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		if (var.back().count(variableName))
			throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
	} else {
		if (variable->GetArrayIndex().empty()) {
			//変数の存在を確認
			for (auto& ref_static : ref_static_var | views::reverse) {
				if (ref_static.count(variableName))
					return *ref_static[variableName] = CalcExpr(expression);
			}
			for (auto& ref : ref_var | views::reverse) {
				if (ref.count(variableName))
					return *ref[variableName] = CalcExpr(expression);
			}
			for (auto& scoped_static_var : static_var | views::reverse) {
				if (scoped_static_var.count(variableName))
					return scoped_static_var[variableName] = CalcExpr(expression);
			}
			for (auto& scoped_var : var | views::reverse) {
				if (scoped_var.count(variableName))
					return scoped_var[variableName] = CalcExpr(expression);
			}
		} else {
			//配列変数の存在を確認
			for (auto& array : var | views::reverse) {
				if (array.count(variableName)) {
					//配列変数
					Var* retval = &array[variableName];
					for (AST* index_node : variable->GetArrayIndex()) {
						long long index = CalcExpr(index_node).GetValue<long long>();
						//配列のインデックスが範囲外の場合
						if (index < 0)
							throw RuntimeException("Array index out of range:" + variableName + "(which is " + to_string(index) + ") < 0 "\
								, node->lineNumber, node->columnNumber);
						if (index >= retval->GetPointer()->size())
							retval->EditValue<vector<Var>>().resize(index + 1);
						retval = &(retval->EditValue<vector<Var>>().at(index));
					}
					return *retval = CalcExpr(expression);
				}
			}
			for (auto& array : static_var | views::reverse) {
				if (array.count(variableName)) {
					//静的配列変数
					StaticVar* retval = &array[variableName];
					for (AST* index_node : variable->GetArrayIndex()) {
						long long index = CalcExpr(index_node).GetValue<long long>();
						//配列のインデックスが範囲外の場合
						if (index < 0)
							throw RuntimeException("Array index out of range:" + variableName + "(which is " + to_string(index) + ") < 0 "\
								, node->lineNumber, node->columnNumber);
						if (index >= retval->GetPointer()->size())
							retval->ResizeArray(index + 1);
						retval = &(retval->EditValue().at(index));
					}
					*retval = CalcExpr(expression);
					array[variableName].update_array();
					return *retval;
				}
			}
		}
	}
	//変数の定義！！
	if (is_static) {
		StaticVar retval = CalcExpr(expression);
		if (retval.GetType() >= 10)type += 10;
		retval.SetType(type);
		return static_var.back()[variableName] = retval;
	} else {
		return var.back()[variableName] = CalcExpr(expression);
	}
}

Var Evaluator::ProcessStaticVar(AST* ast) {
	//静的変数の処理
	StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
	string variableName = node->GetVariableName();
	//変数の存在を確認
	if (ref_static_var.back().count(variableName))
		throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
	if (ref_var.back().count(variableName))
		throw RuntimeException("Redefinition of variable: " + variableName + ".", node->lineNumber, node->columnNumber);
	if (static_var.back().count(variableName))
		throw RuntimeException("Redefinition of static variable: " + variableName + ".", node->lineNumber, node->columnNumber);
	if (var.back().count(variableName))
		throw RuntimeException("Redefinition of variable: " + variableName + ".", node->lineNumber, node->columnNumber);
	//変数の定義
	switch (node->GetType())
	{
	case 0:return ((static_var.back()[variableName] = StaticVar(0LL)));
	case 1:return ((static_var.back()[variableName] = StaticVar(0.0L)));
	case 2:return ((static_var.back()[variableName] = StaticVar(string())));
	case 3:throw RuntimeException("Void function should not return value.", node->lineNumber, node->columnNumber);
	case 10:
	case 11:
	case 12:
	{
		Var init_value;
		if (node->GetType() == 10) {
			init_value = Var(0LL);
		} else if (node->GetType() == 11) {
			init_value = Var(0.0L);
		} else if (node->GetType() == 12) {
			init_value = Var(string());
		}

		//配列の初期化
		Var init_array(init_value);
		for (AST* index_node : node->GetArrayIndex() | views::reverse) {
			long long index = CalcExpr(index_node).GetValue<long long>();
			if (index < 0)
				throw RuntimeException("Array index cannot be negative.", node->lineNumber, node->columnNumber);
			init_array = vector<Var>(index, init_array);
		}
		static_var.back()[variableName] = move(init_array);
		return static_var.back()[variableName];
	}
	default:throw RuntimeException("Unknown variable type.", node->lineNumber, node->columnNumber);
	}
}

inline long long Evaluator::GetTime() {
	//現在時刻を取得
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_time;
}

pair<Var, int> Evaluator::ProcessFunction(AST* ast) {
	//関数の中身(BlockStatement)を実行
	switch (ast->GetNodeType()) {
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		pair<Var, int> result;
		EnterScope();
		for (unsigned long long i = 0;((node->ReadStatement(i) != nullptr) && (result = ProcessFunction(node->ReadStatement(i))).second);i++);
		ExitScope();
		return result;
	}
	case Node::ReturnStatement: {
		ReturnStatementNode* node = static_cast<ReturnStatementNode*>(ast);
		if (node->GetExpression() == nullptr)
			return make_pair(Var(), 0);
		return make_pair(CalcExpr(node->GetExpression()), 0);
	}
	default:return evaluate(ast);
	}
}

pair<Var, int> Evaluator::WhileStatement(AST* ast) {
	//while文の処理
	WhileStatementNode* node = static_cast<WhileStatementNode*>(ast);
	pair<Var, int> retval;
	EnterScope();
	if (node->GetMode()) {
		static_var.back()["cnt"] = StaticVar(0LL);
		long long count = CalcExpr(node->GetCondition()).GetValue<long long>();
		for (long long i = 0; i < count; i++) {
			retval = evaluate(node->GetStatements());
			if (retval.second == 0) {
				ExitScope();
				return retval;
			} else if (retval.second == 2) {
				break;
			}
			static_var.back()["cnt"].EditValue<long long>()++;
		}
	} else {
		while (CalcExpr(node->GetCondition()).GetValue<bool>()) {
			retval = evaluate(node->GetStatements());
			if (retval.second == 0) {
				ExitScope();
				return retval;
			} else if (retval.second == 2) {
				break;
			}
		}
	}
	ExitScope();
	return make_pair(Var(), 1);
}

long double Evaluator::GetFPS() {
	//FPSを取得
	long long current_time = GetTime();
	if (current_time - last_fps_time >= 1000) {
		fps = static_cast<long double>(frame_count) / ((current_time - last_fps_time) / 1000.0);
		fps = (fps > 1000000.0) ? 60.0 : fps; // FPSが異常に大きい場合は60FPSに制限
		frame_count = 0;
		last_fps_time = current_time;
	}
	frame_count++;
	return fps;
}

void Evaluator::init_keycode() {
	// 数字
	for (int i = 0; i < 9; i++)
		keycode[to_string(i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i);
	// アルファベット
	for (int i = 0; i < 26; i++) {
		char c = 'A' + i;
		keycode[string(1, c)] = static_cast<SDL_Scancode>(SDL_SCANCODE_A + i);
		keycode[string(1, tolower(c))] = static_cast<SDL_Scancode>(SDL_SCANCODE_A + i);
	}
	// 特殊キー
	keycode["ESCAPE"] = SDL_SCANCODE_ESCAPE;
	keycode["SPACE"] = SDL_SCANCODE_SPACE;
	keycode["RETURN"] = SDL_SCANCODE_RETURN;
	keycode["BACKSPACE"] = SDL_SCANCODE_BACKSPACE;
	keycode["ESC"] = SDL_SCANCODE_ESCAPE;
	keycode["TAB"] = SDL_SCANCODE_TAB;
	keycode["UP"] = SDL_SCANCODE_UP;
	keycode["DOWN"] = SDL_SCANCODE_DOWN;
	keycode["LEFT"] = SDL_SCANCODE_LEFT;
	keycode["RIGHT"] = SDL_SCANCODE_RIGHT;
	keycode["PAGEUP"] = SDL_SCANCODE_PAGEUP;
	keycode["PAGEDOWN"] = SDL_SCANCODE_PAGEDOWN;
	keycode["HOME"] = SDL_SCANCODE_HOME;
	keycode["END"] = SDL_SCANCODE_END;
	keycode["INSERT"] = SDL_SCANCODE_INSERT;
	keycode["DELETE"] = SDL_SCANCODE_DELETE;
	// ファンクションキー
	for (int i = 1; i <= 12; i++)
		keycode["F" + to_string(i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_F1 + (i - 1));
	// その他のキー
	keycode["CAPSLOCK"] = SDL_SCANCODE_CAPSLOCK;
	keycode["NUMLOCK"] = SDL_SCANCODE_NUMLOCKCLEAR;
	keycode["SCROLLLOCK"] = SDL_SCANCODE_SCROLLLOCK;
	keycode["PRINTSCREEN"] = SDL_SCANCODE_PRINTSCREEN;
	keycode["PAUSE"] = SDL_SCANCODE_PAUSE;
	keycode["LSHIFT"] = SDL_SCANCODE_LSHIFT;
	keycode["RSHIFT"] = SDL_SCANCODE_RSHIFT;
	keycode["LCTRL"] = SDL_SCANCODE_LCTRL;
	keycode["RCTRL"] = SDL_SCANCODE_RCTRL;
	keycode["LALT"] = SDL_SCANCODE_LALT;
	keycode["RALT"] = SDL_SCANCODE_RALT;
	keycode["LMETA"] = SDL_SCANCODE_LGUI; // Windowsキー
	keycode["RMETA"] = SDL_SCANCODE_RGUI; // Windowsキー
	keycode["MENU"] = SDL_SCANCODE_APPLICATION; // アプリケーションキー
}