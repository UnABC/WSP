#ifndef Var_HPP
#define Var_HPP

#include "exception.hpp"
#include <string>
#include <vector>
#include <ranges>

class Var {
protected:
	int type; // 0: int, 1: double, 2: string, 4: bool,10: int array, 11: double array, 12: string array
	long long int_value;
	long double double_value;
	std::string string_value;
	bool bool_value;
	std::vector<Var> array_value;
	//変換関数
	long long my_stoll(const std::string str) const;
	long double my_stold(const std::string str) const;
public:
	Var() : type(-2), int_value(0), double_value(0.0), string_value(""), bool_value(false) {};
	Var(long long          value) : type(0), int_value(value), double_value(value), string_value(std::to_string(value)), bool_value(value != 0) {};
	Var(long double        value) : type(1), int_value(value), double_value(value), string_value(std::to_string(value)), bool_value(value != 0.0) {};
	Var(const std::string& value) : type(2), int_value(my_stoll(value)), double_value(my_stold(value)), string_value(value), bool_value(value != "") {};
	Var(bool               value) : type(4), int_value(value ? 1 : 0), double_value(value ? 1.0 : 0.0), string_value(value ? "1" : ""), bool_value(value) {};
	Var(std::vector<Var> array) : type(10 + (array.empty() ? 0 : array.at(0).type) % 10), int_value(0), double_value(0.0), string_value(""), bool_value(false), array_value(array) {};

	Var& operator=(const std::string& value);
	Var& operator=(const long long& value);
	Var& operator=(const long double& value);
	Var& operator=(const bool& value);
	Var& operator=(const std::vector<Var>& value);

	Var operator+ (const Var& value) const;
	Var operator* (const Var& value) const;
	Var operator==(const Var& value) const;
	Var operator!=(const Var& value) const;
	Var operator&&(const Var& value) const;
	Var operator||(const Var& value) const;
	Var operator< (const Var& value) const;
	Var operator> (const Var& value) const;
	Var operator<=(const Var& value) const;
	Var operator>=(const Var& value) const;
	Var operator<<(const Var& value) const;
	Var operator>>(const Var& value) const;
	Var operator% (const Var& value) const;
	Var operator/ (const Var& value) const;
	Var operator- (const Var& value) const;
	Var operator& (const Var& value) const;
	Var operator| (const Var& value) const;
	Var operator^ (const Var& value) const;
	Var operator! () const;
	Var operator- () const;
	explicit operator bool()const;

	template<typename T>
	T GetValue() const;
	template<typename T>
	T& EditValue();
	std::vector<Var> *GetPointer(){return &array_value;}
	int GetType() const { return type; };
	bool IsZero(int TYPE) const;
};

class StaticVar : public Var {
	std::vector<StaticVar> static_array_value; // 静的配列用の値
public:
	StaticVar() : Var() {};
	StaticVar(long long value) : Var(value) {};
	StaticVar(long double value) : Var(value) {};
	StaticVar(const std::string& value) : Var(value) {};
	StaticVar(bool value) : Var(value) {};
	StaticVar(const Var& value);
	StaticVar(const std::vector<Var>& value);
	StaticVar(const std::vector<StaticVar>& value);

	template<typename T>
	StaticVar& operator=(const T& value);
	StaticVar& operator=(const Var& value);
	StaticVar& operator=(const std::vector<Var>& value);

	void SetType(int type) { this->type = type; };
	using Var::GetValue;
	using Var::EditValue;
	std::vector<StaticVar> GetValue() const;
	std::vector<StaticVar> *GetPointer(){return &static_array_value;}
	std::vector<StaticVar>& EditValue();
	void ResizeArray(size_t size);
	StaticVar update_array();
};


#endif // Var_HPP