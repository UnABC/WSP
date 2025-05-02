#include "evaluator.hpp"
using namespace std;

void Evaluator::evaluate(AST *ast){
	//ASTを表示する
	if (ast == nullptr) return;
	if (ast->GetNodeType() == Node::BlockStatement) {
		BlockStatementNode *node = static_cast<BlockStatementNode*>(ast);
		while (AST *stmt = node->ReadStatement())evaluate(stmt);
		return;
	}
	if (ast->GetNodeType() == Node::IfStatement) {
		IfStatement(ast);
		return;
	}
	//void関数
	if (ast->GetNodeType() == Node::Function){
		VoidFunction(ast);
		return;
	}
	//変数定義、計算等
	if (ast->GetNodeType() == Node::Assignment){
		ProcessVariables(ast);
		return;
	}
	//式の処理
	//cout << "Result: " << CalcExpr<long double>(ast) << endl;
	throw EvaluatorException("RuntimeError: Invaild Statement.");
}


//Tip:string型は別枠で処理する
template<typename T>
T Evaluator::CalcExpr(AST *ast){
	//各種演算
	switch (ast->GetNodeType()) {
		case Node::Number: {
			NumberNode *node = static_cast<NumberNode*>(ast);
			string number = node->GetValue();
			if (number.find('.') != string::npos){
				return stold(number);
			}else{
				return stoll(number);
			}
		}
		case Node::BinaryOperator: {
			BinaryOperatorNode *node = static_cast<BinaryOperatorNode*>(ast);
			string operatorType = node->GetOperatorType();
			AST *left_node = node->GetLeft();
			switch (left_node->GetType())
			{
			case 0:{
				return ProcessBinaryOperator<long long>(CalcExpr<long long>(left_node), CalcExpr<long long>(node->GetRight()), operatorType, node);
			}
			case 1:{
				return ProcessBinaryOperator<long double>(CalcExpr<long double>(left_node), CalcExpr<long double>(node->GetRight()), operatorType, node);
			}
			case 2:{
				//TODO:文字列演算子の処理
			}
			default:
				break;
			}
			throw EvaluatorException("Unknown type:"+to_string(left_node->GetType()));
		}
		//三項演算子
		case Node::TernaryOperator: {
			TernaryOperatorNode *node = static_cast<TernaryOperatorNode*>(ast);
			switch (node->GetType())
			{
			case 0:
				if (CalcExpr<bool>(node->GetCondition())){
					return CalcExpr<long long>(node->GetTrueExpr());
				}else{
					return CalcExpr<long long>(node->GetFalseExpr());
				}
			case 1:
				if (CalcExpr<bool>(node->GetCondition())){
					return CalcExpr<long double>(node->GetTrueExpr());
				}else{
					return CalcExpr<long double>(node->GetFalseExpr());
				}
			case 2:
				//TODO:文字列演算子の処理
			default:
				break;
			}
			throw EvaluatorException("Unknown type");
		}
		case Node::Function: {
			FunctionNode *node = static_cast<FunctionNode*>(ast);
			string functionName = node->GetFunctionName();
			vector<AST*> args = node->GetArgument();

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

			//その他組み込み関数(!=void)
			if (functionName == "double") {
				return CalcExpr<long double>(args.at(0));
			}else if (functionName == "int") {
				return CalcExpr<long long>(args.at(0));
			}
			throw EvaluatorException("Unknown function:"+functionName);
		}
		case Node::Assignment: {
			AssignmentNode *node = static_cast<AssignmentNode*>(ast);
			string variableName = node->GetVariableName();
			AST *expression = node->GetExpression();
			//変数の存在を確認
			if (varll.count(variableName)){
				return varll[variableName] = CalcExpr<long long>(expression);
			}else if (varld.count(variableName)){
				return varld[variableName] = CalcExpr<long double>(expression);
			}else if (vars.count(variableName)){
				//TODO:文字列リテラルの処理
			}
			switch (node->GetType())
			{
				case 0:return varll[variableName] = CalcExpr<long long>(expression);
				case 1:return varld[variableName] = CalcExpr<long double>(expression);
				case 2: //TODO:stringの処理
				case 3:throw EvaluatorException("Void function should not return value.");
				default:throw EvaluatorException("Unknown function variable type.");
			}
			throw EvaluatorException("Unknown type");
		}
		case Node::Variable: {
			VariableNode *node = static_cast<VariableNode*>(ast);
			string variableName = node->GetVariableName();
			//各種数学定数
			if (variableName == "M_PI")       return 3.14159265358979323846;
			if (variableName == "M_E")        return 2.71828182845904523536;
			if (variableName == "M_LOG2E")    return 1.44269504088896340736;
			if (variableName == "M_LOG10E")   return 0.43429448190325182765;
			if (variableName == "M_LN2")      return 0.69314718055994530942;
			if (variableName == "M_LN10")     return 2.30258509299404568402;
			if (variableName == "M_PI_2")     return 1.57079632679489661923;
			if (variableName == "M_PI_4")     return 0.78539816339744830962;
			if (variableName == "M_1_PI")     return 0.31830988618379067154;
			if (variableName == "M_2_PI")     return 0.63661977236758134308;
			if (variableName == "M_2_SQRTPI") return 1.12837916709551257388;
			if (variableName == "M_SQRT2")    return 1.41421356237309504880;
			if (variableName == "M_SQRT1_2")  return 0.70710678118654752440;
			if (variableName == "M_SQRT3")    return 1.73205080756887729352;
			//変数の存在を確認
			if (varll.count(variableName)){
				return varll[variableName];
			}else if (varld.count(variableName)){
				return varld[variableName];
			}else if (vars.count(variableName)){
				//TODO:文字列リテラルの処理
			}else{
				throw RuntimeException("Undefined variable: " + variableName + ".",node->lineNumber,node->columnNumber);
			}
		}
		default:{
			throw EvaluatorException("Unknown node type.");
		}
	}
	throw EvaluatorException("Fatal Error:This message should not be displayed.(´・ω・｀)");
}

template<typename T>
T Evaluator::ProcessBinaryOperator(T left,T right,string operatorType,BinaryOperatorNode *node){
	//演算子の処理
	if (operatorType == "+")return left + right;
	if (operatorType == "-")return left - right;
	if (operatorType == "*")return left * right;
	if (operatorType == "/"){
		if (right == 0)
			throw RuntimeException("Division by zero.",node->lineNumber,node->columnNumber);
		return left / right;
	}
	if (operatorType == "<")return left < right;
	if (operatorType == "<=") return left <= right;
	if (operatorType == ">") return left > right;
	if (operatorType == ">=") return left >= right;
	if (operatorType == "==") return left == right;
	if (operatorType == "!=") return left != right;
	if constexpr (is_integral_v<T>){
		if (operatorType == "&&") return left && right;
		if (operatorType == "||") return left || right;
		if (operatorType == "&") return left & right;
		if (operatorType == "|") return left | right;
		if (operatorType == "^") return left ^ right;
		if (operatorType == "<<") return left << right;
		if (operatorType == ">>") return left >> right;
		if (operatorType == "%"){
			if (right == 0)
				throw RuntimeException("Division by zero.",node->lineNumber,node->columnNumber);
			return left % right;
		}
	}
	throw RuntimeException("Unknown Operator.",node->lineNumber,node->columnNumber);
}

void Evaluator::IfStatement(AST *ast){
	//if文の処理
	IfStatementNode *node = static_cast<IfStatementNode*>(ast);
	AST *falseExpr = node->GetFalseExpr();
	if (CalcExpr<long double>(node->GetCondition())){
		evaluate(node->GetTrueExpr());
	}else if (falseExpr != nullptr){
		evaluate(falseExpr);
	}
	return;
}

//戻り値のない関数の処理
void Evaluator::VoidFunction(AST *ast){
	//void関数の処理
	FunctionNode *node = static_cast<FunctionNode*>(ast);
	string functionName = node->GetFunctionName();
	vector<AST*> args = node->GetArgument();
	//組み込み関数
	if (functionName == "print") {
		switch (args.at(0)->GetType()){
			case 0:{
				cout << CalcExpr<long long>(args.at(0)) << endl;
				break;
			}
			case 1:{
				cout << CalcExpr<long double>(args.at(0)) << endl;
				break;
			}
			case 2:{
				//TODO:文字列リテラルの処理
			}
			case 3:throw EvaluatorException("Void function should not return value.");
			default:throw EvaluatorException("Unknown function argument type.");
		}
		return;
	}
	//TODO:ユーザ定義関数
	return;
}

void Evaluator::ProcessVariables(AST *ast){
	AssignmentNode *node = static_cast<AssignmentNode*>(ast);
	string variableName = node->GetVariableName();
	AST *expression = node->GetExpression();
	//変数の存在を確認
	if (varll.count(variableName)){
		varll[variableName] = CalcExpr<long long>(expression);
		return;
	}else if (varld.count(variableName)){
		varld[variableName] = CalcExpr<long double>(expression);
		return;
	}else if (vars.count(variableName)){
		//TODO:文字列リテラルの処理
	}
	varld[variableName] = CalcExpr<long double>(expression);
	return;
}