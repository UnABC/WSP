#include "evaluator.hpp"
#include "Var.hpp"
using namespace std;

//TODO:Debug用
std::map<Node, std::string> nodeTypeStr = {
	{Node::Number, "Number"},
	{Node::String, "String"},
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
	{Node::ReturnStatement, "ReturnStatement"}
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
	//グローバル変数作成
	EnterScope();
	return;
}

Evaluator::~Evaluator() {
	//スコープを抜ける
	ExitScope();
	return;
}

pair<Var, bool> Evaluator::evaluate(AST* ast) {
	//ASTを表示する
	if (ast == nullptr) return make_pair(Var(), true);
	bool is_static = false;
	int type;
	switch (ast->GetNodeType()) {
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		EnterScope();
		for (unsigned long long i = 0;AST * stmt = node->ReadStatement(i); i++) {
			pair<Var, bool> retval = evaluate(stmt);
			if (!retval.second) {
				ExitScope();
				return retval;
			}
		}
		ExitScope();
		return make_pair(Var(), true);
	}
	case Node::IfStatement: {
		EnterScope();
		pair<Var, bool> retval = IfStatement(ast);
		ExitScope();
		return retval;
	}
	case Node::Function: {
		//void関数
		EnterScope();
		VoidFunction(ast);
		ExitScope();
		return make_pair(Var(), true);
	}
	case Node::DefFunction: {
		return make_pair(Var(), true);
	}
	case Node::StaticVarWithoutAssignment: {
		ProcessStaticVar(ast);
		return make_pair(Var(), true);
	}
	case Node::StaticVarWithAssignment:
		type = static_cast<StaticVariableNode*>(ast)->GetType();
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
		is_static = true;
	case Node::Assignment: {
		//変数定義、計算等
		ProcessVariables(ast, is_static, type);
		return make_pair(Var(), true);
	}
	case Node::ReturnStatement: {
		ReturnStatementNode* node = static_cast<ReturnStatementNode*>(ast);
		if (node->GetExpression() == nullptr)
			return make_pair(Var(), false);
		return make_pair(CalcExpr(node->GetExpression()), false);
	}
	default:
		throw EvaluatorException("RuntimeError: Invaild Statement." + nodeTypeStr[ast->GetNodeType()]);
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
		string operatorType = node->GetOperatorType();
		AST* left_node = node->GetLeft();
		AST* right_node = node->GetRight();
		return ProcessBinaryOperator(left_node, right_node, operatorType, node);
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
		//各種数学関数(引数は2つ)
#define MATH_FUNC(func) if (functionName == #func)\
							return func(CalcExpr(args.at(0)).GetValue<long double>(),CalcExpr(args.at(1)).GetValue<long double>());
		MATH_FUNC(pow);
		MATH_FUNC(atan2);
#undef MATH_FUNC
		//その他組み込み関数(!=void)
		if (functionName == "double") {
			return StaticVar(CalcExpr(args.at(0)).GetValue<long double>());
		} else if (functionName == "int") {
			return StaticVar(CalcExpr(args.at(0)).GetValue<long long>());
		} else if (functionName == "string") {
			return StaticVar(CalcExpr(args.at(0)).GetValue<string>());
		}
		//ユーザー定義関数
		if (user_func.count(functionName)) {
			if (user_func[functionName].first.size() < args.size())
				throw RuntimeException("Argument size is too large.", node->lineNumber, node->columnNumber);
			EnterScope();
			for (int i = 0; i < node->GetArgumentSize(); i++) {
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
					case 3:
						//参照渡し
						if (arg->GetNodeType() != Node::Variable)
							throw RuntimeException("Invalid argument type.Expected variable.", node->lineNumber, node->columnNumber);
						switch (arg->GetType())
						{
						case 0:
							var.back()[variableName] = CalcExpr(arg).GetValue<long long>();
							break;
						case 1:
							var.back()[variableName] = CalcExpr(arg).GetValue<long double>();
							break;
						case 2:
							var.back()[variableName] = CalcExpr(arg).GetValue<string>();
							break;
						default:
							throw RuntimeException("Invalid argument type.Expected variable.", node->lineNumber, node->columnNumber);
						}
						break;
					default:
						throw EvaluatorException("Unknown type:" + to_string(arg_def->GetType()));
					}
				} else {
					//引数を省略した場合
					if (!arg_def->IsAssigned())
						throw RuntimeException("Argument is not assigned.", node->lineNumber, node->columnNumber);
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
		throw EvaluatorException("Unknown function:" + functionName);
	}
	case Node::StaticVarWithAssignment:
		type = static_cast<StaticVariableNode*>(ast)->GetType();
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
		is_static = true;
	case Node::Assignment: {
		AssignmentNode* node = static_cast<AssignmentNode*>(ast);
		string variableName = node->GetVariableName();
		AST* expression = node->GetExpression();
		//変数の存在を確認
		for (auto& scoped_static_var : static_var | views::reverse) {
			if (scoped_static_var.count(variableName))
				return scoped_static_var[variableName] = CalcExpr(expression);
		}
		for (auto& scoped_var : var | views::reverse) {
			if (scoped_var.count(variableName))
				return scoped_var[variableName] = CalcExpr(expression);
		}
		//変数の定義！！
		if (is_static) {
			StaticVar retval = CalcExpr(expression);
			retval.SetType(type);
			return static_var.back()[variableName] = retval;
		} else {
			return var.back()[variableName] = CalcExpr(expression);
		}
	}
	case Node::StaticVarWithoutAssignment: {
		//静的変数の処理
		StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
		string variableName = node->GetVariableName();
		for (auto& scoped_static_var : static_var | views::reverse) {
			if (scoped_static_var.count(variableName))
				throw RuntimeException("Redefinition of variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		}
		for (auto& scoped_var : var | views::reverse) {
			if (scoped_var.count(variableName))
				throw RuntimeException("Redefinition of variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		}
		switch (node->GetType())
		{
		case 0:return ((static_var.back()[variableName] = StaticVar((long long)0)));
		case 1:return ((static_var.back()[variableName] = StaticVar((long double)0.0)));
		case 2:return ((static_var.back()[variableName] = StaticVar(string())));
		case 3:throw RuntimeException("Void function should not return value.", node->lineNumber, node->columnNumber);
		default:throw RuntimeException("Unknown variable type.", node->lineNumber, node->columnNumber);
		}
		throw EvaluatorException("Unknown type");
	}
	case Node::Variable: {
		VariableNode* node = static_cast<VariableNode*>(ast);
		VariableNode* dynamic_casted_node = dynamic_cast<VariableNode*>(ast);
		if (dynamic_casted_node == nullptr) {
			throw EvaluatorException("Segmentation fault: Invalid node type.");
		}
		string variableName = node->GetVariableName();
		//各種数学定数
		if (math_const.count(variableName)) {
			return math_const[variableName];
		}
		//変数の存在を確認
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
		throw RuntimeException("Undefined variable: " + variableName + ".", node->lineNumber, node->columnNumber);
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
		if (Right.GetValue<long double>() == 0.0)
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
	if (operatorType == "%") {
		if (Right.GetValue<long double>() == 0.0)
			throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		return (Left % Right);
	}
	throw RuntimeException("Unknown Operator.\"" + operatorType + "\"", node->lineNumber, node->columnNumber);
}

std::pair<Var, bool> Evaluator::IfStatement(AST* ast) {
	//if文の処理
	IfStatementNode* node = static_cast<IfStatementNode*>(ast);
	AST* falseExpr = node->GetFalseExpr();
	if (CalcExpr(node->GetCondition()).GetValue<bool>()) {
		return evaluate(node->GetTrueExpr());
	} else if (falseExpr != nullptr) {
		return evaluate(falseExpr);
	}
	return make_pair(Var(), true);
}

//戻り値のない関数の処理
void Evaluator::VoidFunction(AST* ast) {
	//void関数の処理
	SystemFunctionNode* node = static_cast<SystemFunctionNode*>(ast);
	string functionName = node->GetFunctionName();
	vector<AST*> args = node->GetArgument();
	//組み込み関数
	if (functionName == "print") {
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
	}
	//TODO:ユーザー定義関数
	if (user_func.count(functionName)) {

	}
	return;
}

void Evaluator::ProcessVariables(AST* ast, bool is_static, int type) {
	AssignmentNode* node = static_cast<AssignmentNode*>(ast);
	string variableName = node->GetVariableName();
	AST* expression = node->GetExpression();
	//変数の存在を確認
	for (auto& scoped_static_var : static_var) {
		if (scoped_static_var.count(variableName)) {
			scoped_static_var[variableName] = CalcExpr(expression);
			return;
		}
	}
	for (auto& scoped_var : var | views::reverse) {
		if (scoped_var.count(variableName)) {
			scoped_var[variableName] = CalcExpr(expression);
			return;
		}
	}
	//変数の定義
	if (is_static) {
		static_var.back()[variableName] = StaticVar(CalcExpr(expression));
		static_var.back()[variableName].SetType(type);
	} else {
		var.back()[variableName] = CalcExpr(expression);
	}
	return;
}

void Evaluator::ProcessStaticVar(AST* ast) {
	//静的変数の処理
	StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
	string variableName = node->GetVariableName();
	//変数の存在を確認
	for (auto& scoped_static_var : static_var) {
		if (scoped_static_var.count(variableName))
			throw EvaluatorException("Redefinition of static variable: " + variableName + ".");
	}
	for (auto& scoped_var : var | views::reverse) {
		if (scoped_var.count(variableName))
			throw EvaluatorException("Redefinition of static variable: " + variableName + ".");
	}
	//変数の定義
	switch (node->GetType())
	{
	case 0:static_var.back()[variableName] = StaticVar((long long)0); break;
	case 1:static_var.back()[variableName] = StaticVar((long double)0.0); break;
	case 2:static_var.back()[variableName] = StaticVar(string()); break;
	case 3:throw RuntimeException("Void function should not return value.", node->lineNumber, node->columnNumber);
	default:throw RuntimeException("Unknown function variable type.", node->lineNumber, node->columnNumber);
	}
}

pair<Var, bool> Evaluator::ProcessFunction(AST* ast) {
	//関数の中身(BlockStatement)を実行
	switch (ast->GetNodeType()) {
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		pair<Var, bool> result;
		EnterScope();
		for (unsigned long long i = 0;(result = ProcessFunction(node->ReadStatement(i))).second;i++);
		ExitScope();
		return result;
	}
	case Node::ReturnStatement: {
		ReturnStatementNode* node = static_cast<ReturnStatementNode*>(ast);
		if (node->GetExpression() == nullptr)
			return make_pair(Var(), false);
		return make_pair(CalcExpr(node->GetExpression()), false);
	}
	default:return evaluate(ast);
	}
}