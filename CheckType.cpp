#include "CheckType.hpp"
using namespace std;

void VerifyType::CheckType(AST *ast){
	if (ast == nullptr) return;
	switch (ast->GetNodeType()) {
		case Node::Variable: {
			VariableNode *node = static_cast<VariableNode*>(ast);
			string variableName = node->GetVariableName();
			if (!var_type.count(variableName)) {
				throw CheckTypeException("Undefined variable: " + variableName + ".", node->lineNumber, node->columnNumber);
			}
			//cout << "Variable: " << variableName <<", Type: " << var_type[variableName] << endl;
			node->SetType(var_type[variableName]); //変数の型を保存
			break;
		}
		case Node::Assignment: {
			AssignmentNode *node = static_cast<AssignmentNode*>(ast);
			CheckType(node->GetExpression());
			node->SetType(node->GetExpression()->GetType()); //式の型を保存
			var_type[node->GetVariableName()] = node->GetType(); //変数の型を保存
			break;
		}
		case Node::BinaryOperator: {
			BinaryOperatorNode *node = static_cast<BinaryOperatorNode*>(ast);
			CheckType(node->GetLeft());
			CheckType(node->GetRight());
			int leftType = node->GetLeft()->GetType();
			if (leftType == -1){
				int rightType = node->GetRight()->GetType();
				if (rightType == -1){
					throw CheckTypeException("Invalid type for binary operator: " + node->GetOperatorType() + ".", node->lineNumber, node->columnNumber);
				}
				node->GetLeft()->SetType(node->GetRight()->GetType()); //右辺の型を保存
				node->SetType(rightType); //右辺の型を保存
				break;
			}else if (leftType < 0){
				throw CheckTypeException("Invalid type for binary operator: " + node->GetOperatorType() + ".", node->lineNumber, node->columnNumber);
			}
			node->SetType(leftType); //左辺の型を保存
			break;
		}
		case Node::TernaryOperator: {
			TernaryOperatorNode *node = static_cast<TernaryOperatorNode*>(ast);
			CheckType(node->GetCondition());
			CheckType(node->GetTrueExpr());
			CheckType(node->GetFalseExpr());
			int trueType = node->GetTrueExpr()->GetType();
			int falseType = node->GetFalseExpr()->GetType();
			node->SetType(std::max(trueType, falseType));
			break;
		}
		case Node::Function: {
			FunctionNode *node = static_cast<FunctionNode*>(ast);
			CheckType(node->GetArgument());
			break;
		}
		case Node::IfStatement: {
			IfStatementNode *node = static_cast<IfStatementNode*>(ast);
			CheckType(node->GetCondition());
			CheckType(node->GetTrueExpr());
			CheckType(node->GetFalseExpr());
			break;
		}
		case Node::Statement: {
			StatementNode *node = static_cast<StatementNode*>(ast);
			CheckType(node->GetExpression());
			break;
		}
		case Node::BlockStatement: {
			BlockStatementNode *node = static_cast<BlockStatementNode*>(ast);
			while (AST *statement = node->ReadStatement()) {
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
