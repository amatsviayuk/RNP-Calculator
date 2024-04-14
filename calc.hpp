#pragma once

#include "stack.hpp"
#include "string.hpp"

class Calculator {
private:
	void GetInput();

	void ToRPN();

	uint8_t GetPriority(const String& token);

	bool IsOperator(const String& token);

	bool IsNumber(const String& token);

	bool IsFunction(const String& token);

	void ProcessRPN();

public:
	Calculator();

	Calculator(uint32_t lines_amt);

	~Calculator();

	void Process();

	void GetResult();

private:
	uint32_t commands_amt_;

	// array of commands line-by-line
	List<String>* commands_;
};