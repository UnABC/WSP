#include "CheckType.hpp"
using namespace std;

// 0:int,1:double
VerifyType::VerifyType() {
	//システム定数を指定
	system_var_type["M_PI"] = 1;
	system_var_type["M_E"] = 1;
	system_var_type["M_LOG2E"] = 1;
	system_var_type["M_LOG10E"] = 1;
	system_var_type["M_LN2"] = 1;
	system_var_type["M_LN10"] = 1;
	system_var_type["M_PI_2"] = 1;
	system_var_type["M_PI_4"] = 1;
	system_var_type["M_1_PI"] = 1;
	system_var_type["M_2_PI"] = 1;
	system_var_type["M_2_SQRTPI"] = 1;
	system_var_type["M_SQRT2"] = 1;
	system_var_type["M_SQRT1_2"] = 1;
	system_var_type["M_SQRT3"] = 1;

	//システム関数を指定
	system_func_type["int"] = 0;
	system_func_type["double"] = 1;
	system_func_type["string"] = 2;
	system_func_type["abs"] = 1;
	system_func_type["sqrt"] = 1;
	system_func_type["sin"] = 1;
	system_func_type["cos"] = 1;
	system_func_type["tan"] = 1;
	system_func_type["asin"] = 1;
	system_func_type["acos"] = 1;
	system_func_type["atan"] = 1;
	system_func_type["exp"] = 1;
	system_func_type["log"] = 1;
	system_func_type["log10"] = 1;
	system_func_type["pow"] = 1;
	system_func_type["atan2"] = 1;
}

void VerifyType::CheckType(AST* ast) {
	if (ast == nullptr) return;
	switch (ast->GetNodeType()) {
	case Node::Variable: {
		VariableNode* node = static_cast<VariableNode*>(ast);
		string variableName = node->GetVariableName();
		if (system_var_type.count(variableName)) {
			node->SetType(system_var_type[variableName]); //システム定数の型を保存
			break;
		}
		if (!var_type.count(variableName))
			throw CheckTypeException("Undefined variable: " + variableName + ".", node->lineNumber, node->columnNumber);
		//cout << "Variable: " << variableName <<", Type: " << var_type[variableName] << endl;
		node->SetType(var_type[variableName]); //変数の型を保存
		break;
	}
	case Node::Assignment: {
		AssignmentNode* node = static_cast<AssignmentNode*>(ast);
		CheckType(node->GetExpression());
		node->SetType(node->GetExpression()->GetType()); //式の型を保存
		var_type[node->GetVariableName()] = node->GetType(); //変数の型を保存
		break;
	}
	case Node::StaticVarWithAssignment: {
		StaticVariableNode* node = static_cast<StaticVariableNode*>(ast);
		AssignmentNode* assignment = static_cast<AssignmentNode*>(node->GetAssignment());
		CheckType(assignment);
		var_type[assignment->GetVariableName()] = node->GetType(); //変数の型を保存
		assignment->SetType(node->GetType()); //式の型を保存
		break;
	}
	case Node::StaticVarWithoutAssignment: {
		StaticVarNodeWithoutAssignment* node = static_cast<StaticVarNodeWithoutAssignment*>(ast);
		string variableName = node->GetVariableName();
		if (system_var_type.count(variableName))
			throw CheckTypeException("System variable \"" + variableName + "\" cannot be used as a static variable.", node->lineNumber, node->columnNumber);
		var_type[variableName] = node->GetType(); //変数の型を保存
		break;
	}
	case Node::BinaryOperator: {
		BinaryOperatorNode* node = static_cast<BinaryOperatorNode*>(ast);
		CheckType(node->GetLeft());
		CheckType(node->GetRight());
		int leftType = node->GetLeft()->GetType();
		if (leftType == -1) {
			int rightType = node->GetRight()->GetType();
			if ((rightType == -1) || (rightType == 3)) {
				throw CheckTypeException("Invalid type for binary operator: " + node->GetOperatorType() + ".", node->lineNumber, node->columnNumber);
			}
			node->GetLeft()->SetType(node->GetRight()->GetType()); //右辺の型を保存
			node->SetType(rightType); //右辺の型を保存
			break;
		} else if ((leftType < 0) || (leftType == 3)) {
			throw CheckTypeException("Invalid type for binary operator: " + node->GetOperatorType() + ".", node->lineNumber, node->columnNumber);
		}
		node->SetType(leftType); //左辺の型を保存
		break;
	}
	case Node::TernaryOperator: {
		TernaryOperatorNode* node = static_cast<TernaryOperatorNode*>(ast);
		CheckType(node->GetCondition());
		CheckType(node->GetTrueExpr());
		CheckType(node->GetFalseExpr());
		int trueType = node->GetTrueExpr()->GetType();
		int falseType = node->GetFalseExpr()->GetType();
		node->SetType(std::max(trueType, falseType));
		break;
	}
	case Node::Function: {
		FunctionNode* node = static_cast<FunctionNode*>(ast);
		if (system_func_type.count(node->GetFunctionName()))
			node->SetType(system_func_type[node->GetFunctionName()]);
		for (int i = 0; i < node->GetArgumentSize(); i++)
			CheckType(node->GetArgument().at(i));
		break;
	}
	case Node::IfStatement: {
		IfStatementNode* node = static_cast<IfStatementNode*>(ast);
		CheckType(node->GetCondition());
		CheckType(node->GetTrueExpr());
		CheckType(node->GetFalseExpr());
		break;
	}
	case Node::Statement: {
		StatementNode* node = static_cast<StatementNode*>(ast);
		CheckType(node->GetExpression());
		break;
	}
	case Node::BlockStatement: {
		BlockStatementNode* node = static_cast<BlockStatementNode*>(ast);
		while (AST* statement = node->ReadStatement()) {
			CheckType(statement);
		}
		node->ResetListNumber(); //内部のカウンターをリセット
		break;
	}
	default:
		break;
	}
	return;
}
