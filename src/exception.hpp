#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <string>

class LexerException : public std::exception {
	std::string msg;
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
public:
	explicit LexerException(const std::string& message, unsigned long long line, unsigned long long column)
		: msg(message), lineNumber(line), columnNumber(column) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}

	const char* where() const noexcept {
		return ("Line: " + std::to_string(lineNumber) + ", Column: " + std::to_string(columnNumber)).c_str();
	}

};

class ParserException : public std::exception {
	std::string msg;
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
public:
	explicit ParserException(const std::string& message, unsigned long long line, unsigned long long column)
		: msg(message), lineNumber(line), columnNumber(column) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}

	const char* where() const noexcept {
		return ("Line: " + std::to_string(lineNumber) + ", Column: " + std::to_string(columnNumber)).c_str();
	}

};

class EvaluatorException : public std::exception {
	std::string msg;
public:
	explicit EvaluatorException(const std::string& message)
		: msg(message) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class CheckTypeException : public std::exception {
	std::string msg;
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
public:
	explicit CheckTypeException(const std::string& message, unsigned long long line, unsigned long long column)
		: msg(message), lineNumber(line), columnNumber(column) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
	const char* where() const noexcept {
		return ("Line: " + std::to_string(lineNumber) + ", Column: " + std::to_string(columnNumber)).c_str();
	}
};

class RuntimeException : public std::exception {
	std::string msg;
	unsigned long long lineNumber;	//行番号
	unsigned long long columnNumber;	//列番号
public:
	explicit RuntimeException(const std::string& message, unsigned long long line, unsigned long long column)
		: msg(message), lineNumber(line), columnNumber(column) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
	const char* where() const noexcept {
		return ("Line: " + std::to_string(lineNumber) + ", Column: " + std::to_string(columnNumber)).c_str();
	}
};

class CalcException : public std::exception {
	std::string msg;
public:
	explicit CalcException(const std::string& message)
		: msg(message) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class WindowException : public std::exception {
	std::string msg;
public:
	explicit WindowException(const std::string& message)
		: msg(message) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class FontException : public std::exception {
	std::string msg;
public:
	explicit FontException(const std::string& message)
		: msg(message) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class ShaderException : public std::exception {
	std::string msg;
public:
	explicit ShaderException(const std::string& message)
		: msg(message) {
	};
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

#endif // EXCEPTION_HPP