#include <iostream>
#include <vector>

#include "Function.h"
#include "Node.h"
#include "Random.h"

using namespace std;


Function::Function()
{
	this->name = "Undefined function";
	this->parity = -1;
	this->func = [](const Terminal& x, const Terminal& y) { return 0; };
}

Function::Function(string name, int parity, function < double (const double& a, const double& b) > func)
{
	this->name = name;
	this->parity = parity;
	this->func = func;
}

Function::Function(const Function& function)
{
	this->name = function.name;
	this->parity = function.parity;
	this->func = function.func;
}

string Function::getName() const
{
	return this->name;
}

int Function::getParity() const
{
	return this->parity;
}

double Function::evaluate(const double& a, const double& b) const
{
	return this->func(a,b);
}

FunctionSet::FunctionSet(const int& size)
{
	this->functions = vector<Function>(size);
}

void FunctionSet::addFunction(const Function& func)
{
	this->functions.push_back(func);
}

FunctionSet FunctionSet::createArithmeticFunctionSet()
{
	FunctionSet functionSet = FunctionSet();
	
	functionSet.addFunction(Function("+", 2, [](const double &x, const double& y) { return x + y; }));
	functionSet.addFunction(Function("-", 2, [](const double& x, const double& y) { return x - y; }));
	functionSet.addFunction(Function("*", 2, [](const double& x, const double& y) { return x * y; }));
	functionSet.addFunction(Function("%", 2, [](const double& x, const double& y) { return x / y; }));
	functionSet.addFunction(Function("neg", 1, [](const double& x, const double& y) { return -x; }));
	functionSet.addFunction(Function("inv", 1, [](const double& x, const double& y) { return 1/x; }));

	return functionSet;
}

FunctionSet FunctionSet::createArithmeticFunctionSetNoDivide()
{
	FunctionSet functionSet = FunctionSet(4);

	functionSet.addFunction(Function("+", 2, [](const double& x, const double& y) { return x + y; }));
	functionSet.addFunction(Function("-", 2, [](const double& x, const double& y) { return x - y; }));
	functionSet.addFunction(Function("x", 2, [](const double& x, const double& y) { return x * y; }));
	functionSet.addFunction(Function("neg", 1, [](const double& x, const double& y) { return -x; }));

	return functionSet;
}

Function FunctionSet::getRandomFunction() const
{
	const size_t & seed = Random::randInt(0, this->functions.size() - 1);
	return this->functions.at(seed);
}

Function FunctionSet::getRandomFunction(const std::map<std::string, double>& probabilityMap) const {
	// Vytvoøíme vektor kumulativních pravdìpodobností
	std::vector<double> cumulative;
	std::vector<size_t> indices;
	double total = 0.0;


	for (size_t i = 0; i < functions.size(); ++i) {
		const Function& f = functions[i];
		std::map<std::string, double>::const_iterator it = probabilityMap.find(f.getName());
		if (it == probabilityMap.end()) {
			throw std::invalid_argument("Missing probability for function: " + f.getName());
		}

		double p = it->second;
		if (p < 0.0) {
			throw std::invalid_argument("Negative probability for function: " + f.getName());
		}

		total += p;
		cumulative.push_back(total);   // mezisouèet
		indices.push_back(i);          // odpovídající index ve functions
	}

	if (total == 0.0) {
		throw std::runtime_error("Total function probability is zero.");
	}

	if (std::abs(total - 1.0) > 1e-6) {
		throw std::runtime_error("Function probabilities must sum to 1.0");
	}

	// Náhodná hodnota v rozsahu <0, total)
	double r = Random::randProb();

	// Najdi první index, kde kumulativní pravdìpodobnost > r
	for (size_t i = 0; i < cumulative.size(); ++i) {
		if (r < cumulative[i]) {
			return functions[indices[i]];
		}
	}

	// Pokud nic nenajdeme (kvùli zaokrouhlení), vra poslední
	return functions[indices.back()];
}

std::vector<int> FunctionSet::prepareFunctionIndexPool(const std::map<std::string, double>& functionProbabilities) const {
	std::vector<int> indexPool;
	for (int i = 0; i < functions.size(); ++i) {
		const Function& func = functions[i];
		auto it = functionProbabilities.find(func.getName());
		if (it == functionProbabilities.end()) {
			continue; // nebo výchozí pravdìpodobnost 0
		}
		double probability = it->second;
		int repeat = static_cast<int>(probability * 100); // škálujeme pro diskrétní rozložení
		for (int j = 0; j < repeat; ++j) {
			indexPool.push_back(i);
		}
	}
	if (indexPool.empty()) {
		throw std::invalid_argument("Function probability pool is empty. Check your probability map.");
	}
	return indexPool;
}

const std::vector<Function>& FunctionSet::getFunctions() const {
	return functions;
}

bool operator==(const Function& lhs, const Function& rhs)
{
	return lhs.getName() == rhs.getName() &&
		lhs.getParity() == rhs.getParity();
}

bool operator==(const Function lhs, const Function rhs)
{
	return false;
}
