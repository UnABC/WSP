#include "evaluator.hpp"
#include "OperationType.hpp"
using namespace std;

Evaluator::Evaluator() {
	//数学定数を指定
	math_const["M_PI"] = 3.14159265358979323846;
	math_const["M_E"] = 2.71828182845904523536;
	math_const["M_LOG2E"] = 1.44269504088896340736;
	math_const["M_LOG10E"] = 0.43429448190325182765;
	math_const["M_LN2"] = 0.69314718055994530942;
	math_const["M_LN10"] = 2.30258509299404568402;
	math_const["M_PI_2"] = 1.57079632679489661923;
	math_const["M_PI_4"] = 0.78539816339744830962;
	math_const["M_1_PI"] = 0.31830988618379067154;
	math_const["M_2_PI"] = 0.63661977236758134308;
	math_const["M_2_SQRTPI"] = 1.12837916709551257390;
	math_const["M_SQRT2"] = 1.41421356237309504880;
	math_const["M_SQRT1_2"] = 0.70710678118654752440;
	math_const["M_SQRT3"] = 1.73205080756887729352;
	return;
}

void Evaluator::evaluate(AST* ast) {
	//ASTを表示する
	if (ast == nullptr) return;
	switch (ast->GetNodeType()) {
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		while (AST* stmt = node->ReadStatement())evaluate(stmt);
		return;
	}
	case Node::IfStatement: {
		IfStatement(ast);
		return;
	}
	case Node::Function: {
		//void関数
		VoidFunction(ast);
		return;
	}
	case Node::StaticVarWithoutAssignment: {
		ProcessStaticVar(ast);
		return;
	}
	case Node::StaticVarWithAssignment:
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
	case Node::Assignment: {
		//変数定義、計算等
		ProcessVariables(ast);
		return;
	}
	default:
		throw EvaluatorException("RuntimeError: Invaild Statement.");
	}
}


template<typename T>
T Evaluator::CalcExpr(AST* ast) {
	//各種演算
	switch (ast->GetNodeType()) {
	case Node::Number:
	case Node::String: {
		NumberNode* node = static_cast<NumberNode*>(ast);
		string number = node->GetValue();
		if constexpr (is_same_v<T, string>) {
			return number;
		} else {
			if (number.find('.') != string::npos) {
				return my_stold(number);
			} else {
				return my_stoll(number);
			}
		}
	}
	case Node::UnaryOperator: {
		UnaryOperatorNode* node = static_cast<UnaryOperatorNode*>(ast);
		switch (node->GetType())
		{
		case 0: {
			OperationType value(CalcExpr<long long>(node->GetExpression()));
			return ReturnProperType<T>((!value).GetValue<long long>());
		}
		case 1: {
			OperationType value(CalcExpr<long double>(node->GetExpression()));
			return ReturnProperType<T>((!value).GetValue<long double>());
		}
		case 2: {
			OperationType value(CalcExpr<string>(node->GetExpression()));
			return ReturnProperType<T>((!value).GetValue<string>());
		}
		default:
			break;
		}
		throw EvaluatorException("Unknown type:" + to_string(node->GetType()));
	}
	case Node::BinaryOperator: {
		BinaryOperatorNode* node = static_cast<BinaryOperatorNode*>(ast);
		string operatorType = node->GetOperatorType();
		AST* left_node = node->GetLeft();
		AST* right_node = node->GetRight();
		switch (left_node->GetType())
		{
		case 0: {
			return ReturnProperType<T>(ProcessBinaryOperator<long long>(left_node, right_node, operatorType, node));
		}
		case 1: {
			return ReturnProperType<T>(ProcessBinaryOperator<long double>(left_node, right_node, operatorType, node));
		}
		case 2: {
			return ReturnProperType<T>(ProcessBinaryOperator<string>(left_node, right_node, operatorType, node));
		}
		default:
			break;
		}
		throw EvaluatorException("Unknown type:" + to_string(left_node->GetType()));
	}
	case Node::TernaryOperator: {
		//三項演算子
		TernaryOperatorNode* node = static_cast<TernaryOperatorNode*>(ast);
		switch (node->GetType())
		{
		case 0:
			if (CalcExpr<bool>(node->GetCondition())) {
				return ReturnProperType<T>(CalcExpr<long long>(node->GetTrueExpr()));
			} else {
				return ReturnProperType<T>(CalcExpr<long long>(node->GetFalseExpr()));
			}
		case 1:
			if (CalcExpr<bool>(node->GetCondition())) {
				return ReturnProperType<T>(CalcExpr<long double>(node->GetTrueExpr()));
			} else {
				return ReturnProperType<T>(CalcExpr<long double>(node->GetFalseExpr()));
			}
		case 2:
			if (CalcExpr<bool>(node->GetCondition())) {
				return ReturnProperType<T>(CalcExpr<string>(node->GetTrueExpr()));
			} else {
				return ReturnProperType<T>(CalcExpr<string>(node->GetFalseExpr()));
			}
		default:
			throw EvaluatorException("Unknown type");
		}
		throw EvaluatorException("Unknown type");
	}
	case Node::Function: {
		FunctionNode* node = static_cast<FunctionNode*>(ast);
		string functionName = node->GetFunctionName();
		vector<AST*> args = node->GetArgument();
		if constexpr (!is_same_v<T, string>) {
			//各種数学関数(引数は1つ)
#define MATH_FUNC(func) if (functionName == #func){\
				switch (args.at(0)->GetType()){\
					case 0:return func(CalcExpr<long long>(args.at(0)));\
					case 1:return func(CalcExpr<long double>(args.at(0)));\
					default:throw RuntimeException("Invaild argumant.",node->lineNumber,node->columnNumber);\
				}} 
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
#define MATH_FUNC(func) if (functionName == #func){\
				if (args.size() < 2)throw EvaluatorException("Invalid argument size.");\
				switch (args.at(0)->GetType()){\
					case 0:return func(CalcExpr<long long>(args.at(0)),CalcExpr<long long>(args.at(1)));\
					case 1:return func(CalcExpr<long double>(args.at(0)),CalcExpr<long double>(args.at(1)));\
					default:throw RuntimeException("Invaild argumant.",node->lineNumber,node->columnNumber);\
				}} 
			MATH_FUNC(pow);
			MATH_FUNC(atan2);
#undef MATH_FUNC
		}
		//その他組み込み関数(!=void)
		if (functionName == "double") {
			return ReturnProperType<T>(CalcExpr<long double>(args.at(0)));
		} else if (functionName == "int") {
			return ReturnProperType<T>(CalcExpr<long long>(args.at(0)));
		} else if (functionName == "string") {
			return ReturnProperType<T>(CalcExpr<string>(args.at(0)));
		}
		throw EvaluatorException("Unknown function:" + functionName);
	}
	case Node::StaticVarWithAssignment:
		ast = static_cast<StaticVariableNode*>(ast)->GetAssignment();
	case Node::Assignment: {
		AssignmentNode* node = static_cast<AssignmentNode*>(ast);
		string variableName = node->GetVariableName();
		AST* expression = node->GetExpression();
		//変数の存在を確認
		if (varll.count(variableName)) {
			return ReturnProperType<T>(varll[variableName] = CalcExpr<long long>(expression));
		} else if (varld.count(variableName)) {
			return ReturnProperType<T>(varld[variableName] = CalcExpr<long double>(expression));
		} else if (vars.count(variableName)) {
			return ReturnProperType<T>(vars[variableName] = CalcExpr<string>(expression));
		}
		switch (node->GetType())
		{
		case 0:return ReturnProperType<T>(varll[variableName] = CalcExpr<long long>(expression));
		case 1:return ReturnProperType<T>(varld[variableName] = CalcExpr<long double>(expression));
		case 2:return ReturnProperType<T>(vars[variableName] = CalcExpr<string>(expression));
		case 3:throw  EvaluatorException("Void function should not return value.");
		default:throw EvaluatorException("Unknown function variable type.");
		}
		throw EvaluatorException("Unknown type");
	}
	case Node::StaticVarWithoutAssignment: {
		//静的変数の処理
		StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
		string variableName = node->GetVariableName();
		if (varll.count(variableName)) {
			return ReturnProperType<T>(varll[variableName]);
		} else if (varld.count(variableName)) {
			return ReturnProperType<T>(varld[variableName]);
		} else if (vars.count(variableName)) {
			return ReturnProperType<T>(vars[variableName]);
		}
		switch (node->GetType())
		{
		case 0:return ReturnProperType<T>(varll[variableName] = 0);
		case 1:return ReturnProperType<T>(varld[variableName] = 0.0);
		case 2:return ReturnProperType<T>(vars[variableName] = "");
		case 3:throw EvaluatorException("Void function should not return value.");
		default:throw EvaluatorException("Unknown function variable type.");
		}
		throw EvaluatorException("Unknown type");
	}
	case Node::Variable: {
		VariableNode* node = static_cast<VariableNode*>(ast);
		string variableName = node->GetVariableName();
		//各種数学定数
		if (math_const.count(variableName)) {
			if constexpr (is_same_v<T, string>) {
				return to_string(math_const[variableName]);
			} else {
				return math_const[variableName];
			}
		}
		//変数の存在を確認
		if (varll.count(variableName)) {
			return ReturnProperType<T>(varll[variableName]);
		} else if (varld.count(variableName)) {
			return ReturnProperType<T>(varld[variableName]);
		} else if (vars.count(variableName)) {
			return ReturnProperType<T>(vars[variableName]);
		} else {
			throw RuntimeException("Undefined variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		}
	}
	default: {
		throw EvaluatorException("Unknown node type.");
	}
	}
	throw EvaluatorException("Fatal Error:This message should not be displayed.(´・ω・｀)");
}

template<typename T>
T Evaluator::ProcessBinaryOperator(AST* left_node, AST* right_node, string operatorType, BinaryOperatorNode* node) {
	//演算子の処理
	OperationType Left(CalcExpr<T>(left_node));
	//短絡評価
	if (operatorType == "&&") {
		if (Left.GetValue<bool>() == false) return Left.GetValue<T>();
	} else if (operatorType == "||") {
		if (Left.GetValue<bool>() == true) return Left.GetValue<T>();
	}
	//各種演算子の処理
	OperationType Right(CalcExpr<T>(right_node));
	if (operatorType == "+")return (Left + Right).GetValue<T>();
	if (operatorType == "*")return (Left * Right).GetValue<T>();
	if (operatorType == "==") return (Left == Right).GetValue<T>();
	if (operatorType == "!=") return (Left != Right).GetValue<T>();
	if (operatorType == "&&")return (Left && Right).GetValue<T>();
	if (operatorType == "||") return (Left || Right).GetValue<T>();
	if (operatorType == "-")return (Left - Right).GetValue<T>();
	if (operatorType == "/") {
		if (Right.GetValue<long double>() == 0.0)
			throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		return (Left / Right).GetValue<T>();
	}
	if (operatorType == "<")  return (Left < Right).GetValue<T>();
	if (operatorType == "<=") return (Left <= Right).GetValue<T>();
	if (operatorType == ">")  return (Left > Right).GetValue<T>();
	if (operatorType == ">=") return (Left >= Right).GetValue<T>();
	if (operatorType == "&")  return (Left & Right).GetValue<T>();
	if (operatorType == "|")  return (Left | Right).GetValue<T>();
	if (operatorType == "^")  return (Left ^ Right).GetValue<T>();
	if (operatorType == "<<") return (Left << Right).GetValue<T>();
	if (operatorType == ">>") return (Left >> Right).GetValue<T>();
	if (operatorType == "%") {
		if (Right.GetValue<long double>() == 0.0)
			throw RuntimeException("Division by zero.", node->lineNumber, node->columnNumber);
		return (Left % Right).GetValue<T>();
	}
	throw RuntimeException("Unknown Operator.\"" + operatorType + "\"", node->lineNumber, node->columnNumber);
}

void Evaluator::IfStatement(AST* ast) {
	//if文の処理
	IfStatementNode* node = static_cast<IfStatementNode*>(ast);
	AST* falseExpr = node->GetFalseExpr();
	if (CalcExpr<long double>(node->GetCondition())) {
		evaluate(node->GetTrueExpr());
	} else if (falseExpr != nullptr) {
		evaluate(falseExpr);
	}
	return;
}

//戻り値のない関数の処理
void Evaluator::VoidFunction(AST* ast) {
	//void関数の処理
	FunctionNode* node = static_cast<FunctionNode*>(ast);
	string functionName = node->GetFunctionName();
	vector<AST*> args = node->GetArgument();
	//組み込み関数
	if (functionName == "print") {
		switch (args.at(0)->GetType()) {
		case 0: {
			cout << CalcExpr<long long>(args.at(0)) << endl;
			break;
		}
		case 1: {
			cout << CalcExpr<long double>(args.at(0)) << endl;
			break;
		}
		case 2: {
			cout << CalcExpr<string>(args.at(0)) << endl;
			break;
		}
		case 3:throw EvaluatorException("Void function should not return value.");
		default:throw EvaluatorException("Unknown function argument type.");
		}
		return;
	}
	//TODO:ユーザ定義関数
	return;
}

void Evaluator::ProcessVariables(AST* ast) {
	AssignmentNode* node = static_cast<AssignmentNode*>(ast);
	string variableName = node->GetVariableName();
	AST* expression = node->GetExpression();
	//変数の存在を確認
	if (varll.count(variableName)) {
		varll[variableName] = CalcExpr<long long>(expression);
		return;
	} else if (varld.count(variableName)) {
		varld[variableName] = CalcExpr<long double>(expression);
		return;
	} else if (vars.count(variableName)) {
		vars[variableName] = CalcExpr<string>(expression);
		return;
	}
	//変数の定義
	if (node->GetType() == 0) {
		varll[variableName] = CalcExpr<long long>(expression);
		return;
	} else if (node->GetType() == 1) {
		varld[variableName] = CalcExpr<long double>(expression);
		return;
	} else if (node->GetType() == 2) {
		vars[variableName] = CalcExpr<string>(expression);
		return;
	} else {
		throw EvaluatorException("Unknown variable type.");
	}
	return;
}

void Evaluator::ProcessStaticVar(AST* ast) {
	//静的変数の処理
	StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
	string variableName = node->GetVariableName();
	//変数の存在を確認
	if (varll.count(variableName) || varld.count(variableName) || vars.count(variableName))
		throw EvaluatorException("Static variable \"" + variableName + "\" already exists.");
	//変数の定義
	if (node->GetType() == 0) {
		varll[variableName] = 0;
		return;
	} else if (node->GetType() == 1) {
		varld[variableName] = 0.0;
		return;
	} else if (node->GetType() == 2) {
		vars[variableName] = "";
		return;
	}
	throw EvaluatorException("Unknown variable type." + to_string(node->GetType()));
}

template<typename T, typename S>
T Evaluator::ReturnProperType(S value) {
	//適切な型を返す
	if constexpr (is_same_v<T, long double>) {
		if constexpr (is_same_v<S, string>) {
			return my_stold(value);
		} else {
			return (long double)value;
		}
	} else if constexpr (is_same_v<T, long long>) {
		if constexpr (is_same_v<S, string>) {
			return my_stoll(value);
		} else {
			return (long long)value;
		}
	} else if constexpr (is_same_v<T, string>) {
		if constexpr (is_same_v<S, string>) {
			return value;
		} else if constexpr (is_same_v<S, bool>) {
			return value ? "1" : "";
		} else {
			return to_string(value);
		}
	} else if constexpr (is_same_v<T, bool>) {
		if constexpr (is_same_v<S, string>) {
			return (value == "") ? false : true;
		} else {
			return (bool)value;
		}
	}
}

long long Evaluator::my_stoll(const string str) const {
	try {
		return stoll(str);
	}
	catch (...) {
		return 0; // 変換できない場合は0を返す
	}
}

long double Evaluator::my_stold(const string str) const {
	try {
		return stold(str);
	}
	catch (...) {
		return 0.0; // 変換できない場合は0.0を返す
	}
}
