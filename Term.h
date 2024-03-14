#pragma once

#include <string>
#include <vector>

#include "Variable.h"

using namespace std;

class Term
{
private:
	double value;
	vector<Variable> variables;
	string label;

public:
	Term(const double & value);
	Term(const double & value, const vector<Variable> & variables);
	Term(const double& value, string varName);

	void createLabel();
	string getLabel() const;

	double getValue() const;
	void setValue(const double& value);

	Variable getVariable(const int& idx) const;
	vector<Variable> getVariables() const;
	void addNewVariable(string name, const double& power);
	void addVariableAtEnd(string name, const double& power);

	Term createReverseCopy() const;
	Term createDiffValCopy(const double & newValue) const;
	Term createInverseCopy() const;
	Term createAbsCopy() const;

	int size() const;

	friend std::ostream& operator<<(std::ostream& os, const Term& term);
	string getSign() const;
};

