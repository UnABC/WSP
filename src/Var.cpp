#include "Var.hpp"
using namespace std;

Var& Var::operator=(const string& value) {
	// コピーコンストラクタの実装
	type = 2; // string型に設定
	string_value = value;
	int_value = my_stoll(value); // int型の値を初期化
	double_value = my_stold(value); // double型の値を初期化
	bool_value = (value == "") ? false : true; // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

Var& Var::operator=(const long long& value) {
	// コピーコンストラクタの実装
	type = 0; // int型に設定
	int_value = value;
	string_value = to_string(value); // string型の値を初期化
	double_value = value; // double型の値を初期化
	bool_value = (value != 0); // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}
Var& Var::operator=(const long double& value) {
	// コピーコンストラクタの実装
	type = 1; // double型に設定
	double_value = value;
	int_value = value; // int型の値を初期化
	string_value = to_string(value); // string型の値を初期化
	bool_value = (value != 0.0); // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

Var& Var::operator=(const bool& value) {
	// コピーコンストラクタの実装
	type = 4; // bool型に設定
	int_value = value ? 1 : 0; // int型の値を初期化
	double_value = value ? 1.0 : 0.0; // double型の値を初期化
	string_value = value ? "1" : ""; // string型の値を初期化
	bool_value = value; // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

Var& Var::operator=(const vector<Var>& value) {
	// 配列型の値を設定
	type = value.empty() ? 10 : value.at(0).GetType(); // int array型に設定
	array_value.clear(); // 既存の配列をクリア
	array_value.reserve(value.size()); // 配列のサイズを予約
	array_value = value;
	int_value = 0; // int型の値は初期化
	double_value = 0.0; // double型の値は初期化
	string_value = ""; // string型の値は初期化
	bool_value = false; // bool型の値は初期化
	if (!value.empty()) {
		switch (type) {
		case 10:int_value = value.at(0).int_value; break;
		case 11:double_value = value.at(0).double_value; break;
		case 12:string_value = value.at(0).string_value; break;
		}
	}
	return *this;
}

Var Var::operator+(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value + value.int_value);
	case 1: return Var(double_value + value.double_value);
	case 2: return Var(string_value + value.string_value);
	case 4: return Var(int_value + value.int_value);
	case 10:
	case 11:
	case 12: {
		if (value.array_value.size() == 1) {
			vector<Var> result(array_value.size());
			for (size_t i = 0; i < array_value.size(); i++)
				result.at(i) = (array_value.at(i) + value.array_value.at(0));
			return Var(result);
		}
		if (array_value.size() != value.array_value.size())
			throw EvaluatorException("Array sizes do not match for addition.");
		vector<Var> result(array_value.size());
		for (size_t i = 0; i < array_value.size(); i++)
			result.at(i) = (array_value.at(i) + value.array_value[i]);
		return Var(result);
	}
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator*(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value * value.int_value);
	case 1: return Var(double_value * value.double_value);
	case 2: {
		//Pythonみたいな感じ
		const size_t len = string_value.size();
		const size_t count = (value.int_value > 0) ? value.int_value : 0;
		if ((len == 0) || (count == 0)) return Var(string());
		string result;
		result.resize_and_overwrite(len * count, [&](char* dst, size_t) {
			for (size_t i = 0; i < count; i++)
				memcpy(dst + i * len, string_value.data(), len);
			return len * count;
			});
		return Var(result);
	}
	case 4: return Var(int_value * value.int_value);
	case 10:
	case 11:
	case 12: {
		if (value.array_value.size() == 1) {
			vector<Var> result(array_value.size());
			for (size_t i = 0; i < array_value.size(); i++)
				result.at(i) = (array_value.at(i) * value.array_value.at(0));
			return Var(result);
		}
		if (array_value.size() != value.array_value.size())
			throw EvaluatorException("Array sizes do not match for addition.");
		vector<Var> result(array_value.size());
		for (size_t i = 0; i < array_value.size(); i++)
			result.at(i) = (array_value.at(i) * value.array_value[i]);
		return Var(result);
	}
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator==(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value == value.int_value);
	case 1: return Var(double_value == value.double_value);
	case 2: return Var(string_value == value.string_value);
	case 4: return Var(bool_value == value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value == value.array_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator!=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value != value.int_value);
	case 1: return Var(double_value != value.double_value);
	case 2: return Var(string_value != value.string_value);
	case 4: return Var(bool_value != value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value != value.array_value);
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator&&(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value && value.int_value);
	case 1: return Var(double_value && value.double_value);
	case 2: return Var(bool_value && value.bool_value);
	case 4: return Var(bool_value && value.bool_value);
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("&& operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator||(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value || value.int_value);
	case 1: return Var(double_value || value.double_value);
	case 2: return Var(bool_value || value.bool_value);
	case 4: return Var(bool_value || value.bool_value);
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("|| operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator<(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value < value.int_value);
	case 1: return Var(double_value < value.double_value);
	case 2: return Var(string_value < value.string_value);
	case 4: return Var(bool_value < value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value.size() < value.array_value.size());
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator>(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value > value.int_value);
	case 1: return Var(double_value > value.double_value);
	case 2: return Var(string_value > value.string_value);
	case 4: return Var(bool_value > value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value.size() > value.array_value.size());
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator<=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value <= value.int_value);
	case 1: return Var(double_value <= value.double_value);
	case 2: return Var(string_value <= value.string_value);
	case 4: return Var(bool_value <= value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value.size() <= value.array_value.size());
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator>=(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value >= value.int_value);
	case 1: return Var(double_value >= value.double_value);
	case 2: return Var(string_value >= value.string_value);
	case 4: return Var(bool_value >= value.bool_value);
	case 10:
	case 11:
	case 12:
		return Var(array_value.size() >= value.array_value.size());
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
	case 10:
	case 11:
	case 12:
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
	case 10:
	case 11:
	case 12:
		throw EvaluatorException(">> operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator%(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value % value.int_value);
	case 10: {
		if (value.array_value.size() == 1) {
			if (value.array_value.at(0).int_value == 0)
				throw EvaluatorException("Division by zero in array % operator.");
			vector<Var> result(array_value.size());
			for (size_t i = 0; i < array_value.size(); i++)
				result.at(i) = (array_value.at(i) % value.array_value.at(0));
			return Var(result);
		}
		if (array_value.size() != value.array_value.size())
			throw EvaluatorException("Array sizes do not match for addition.");
		vector<Var> result(array_value.size());
		for (size_t i = 0; i < array_value.size(); i++) {
			if (value.array_value[i].int_value == 0)
				throw EvaluatorException("Division by zero in array % operator.");
			result.at(i) = (array_value.at(i) % value.array_value[i]);
		}
		return Var(result);
	}
	case 1:
	case 2:
	case 4:
	case 11:
	case 12:
		throw EvaluatorException("% operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator/(const Var& value) const {
	switch (type) {
	case 0: return Var(int_value / value.int_value);
	case 1: return Var(double_value / value.double_value);
	case 10:
	case 11: {
		if (value.array_value.size() == 1) {
			if (value.array_value.at(0).int_value == 0 || value.array_value.at(0).double_value == 0.0)
				throw EvaluatorException("Division by zero in array / operator.");
			vector<Var> result(array_value.size());
			for (size_t i = 0; i < array_value.size(); i++)
				result.at(i) = (array_value.at(i) / value.array_value.at(0));
			return Var(result);
		}
		if (array_value.size() != value.array_value.size())
			throw EvaluatorException("Array sizes do not match for addition.");
		vector<Var> result(array_value.size());
		for (size_t i = 0; i < array_value.size(); i++) {
			if (value.array_value[i].int_value == 0 || value.array_value[i].double_value == 0.0)
				throw EvaluatorException("Division by zero in array / operator.");
			result.at(i) = (array_value.at(i) / value.array_value[i]);
		}
		return Var(result);
	}
	case 12:
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
	case 10:
	case 11: {
		if (value.array_value.size() == 1) {
			vector<Var> result(array_value.size());
			for (size_t i = 0; i < array_value.size(); i++)
				result.at(i) = (array_value.at(i) - value.array_value.at(0));
			return Var(result);
		}
		if (array_value.size() != value.array_value.size())
			throw EvaluatorException("Array sizes do not match for subtraction.");
		vector<Var> result(array_value.size());
		for (size_t i = 0; i < array_value.size(); i++)
			result.at(i) = (array_value.at(i) - value.array_value.at(i));
		return Var(result);
	}
	case 12:
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
	case 10:
	case 11:
	case 12:
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
	case 10:
	case 11:
	case 12:
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
	case 10:
	case 11:
	case 12:
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
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("! operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var Var::operator-() const {
	switch (type) {
	case 0: return Var(-int_value);
	case 1: return Var(-double_value);
	case 2: throw EvaluatorException("- operator is not supported for string type.");
	case 4: throw EvaluatorException("- operator is not supported for bool type.");
	case 10:
	case 11: {
		auto view = array_value | views::transform([](const Var& v) { return -v; });
		return Var(vector<Var>(view.begin(), view.end()));
	}
	case 12:
		throw EvaluatorException("- operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type.");
	}
}

Var::operator bool() const {
	switch (type) {
	case 0: return int_value != 0;
	case 1: return double_value != 0.0;
	case 2: return string_value != "";
	case 4: return bool_value;
	case 10:
	case 11:
	case 12:
		return !array_value.empty();
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
	} else if constexpr (is_same_v<T, vector<Var>>) {
		return array_value;
	} else {
		throw EvaluatorException("Unknown value type.");
	}
}

template<typename T>
T& Var::EditValue() {
	if constexpr (is_same_v<T, long long>) {
		return int_value;
	} else if constexpr (is_same_v<T, long double>) {
		return double_value;
	} else if constexpr (is_same_v<T, string>) {
		return string_value;
	} else if constexpr (is_same_v<T, bool>) {
		return bool_value;
	} else if constexpr (is_same_v<T, vector<Var>>) {
		return array_value;
	} else {
		throw EvaluatorException("Unknown value type.");
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
template std::vector<Var> Var::GetValue<std::vector<Var>>() const;
template std::vector<StaticVar> Var::GetValue<std::vector<StaticVar>>() const;

template long long& Var::EditValue<long long>();
template long double& Var::EditValue<long double>();
template std::string& Var::EditValue<std::string>();
template bool& Var::EditValue<bool>();
template std::vector<Var>& Var::EditValue<std::vector<Var>>();
template std::vector<StaticVar>& Var::EditValue<std::vector<StaticVar>>();

StaticVar& StaticVar::operator=(const std::string& value) {
	// コピーコンストラクタの実装
	string_value = value;
	int_value = my_stoll(value); // int型の値を初期化
	double_value = my_stold(value); // double型の値を初期化
	bool_value = (value == "") ? false : true; // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const long long& value) {
	// コピーコンストラクタの実装
	int_value = value;
	string_value = to_string(value); // string型の値を初期化
	double_value = value; // double型の値を初期化
	bool_value = (value != 0); // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const long double& value) {
	// コピーコンストラクタの実装
	double_value = value;
	int_value = value; // int型の値を初期化
	string_value = to_string(value); // string型の値を初期化
	bool_value = (value != 0.0); // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const bool& value) {
	// コピーコンストラクタの実装
	int_value = value ? 1 : 0; // int型の値を初期化
	double_value = value ? 1.0 : 0.0; // double型の値を初期化
	string_value = value ? "1" : ""; // string型の値を初期化
	bool_value = value; // bool型の値を初期化
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, Var(value)); // 配列型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const Var& value) {
	// コピーコンストラクタの実装
	int_value = value.GetValue<long long>();
	double_value = value.GetValue<long double>();
	string_value = value.GetValue<string>();
	bool_value = value.GetValue<bool>();
	array_value.clear(); // 配列型の値は初期化
	array_value.resize(1, value); // 配列型の値を初期化
	return *this;
}

StaticVar& StaticVar::operator=(const std::vector<Var>& value) {
	// 配列型の値を設定
	array_value = value;
	int_value = 0; // int型の値は初期化
	double_value = 0.0; // double型の値は初期化
	string_value = ""; // string型の値は初期化
	bool_value = false; // bool型の値は初期化
	if (!value.empty()) {
		switch (type) {
		case 10:int_value = value.at(0).GetValue<long long>(); break;
		case 11:double_value = value.at(0).GetValue<long double>(); break;
		case 12:string_value = value.at(0).GetValue<string>(); break;
		}
	}
	return *this;
}

bool Var::IsZero(int TYPE) const {
	//ゼロかどうかを判定
	switch (TYPE) {
	case 0: return int_value == 0;
	case 1: return double_value == 0.0;
	case 2: return string_value == "";
	case 4: return bool_value == false;
	case 10:
	case 11:
	case 12:
		return array_value.empty();
	default: throw EvaluatorException("Unknown type.");
	}
}