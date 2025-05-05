#ifndef OPERATIONTYPE_HPP
#define OPERATIONTYPE_HPP

#include "exception.hpp"
#include "evaluator.hpp"
#include <string>

class OperationType :public Evaluator {
private:
	int type; // 0: int, 1: double, 2: string, 4: bool
	long long int_value;
	long double double_value;
	std::string string_value;
	bool bool_value;
public:
	OperationType(long long          value) : type(0), int_value(value), double_value(value), string_value(std::to_string(value)), bool_value(value != 0) {};
	OperationType(long double        value) : type(1), int_value(value), double_value(value), string_value(std::to_string(value)), bool_value(value != 0.0) {};
	OperationType(const std::string& value) : type(2), int_value(my_stoll(value)), double_value(my_stold(value)), string_value(value), bool_value(value != "") {};
	OperationType(bool               value) : type(4), int_value(value ? 1 : 0), double_value(value ? 1.0 : 0.0), string_value(value ? "1" : ""), bool_value(value) {};

	OperationType& operator=(const std::string& value);
	OperationType& operator=(const long long& value);
	OperationType& operator=(const long double& value);
	OperationType& operator=(const bool& value);

	OperationType operator+ (const OperationType& value) const;
	OperationType operator* (const OperationType& value) const;
	OperationType operator==(const OperationType& value) const;
	OperationType operator!=(const OperationType& value) const;
	OperationType operator&&(const OperationType& value) const;
	OperationType operator||(const OperationType& value) const;
	OperationType operator< (const OperationType& value) const;
	OperationType operator> (const OperationType& value) const;
	OperationType operator<=(const OperationType& value) const;
	OperationType operator>=(const OperationType& value) const;
	OperationType operator<<(const OperationType& value) const;
	OperationType operator>>(const OperationType& value) const;
	OperationType operator% (const OperationType& value) const;
	OperationType operator/ (const OperationType& value) const;
	OperationType operator- (const OperationType& value) const;
	OperationType operator& (const OperationType& value) const;
	OperationType operator| (const OperationType& value) const;
	OperationType operator^ (const OperationType& value) const;
	OperationType operator! () const;

	template<typename T>
	T GetValue() const;

};
#endif // OPERATIONTYPE_HPP