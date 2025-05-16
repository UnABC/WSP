#include "Var.hpp"
using namespace std;

Var& Var::operator=(const string& value) {
	// コピーコンストラクタの実装
	type = 2; // string型に設定
	string_value = value;
	int_value = my_stoll(value); // int型の値を初期化
	double_value = my_stold(value); // double型の値を初期化
	bool_value = (value == "") ? false : true; // bool型の値を初期化
	return *this;
}

Var& Var::operator=(const long long& value) {
	// コピーコンストラクタの実装
	type = 0; // int型に設定
	int_value = value;
	string_value = to_string(value); // string型の値を初期化
	double_value = value; // double型の値を初期化
	bool_value = (value != 0); // bool型の値を初期化
	return *this;
}
Var& Var::operator=(const long double& value) {
	// コピーコンストラクタの実装
	type = 1; // double型に設定
	double_value = value;
	int_value = value; // int型の値を初期化
	string_value = to_string(value); // string型の値を初期化
	bool_value = (value != 0.0); // bool型の値を初期化
	return *this;
}

Var& Var::operator=(const bool& value) {
	// コピーコンストラクタの実装
	type = 4; // bool型に設定
	int_value = value ? 1 : 0; // int型の値を初期化
	double_value = value ? 1.0 : 0.0; // double型の値を初期化
	string_value = value ? "1" : ""; // string型の値を初期化
	bool_value = value; // bool型の値を初期化
	return *this;
}

Var Var::operator+(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value + value.int_value);
	case 1: return Var(double_value + value.double_value);
	case 2: return Var(string_value + value.string_value);
	case 4: return Var(int_value + value.int_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator*(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value * value.int_value);
	case 1: return Var(double_value * value.double_value);
	case 2: {
		//Pythonみたいな感じ
		string result = string_value;
		if (string_value.size() == 1) return Var(string(value.int_value, string_value[0]));
		for (int i = 1; i < value.int_value; i++)result += string_value;
		return Var(result);
	}
	case 4: return Var(int_value * value.int_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator==(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value == value.int_value);
	case 1: return Var(double_value == value.double_value);
	case 2: return Var(string_value == value.string_value);
	case 4: return Var(bool_value == value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator!=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value != value.int_value);
	case 1: return Var(double_value != value.double_value);
	case 2: return Var(string_value != value.string_value);
	case 4: return Var(bool_value != value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator&&(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value && value.int_value);
	case 1: return Var(double_value && value.double_value);
	case 2: return Var(bool_value && value.bool_value);
	case 4: return Var(bool_value && value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator||(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value || value.int_value);
	case 1: return Var(double_value || value.double_value);
	case 2: return Var(bool_value || value.bool_value);
	case 4: return Var(bool_value || value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator<(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value < value.int_value);
	case 1: return Var(double_value < value.double_value);
	case 2: return Var(string_value < value.string_value);
	case 4: return Var(bool_value < value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator>(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value > value.int_value);
	case 1: return Var(double_value > value.double_value);
	case 2: return Var(string_value > value.string_value);
	case 4: return Var(bool_value > value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator<=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value <= value.int_value);
	case 1: return Var(double_value <= value.double_value);
	case 2: return Var(string_value <= value.string_value);
	case 4: return Var(bool_value <= value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator>=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value >= value.int_value);
	case 1: return Var(double_value >= value.double_value);
	case 2: return Var(string_value >= value.string_value);
	case 4: return Var(bool_value >= value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator<<(const Var& value) const {
	switch (type) {
	case 0:
		return Var(int_value << value.int_value);
	case 1:
	case 2:
	case 4:
		throw EvaluatorException("<< operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator>>(const Var& value) const {
	switch (type) {
	case 0:
		return Var(int_value >> value.int_value);
	case 1:
	case 2:
	case 4:
		throw EvaluatorException(">> operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator%(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value % value.int_value);
	case 1:
	case 2:
	case 4:
		throw EvaluatorException("% operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator/(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value / value.int_value);
	case 1: return Var(double_value / value.double_value);
	case 2: throw EvaluatorException("/ operator is not supported for string type.");
	case 4:
		throw EvaluatorException("/ operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator-(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value - value.int_value);
	case 1: return Var(double_value - value.double_value);
	case 2: throw EvaluatorException("- operator is not supported for string type.");
	case 4:
		throw EvaluatorException("- operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator&(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value & value.int_value);
	case 1:
	case 2:
		throw EvaluatorException("& operator is not supported for this type.");
	case 4:
		return Var(bool_value && value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator|(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value | value.int_value);
	case 1:
	case 2:
		throw EvaluatorException("| operator is not supported for this type.");
	case 4:
		return Var(bool_value || value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator^(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value ^ value.int_value);
	case 1:
	case 2:
		throw EvaluatorException("^ operator is not supported for this type.");
	case 4:
		return Var(bool_value != value.bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator!() const {
	switch (type) {
	case 0: return Var(!int_value);
	case 1: return Var(!double_value);
	case 2: return Var(string_value == "" ? "1" : "");
	case 4: return Var(!bool_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator-() const {
	switch (type) {
	case 0: return Var(-int_value);
	case 1: return Var(-double_value);
	case 2: throw EvaluatorException("- operator is not supported for string type.");
	case 4: throw EvaluatorException("- operator is not supported for bool type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

template<typename T>
T Var::GetValue() const {
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

long long Var::my_stoll(const string str) const {
	try {
		return stoll(str);
	}
	catch (...) {
		return 0; // 変換できない場合は0を返す
	}
}

long double Var::my_stold(const string str) const {
	try {
		return stold(str);
	}
	catch (...) {
		return 0.0; // 変換できない場合は0.0を返す
	}
}

template long long Var::GetValue<long long>() const;
template long double Var::GetValue<long double>() const;
template std::string Var::GetValue<std::string>() const;
template bool Var::GetValue<bool>() const;

StaticVar& StaticVar::operator=(const std::string& value) {
	// コピーコンストラクタの実装
	string_value = value;
	int_value = my_stoll(value); // int型の値を初期化
	double_value = my_stold(value); // double型の値を初期化
	bool_value = (value == "") ? false : true; // bool型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const long long& value) {
	// コピーコンストラクタの実装
	int_value = value;
	string_value = to_string(value); // string型の値を初期化
	double_value = value; // double型の値を初期化
	bool_value = (value != 0); // bool型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const long double& value) {
	// コピーコンストラクタの実装
	double_value = value;
	int_value = value; // int型の値を初期化
	string_value = to_string(value); // string型の値を初期化
	bool_value = (value != 0.0); // bool型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const bool& value) {
	// コピーコンストラクタの実装
	int_value = value ? 1 : 0; // int型の値を初期化
	double_value = value ? 1.0 : 0.0; // double型の値を初期化
	string_value = value ? "1" : ""; // string型の値を初期化
	bool_value = value; // bool型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const Var& value) {
	// コピーコンストラクタの実装
	int_value = value.GetValue<long long>();
	double_value = value.GetValue<long double>();
	string_value = value.GetValue<string>();
	bool_value = value.GetValue<bool>();
	return *this;
}

bool Var::IsZero(int TYPE) const {
	//ゼロかどうかを判定
	switch (TYPE) {
	case 0: return int_value == 0;
	case 1: return double_value == 0.0;
	case 2: return string_value == "";
	case 4: return bool_value == false;
	default: throw EvaluatorException("Unknown type.");
	}
}
