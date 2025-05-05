#include "OperationType.hpp"
using namespace std;

OperationType& OperationType::operator=(const string& value) {
	// コピーコンストラクタの実装
	type = 2; // string型に設定
	string_value = value;
	int_value = my_stoll(value); // int型の値を初期化
	double_value = my_stold(value); // double型の値を初期化
	bool_value = (value == "") ? false : true; // bool型の値を初期化
	return *this;
}

OperationType& OperationType::operator=(const long long& value) {
	// コピーコンストラクタの実装
	type = 0; // int型に設定
	int_value = value;
	string_value = to_string(value); // string型の値を初期化
	double_value = value; // double型の値を初期化
	bool_value = (value != 0); // bool型の値を初期化
	return *this;
}
OperationType& OperationType::operator=(const long double& value) {
	// コピーコンストラクタの実装
	type = 1; // double型に設定
	double_value = value;
	int_value = value; // int型の値を初期化
	string_value = to_string(value); // string型の値を初期化
	bool_value = (value != 0.0); // bool型の値を初期化
	return *this;
}

OperationType& OperationType::operator=(const bool& value) {
	// コピーコンストラクタの実装
	type = 4; // bool型に設定
	int_value = value ? 1 : 0; // int型の値を初期化
	double_value = value ? 1.0 : 0.0; // double型の値を初期化
	string_value = value ? "1" : ""; // string型の値を初期化
	bool_value = value; // bool型の値を初期化
	return *this;
}

OperationType OperationType::operator+(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value + value.int_value);
	case 1: return OperationType(double_value + value.double_value);
	case 2: return OperationType(string_value + value.string_value);
	case 4: return OperationType(int_value + value.int_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

OperationType OperationType::operator*(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value * value.int_value);
	case 1: return OperationType(double_value * value.double_value);
	case 2: {
		//Pythonみたいな感じ
		string result = string_value;
		if (string_value.size() == 1) return OperationType(string(value.int_value, string_value[0]));
		for (int i = 1; i < value.int_value; i++)result += string_value;
		return OperationType(result);
	}
	case 4: return OperationType(int_value * value.int_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

OperationType OperationType::operator==(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value == value.int_value);
	case 1: return OperationType(double_value == value.double_value);
	case 2: return OperationType(string_value == value.string_value);
	case 4: return OperationType(bool_value == bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

OperationType OperationType::operator!=(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value != value.int_value);
	case 1: return OperationType(double_value != value.double_value);
	case 2: return OperationType(string_value != value.string_value);
	case 4: return OperationType(bool_value != bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

OperationType OperationType::operator&&(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value && value.int_value);
	case 1: return OperationType(double_value && value.double_value);
	case 2: return OperationType(bool_value && bool_value);
	case 4: return OperationType(bool_value && bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

OperationType OperationType::operator||(const OperationType& value) const {
	switch (type) {
	case 0: return OperationType(int_value || value.int_value);
	case 1: return OperationType(double_value || value.double_value);
	case 2: return OperationType(bool_value || bool_value);
	case 4: return OperationType(bool_value || bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

template<typename T>
T OperationType::GetValue() const {
	if constexpr (is_same_v<T, long long>) {
		return int_value;
	} else if constexpr (is_same_v<T, long double>) {
		return double_value;
	} else if constexpr (is_same_v<T, string>) {
		return string_value;
	} else if constexpr (is_same_v<T, bool>) {
		return int_value != 0;
	} else {
		throw EvaluatorException("Unknown type.");
	}
}

template long long OperationType::GetValue<long long>() const;
template long double OperationType::GetValue<long double>() const;
template std::string OperationType::GetValue<std::string>() const;
template bool OperationType::GetValue<bool>() const;


