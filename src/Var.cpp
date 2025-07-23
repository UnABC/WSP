#include "Var.hpp"
using namespace std;

Var& Var::operator=(const string& value) {
	// コピーコンストラクタの実装
	type = 2; // string型に設定
	this->value.emplace<string>(value);
	return *this;
}

Var& Var::operator=(const long long& value) {
	// コピーコンストラクタの実装
	type = 0; // int型に設定
	this->value.emplace<long long>(value);
	return *this;
}
Var& Var::operator=(const long double& value) {
	// コピーコンストラクタの実装
	type = 1; // double型に設定
	this->value.emplace<long double>(value);
	return *this;
}

Var& Var::operator=(const bool& value) {
	// コピーコンストラクタの実装
	type = 4; // bool型に設定
	this->value.emplace<bool>(value);
	return *this;
}

Var& Var::operator=(const vector<Var>& value) {
	// 配列型の値を設定
	type = 10 + (value.empty() ? 0 : value.at(0).GetType() % 10); // int array型に設定
	this->value.emplace<vector<Var>>(value);
	return *this;
}

Var Var::operator+(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) + value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) + value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) + value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) + value.GetValue<long long>());
	case 10:
	case 11:
	case 12: {
		vector<Var> result(get<vector<Var>>(this->value).size());
		if (value.GetType() >= 10) {
			// vector + vector
			if (value.GetValue<vector<Var>>().size() != get<vector<Var>>(this->value).size())
				throw EvaluatorException("Array sizes do not match for addition.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) + value.GetValue<vector<Var>>().at(i));
		} else {
			// vector + non-vector
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) + value);
		}
		return Var(result);
	}
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator*(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) * value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) * value.GetValue<long double>());
	case 2: {
		//Pythonみたいな感じ
		const size_t len = get<string>(this->value).size();
		const size_t count = (value.GetValue<long long>() > 0) ? value.GetValue<long long>() : 0;
		if ((len == 0) || (count == 0)) return Var(string());
		string result;
		result.resize_and_overwrite(len * count, [&](char* dst, size_t) {
			for (size_t i = 0; i < count; i++)
				memcpy(dst + i * len, get<string>(this->value).data(), len);
			return len * count;
			});
		return Var(result);
	}
	case 4: return Var(get<long long>(this->value) * value.GetValue<long long>());
	case 10:
	case 11:
	case 12: {
		vector<Var> result(get<vector<Var>>(this->value).size());
		if (value.GetType() >= 10) {
			// vector + vector
			if (value.GetValue<vector<Var>>().size() != get<vector<Var>>(this->value).size())
				throw EvaluatorException("Array sizes do not match for multiplication.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) * value.GetValue<vector<Var>>().at(i));
		} else {
			// vector + non-vector
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) * value);
		}
		return Var(result);
	}
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator==(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) == value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) == value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) == value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) == value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value) == value.GetValue<vector<Var>>());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator!=(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) != value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) != value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) != value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) != value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value) != value.GetValue<vector<Var>>());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator&&(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) && value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) && value.GetValue<long double>());
	case 2: return Var(get<bool>(this->value) && value.GetValue<bool>());
	case 4: return Var(get<bool>(this->value) && value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("&& operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator||(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) || value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) || value.GetValue<long double>());
	case 2: return Var(get<bool>(this->value) || value.GetValue<bool>());
	case 4: return Var(get<bool>(this->value) || value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("|| operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator<(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) < value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) < value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) < value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) < value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value).size() < value.GetValue<vector<Var>>().size());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator>(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) > value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) > value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) > value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) > value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value).size() > value.GetValue<vector<Var>>().size());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator<=(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) <= value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) <= value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) <= value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) <= value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value).size() <= value.GetValue<vector<Var>>().size());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator>=(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) >= value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) >= value.GetValue<long double>());
	case 2: return Var(get<string>(this->value) >= value.GetValue<string>());
	case 4: return Var(get<bool>(this->value) >= value.GetValue<bool>());
	case 10:
	case 11:
	case 12:
		return Var(get<vector<Var>>(this->value).size() >= value.GetValue<vector<Var>>().size());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator<<(const Var& value) const {
	switch (type) {
	case 0:
		return Var(get<long long>(this->value) << value.GetValue<long long>());
	case 1:
	case 2:
	case 4:
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("<< operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator>>(const Var& value) const {
	switch (type) {
	case 0:
		return Var(get<long long>(this->value) >> value.GetValue<long long>());
	case 1:
	case 2:
	case 4:
	case 10:
	case 11:
	case 12:
		throw EvaluatorException(">> operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator%(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) % value.GetValue<long long>());
	case 10: {
		vector<Var> result(get<vector<Var>>(this->value).size());
		if (value.GetType() >= 10) {
			// vector + vector
			if (value.GetValue<vector<Var>>().size() != get<vector<Var>>(this->value).size())
				throw EvaluatorException("Array sizes do not match for %.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++) {
				if (value.GetValue<vector<Var>>().at(i).GetValue<long long>() == 0)
					throw EvaluatorException("Division by zero in array % operator.");
				result.at(i) = (get<vector<Var>>(this->value).at(i) % value.GetValue<vector<Var>>().at(i));
			}
		} else {
			// vector + non-vector
			if (value.GetValue<long long>() == 0)
				throw EvaluatorException("Division by zero in array % operator.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) % value);
		}
		return Var(result);
	}
	case 1:
	case 2:
	case 4:
	case 11:
	case 12:
		throw EvaluatorException("% operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator/(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) / value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) / value.GetValue<long double>());
	case 10:
	case 11: {
		vector<Var> result(get<vector<Var>>(this->value).size());
		if (value.GetType() >= 10) {
			// vector + vector
			if (value.GetValue<vector<Var>>().size() != get<vector<Var>>(this->value).size())
				throw EvaluatorException("Array sizes do not match for division.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++) {
				if (value.GetValue<vector<Var>>().at(i).GetValue<long long>() == 0)
					throw EvaluatorException("Division by zero in array / operator.");
				result.at(i) = (get<vector<Var>>(this->value).at(i) / value.GetValue<vector<Var>>().at(i));
			}
		} else {
			// vector + non-vector
			if (value.GetValue<long long>() == 0)
				throw EvaluatorException("Division by zero in array / operator.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) / value);
		}
		return Var(result);
	}
	case 12:
	case 2: throw EvaluatorException("/ operator is not supported for string type.");
	case 4:
		throw EvaluatorException("/ operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator-(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) - value.GetValue<long long>());
	case 1: return Var(get<long double>(this->value) - value.GetValue<long double>());
	case 10:
	case 11: {
		vector<Var> result(get<vector<Var>>(this->value).size());
		if (value.GetType() >= 10) {
			// vector + vector
			if (value.GetValue<vector<Var>>().size() != get<vector<Var>>(this->value).size())
				throw EvaluatorException("Array sizes do not match for subtraction.");
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++) {
				result.at(i) = (get<vector<Var>>(this->value).at(i) - value.GetValue<vector<Var>>().at(i));
			}
		} else {
			// vector + non-vector
			for (size_t i = 0; i < get<vector<Var>>(this->value).size(); i++)
				result.at(i) = (get<vector<Var>>(this->value).at(i) - value);
		}
		return Var(result);
	}
	case 12:
	case 2: throw EvaluatorException("- operator is not supported for string type.");
	case 4:
		throw EvaluatorException("- operator is not supported for this type.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator&(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) & value.GetValue<long long>());
	case 1:
	case 2:
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("& operator is not supported for this type.");
	case 4:
		return Var(get<bool>(this->value) && value.GetValue<bool>());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator|(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) | value.GetValue<long long>());
	case 1:
	case 2:
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("| operator is not supported for this type.");
	case 4:
		return Var(get<bool>(this->value) || value.GetValue<bool>());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator^(const Var& value) const {
	switch (type) {
	case 0: return Var(get<long long>(this->value) ^ value.GetValue<long long>());
	case 1:
	case 2:
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("^ operator is not supported for this type.");
	case 4:
		return Var(get<bool>(this->value) != value.GetValue<bool>());
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator!() const {
	switch (type) {
	case 0: return Var(!get<long long>(this->value));
	case 1: return Var(!get<long double>(this->value));
	case 2: return Var(get<string>(this->value) == "" ? "1" : "");
	case 4: return Var(!get<bool>(this->value));
	case 10:
	case 11:
	case 12:
		throw EvaluatorException("! operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var Var::operator-() const {
	switch (type) {
	case 0: return Var(-get<long long>(this->value));
	case 1: return Var(-get<long double>(this->value));
	case 2: throw EvaluatorException("- operator is not supported for string type.");
	case 4: throw EvaluatorException("- operator is not supported for bool type.");
	case 10:
	case 11: {
		auto view = get<vector<Var>>(this->value) | views::transform([](const Var& v) { return -v; });
		return Var(vector<Var>(view.begin(), view.end()));
	}
	case 12:
		throw EvaluatorException("- operator is not supported for array types.");
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

Var& Var::operator++(int) {
	switch (type) {
	case 0: // int型
		get<long long>(this->value)++;
		break;
	case 1: // double型
		get<long double>(this->value)++;
		break;
	case 2: // string型
		get<string>(this->value) += "1"; // 文字列に"1"を追加
		break;
	case 4: // bool型
		get<bool>(this->value) = true; // bool型はtrueに設定
		break;
	case 10: //array型
	case 11:
	case 12:
		if (!get<vector<Var>>(this->value).empty()) {
			for (auto& v : get<vector<Var>>(this->value))
				v = v + Var(1LL); // 配列の各要素に1を加える
		}
		break;
	default:
		throw EvaluatorException("Unknown type for increment operator.");
	}
	return *this;
}

Var::operator bool() const {
	switch (type) {
	case 0: return get<long long>(this->value) != 0;
	case 1: return get<long double>(this->value) != 0.0;
	case 2: return get<string>(this->value) != "";
	case 4: return get<bool>(this->value);
	case 10:
	case 11:
	case 12:
		return !get<vector<Var>>(this->value).empty();
	default: throw EvaluatorException("Unknown type: " + to_string(type));
	}
}

template<typename T>
T Var::GetValue() const {
	if constexpr (is_same_v<T, long long>) {
		switch (type) {
		case 0: return get<long long>(this->value);
		case 1: return static_cast<long long>(get<long double>(this->value));
		case 2: return my_stoll(get<string>(this->value));
		case 4: return get<bool>(this->value) ? 1 : 0;
		case 10:
			if (get<vector<Var>>(this->value).empty()) return 0;
			return get<vector<Var>>(this->value).at(0).GetValue<long long>();
		case 11:
			if (get<vector<Var>>(this->value).empty()) return 0;
			return static_cast<long long>(get<vector<Var>>(this->value).at(0).GetValue<long double>());
		case 12:
			if (get<vector<Var>>(this->value).empty()) return 0;
			return my_stoll(get<vector<Var>>(this->value).at(0).GetValue<string>());
		default: throw EvaluatorException("Unknown type: " + to_string(type));
		}
	} else if constexpr (is_same_v<T, long double>) {
		switch (type) {
		case 0: return static_cast<long double>(get<long long>(this->value));
		case 1: return get<long double>(this->value);
		case 2: return my_stold(get<string>(this->value));
		case 4: return get<bool>(this->value) ? 1.0 : 0.0;
		case 10:
			if (get<vector<Var>>(this->value).empty()) return 0.0;
			return static_cast<long double>(get<vector<Var>>(this->value).at(0).GetValue<long long>());
		case 11: return get<vector<Var>>(this->value).empty() ? 0.0 : get<vector<Var>>(this->value).at(0).GetValue<long double>();
		case 12:
			if (get<vector<Var>>(this->value).empty()) return 0.0;
			return my_stold(get<vector<Var>>(this->value).at(0).GetValue<string>());
		default: throw EvaluatorException("Unknown type: " + to_string(type));
		}
	} else if constexpr (is_same_v<T, string>) {
		switch (type) {
		case 0: return to_string(get<long long>(this->value));
		case 1: return to_string(get<long double>(this->value));
		case 2: return get<string>(this->value);
		case 4: return get<bool>(this->value) ? "1" : "";
		case 10:
			if (get<vector<Var>>(this->value).empty()) return "";
			return get<vector<Var>>(this->value).at(0).GetValue<string>();
		case 11:
			if (get<vector<Var>>(this->value).empty()) return "";
			return to_string(get<vector<Var>>(this->value).at(0).GetValue<long double>());
		case 12:
			if (get<vector<Var>>(this->value).empty()) return "";
			return get<vector<Var>>(this->value).at(0).GetValue<string>();
		default: throw EvaluatorException("Unknown type: " + to_string(type));
		}
	} else if constexpr (is_same_v<T, bool>) {
		switch (type) {
		case 0: return get<long long>(this->value) != 0;
		case 1: return get<long double>(this->value) != 0.0;
		case 2: return !get<string>(this->value).empty();
		case 4: return get<bool>(this->value);
		case 10:
			if (get<vector<Var>>(this->value).empty()) return false;
			return get<vector<Var>>(this->value).at(0).GetValue<bool>();
		case 11:
			if (get<vector<Var>>(this->value).empty()) return false;
			return get<vector<Var>>(this->value).at(0).GetValue<long double>() != 0.0;
		case 12:
			if (get<vector<Var>>(this->value).empty()) return false;
			return !get<vector<Var>>(this->value).at(0).GetValue<string>().empty();
		default: throw EvaluatorException("Unknown type: " + to_string(type));
		}
	} else if constexpr (is_same_v<T, vector<Var>>) {
		switch (type)
		{
		case 0:
			return vector<Var>(1, Var(get<long long>(this->value)));
		case 1:
			return vector<Var>(1, Var(get<long double>(this->value)));
		case 2:
			return vector<Var>(1, Var(get<string>(this->value)));
		case 4:
			return vector<Var>(1, Var(get<bool>(this->value)));
		case 10:
		case 11:
		case 12:
			return get<vector<Var>>(this->value);
		default: throw EvaluatorException("Unknown type: " + to_string(type));
		}
	} else {
		throw EvaluatorException("Unknown value type.");
	}
}

template<typename T>
T& Var::EditValue() {
	if constexpr (is_same_v<T, long long>) {
		value.emplace<long long>(GetValue<long long>());
		type = 0;
		return get<long long>(value);
	} else if constexpr (is_same_v<T, long double>) {
		value.emplace<long double>(GetValue<long double>());
		type = 1;
		return get<long double>(value);
	} else if constexpr (is_same_v<T, string>) {
		value.emplace<string>(GetValue<string>());
		type = 2;
		return get<string>(value);
	} else if constexpr (is_same_v<T, bool>) {
		value.emplace<bool>(GetValue<bool>());
		type = 4;
		return get<bool>(value);
	} else if constexpr (is_same_v<T, vector<Var>>) {
		return get<vector<Var>>(value);
	} else {
		throw EvaluatorException("Unknown value type.");
	}
}

long long Var::my_stoll(const string str) const {
	long long value;
	const auto result = from_chars(str.data(), str.data() + str.size(), value);
	if (result.ec == errc())
		return value;
	return 0LL;
}

long double Var::my_stold(const string str) const {
	long double value;
	const auto result = from_chars(str.data(), str.data() + str.size(), value);
	if (result.ec == errc())
		return value;
	return 0.0L;
}

template long long Var::GetValue<long long>() const;
template long double Var::GetValue<long double>() const;
template string Var::GetValue<string>() const;
template bool Var::GetValue<bool>() const;
template vector<Var> Var::GetValue<vector<Var>>() const;

template long long& Var::EditValue<long long>();
template long double& Var::EditValue<long double>();
template string& Var::EditValue<string>();
template bool& Var::EditValue<bool>();
template vector<Var>& Var::EditValue<vector<Var>>();

StaticVar::StaticVar(const Var& value) : Var(value) {
	if (value.GetType() < 10)
		return;
	static_array_value.clear();
	static_array_value.reserve(value.GetValue<vector<Var>>().size());
	for (const auto& v : value.GetValue<vector<Var>>())
		static_array_value.push_back(StaticVar(v));
}

StaticVar::StaticVar(const vector<Var>& value) : Var(value) {
	static_array_value.clear();
	static_array_value.reserve(value.size());
	for (const auto& v : value)
		static_array_value.push_back(StaticVar(v));
}

StaticVar::StaticVar(const vector<StaticVar>& value) : Var() {
	static_array_value = value;
	vector<Var> base_array;
	base_array.reserve(value.size());
	for (const auto& sv : value)
		base_array.push_back(sv);
	if (!base_array.empty()) {
		this->type = (base_array.at(0).GetType() % 10) + 10;
		(this->value).emplace<vector<Var>>(base_array); // 配列の値を設定
	} else {
		this->type = 10; // 空の場合はデフォルトで int array type
		(this->value).emplace<vector<Var>>(vector<Var>()); // 空の配列を設定
	}
}

template<typename T>
StaticVar& StaticVar::operator=(const T& value) {
	if (type < 10) {
		int const_type = type;
		operator=(value);
		type = const_type; // 元の型を保持
	} else {
		for (auto& v : get<vector<Var>>(this->value))
			v = value;
	}
	return *this;
}

StaticVar& StaticVar::operator=(const Var& value) {
	// 配列型の値を設定
	switch (type) {
	case 0: this->value.emplace<long long>(value.GetValue<long long>()); return *this;
	case 1: this->value.emplace<long double>(value.GetValue<long double>()); return *this;
	case 2: this->value.emplace<string>(value.GetValue<string>()); return *this;
	case 4: this->value.emplace<bool>(value.GetValue<bool>()); return *this;
	case 10:
	case 11:
	case 12: {
		for (Var& v : get<vector<Var>>(this->value))
			v = value; // 配列の各要素に値を設定
		static_array_value.clear(); // 静的配列用の値をクリア
		static_array_value.reserve(get<vector<Var>>(this->value).size());
		for (const auto& v : get<vector<Var>>(this->value))
			static_array_value.push_back(StaticVar(v));
		return *this;
	}
	default:
		break;
	}
	// typeに関する情報が何もないとき
	switch (value.GetType()) {
	case 0: this->value.emplace<long long>(value.GetValue<long long>()); type = 0; break;
	case 1: this->value.emplace<long double>(value.GetValue<long double>()); type = 1; break;
	case 2: this->value.emplace<string>(value.GetValue<string>()); type = 2; break;
	case 4: this->value.emplace<bool>(value.GetValue<bool>()); type = 4; break;
	case 10:
	case 11:
	case 12: {
		this->value.emplace<vector<Var>>(value.GetValue<vector<Var>>());
		static_array_value.clear(); // 静的配列用の値をクリア
		static_array_value.reserve(get<vector<Var>>(this->value).size());
		for (const auto& v : get<vector<Var>>(this->value))
			static_array_value.push_back(StaticVar(v));
		type = (get<vector<Var>>(this->value).empty() ? 10 : get<vector<Var>>(this->value).at(0).GetType() % 10) + 10;
		break;
	}
	default:
		throw EvaluatorException("Unknown type for StaticVar.: " + to_string(value.GetType()));
	}
	return *this;
}

StaticVar& StaticVar::operator=(const vector<Var>& value) {
	int const_type = type;
	operator=(value);
	type = const_type; // 元の型を保持
	if (type < 10)
		return *this; // 配列型でない場合は終了
	static_array_value.clear(); // 静的配列用の値をクリア
	static_array_value.reserve(value.size());
	for (const auto& v : value)
		static_array_value.push_back(StaticVar(v));
	return *this;
}

vector<StaticVar> StaticVar::GetValue() const {
	return static_array_value;
}

vector<StaticVar>& StaticVar::EditValue() {
	return static_array_value;
}

void StaticVar::ExpandArray(size_t size) {
	// 静的配列のサイズを変更
	StaticVar new_var;
	new_var.type = type; // 新しい型を設定
	while (static_array_value.size() < size) {
		static_array_value.push_back(new_var);
		get<vector<Var>>(this->value).push_back(new_var); // 配列にも追加
	}
	return;
}

StaticVar StaticVar::update_array() {
	// 静的配列を更新
	get<vector<Var>>(this->value).clear(); // 既存の配列をクリア
	get<vector<Var>>(this->value).reserve(static_array_value.size()); // 配列のサイズを予約
	for (const auto& sv : static_array_value)
		get<vector<Var>>(this->value).push_back(sv);
	return *this;
}

void StaticVar::SetType(int new_type) {
	switch (new_type)
	{
	case 0:
		(this->value).emplace<long long>(GetValue<long long>());
		break;
	case 1:
		(this->value).emplace<long double>(GetValue<long double>());
		break;
	case 2:
		(this->value).emplace<string>(GetValue<string>());
		break;
	case 4:
		(this->value).emplace<bool>(GetValue<bool>());
		break;
	case 10:
	case 11:
	case 12:
		break;
	default:
		throw EvaluatorException("Unknown type for StaticVar.: " + to_string(new_type));
	}
	type = new_type; // 新しい型を設定
}

bool Var::IsZero(int TYPE) const {
	//ゼロかどうかを判定
	switch (TYPE) {
	case 0: return GetValue<long long>() == 0;
	case 1: return GetValue<long double>() == 0.0;
	case 2: return GetValue<string>().empty();
	case 4: return GetValue<bool>() == false;
	case 10:
	case 11:
	case 12:
		return get<vector<Var>>(this->value).empty();
	default: throw EvaluatorException("Unknown type.: " + to_string(TYPE));
	}
}

template StaticVar& StaticVar::operator=<string>(const string&);
template StaticVar& StaticVar::operator=<long long>(const long long&);
template StaticVar& StaticVar::operator=<long double>(const long double&);
template StaticVar& StaticVar::operator=<bool>(const bool&);