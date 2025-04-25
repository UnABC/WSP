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
	return;
}

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
			T left = CalcExpr<T>(node->GetLeft());
			T right = CalcExpr<T>(node->GetRight());
			if (operatorType == "+")return left + right;
			if (operatorType == "-")return left - right;
			if (operatorType == "*")return left * right;
			if (operatorType == "/"){
				if (right == 0){
					throw EvaluatorException("RuntimeError: Division by zero.");
				}
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
					if (right == 0){
						throw EvaluatorException("RuntimeError: Division by zero.");
					}
					return left % right;
				}
			}
			throw EvaluatorException("Unknown Operator.");
		}
		//三項演算子
		case Node::TernaryOperator: {
			TernaryOperatorNode *node = static_cast<TernaryOperatorNode*>(ast);
			if (CalcExpr<T>(node->GetCondition())){
				return CalcExpr<T>(node->GetTrueExpr());
			}else{
				return CalcExpr<T>(node->GetFalseExpr());
			}
		}
		case Node::Function: {
			FunctionNode *node = static_cast<FunctionNode*>(ast);
			string functionName = node->GetFunctionName();
			AST *arg = node->GetArgument();
			//各種数学関数
			if (functionName == "abs") return abs(CalcExpr<T>(arg));
			if (functionName == "sqrt") return sqrt(CalcExpr<T>(arg));
			if (functionName == "sin") return sin(CalcExpr<T>(arg));
			if (functionName == "cos") return cos(CalcExpr<T>(arg));
			if (functionName == "tan") return tan(CalcExpr<T>(arg));
			if (functionName == "asin") return asin(CalcExpr<T>(arg));
			if (functionName == "acos") return acos(CalcExpr<T>(arg));
			if (functionName == "atan") return atan(CalcExpr<T>(arg));
			if (functionName == "exp") return exp(CalcExpr<T>(arg));
			if (functionName == "log") return log(CalcExpr<T>(arg));
			if (functionName == "log10") return log10(CalcExpr<T>(arg));
			//その他組み込み関数
			if (functionName == "print") {
				cout << CalcExpr<T>(arg) << endl;
				return 0;
			}
			throw EvaluatorException("Unknown function.");
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
			return varld[variableName] = CalcExpr<long double>(expression);
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
				throw EvaluatorException("Undefined variable: " + variableName);
			}
		}
		default:{
			throw EvaluatorException("Unknown node type.");
		}
	}
	throw EvaluatorException("Fatal Error:This message should not be displayed.(´・ω・｀)");
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

void Evaluator::VoidFunction(AST *ast){
	//void関数の処理
	FunctionNode *node = static_cast<FunctionNode*>(ast);
	string functionName = node->GetFunctionName();
	AST *arg = node->GetArgument();
	//組み込み関数
	if (functionName == "print") {
		cout << CalcExpr<long double>(arg) << endl;
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