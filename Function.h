#pragma once

#include <functional>
#include <string>

#include "Terminal.h"

using namespace std;

class Function {
private:
	string name;
	int parity;
	function<double(const double& a, const double& b)> func;

public:
	Function();
	Function(string name, int parity, function <double(const double& a, const double &b) > func);
	Function(const Function& function);
	double evaluate(const double& a, const double& b) const;

	string getName() const;
	int getParity() const;
	friend bool operator==(const Function& lhs, const Function& rhs);
};

class FunctionSet {
private:
	vector<Function> functions;

public:
	FunctionSet(const int & size = 0);
	void addFunction(const Function& func);
	static FunctionSet createArithmeticFunctionSet();
	static FunctionSet createBinaryOnlyFunctionSet();
	static FunctionSet createArithmeticFunctionSetNoDivide();

	Function getRandomFunction() const;
	Function getRandomFunction(const map<std::string, double>& probabilityMap) const;
	const std::vector<Function>& getFunctions() const;

	std::vector<int> prepareFunctionIndexPool(const std::map<std::string, double>& functionProbabilities) const;
};