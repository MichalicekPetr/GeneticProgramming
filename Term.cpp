#include <algorithm>
#include <ostream>

#include "Term.h"

Term::Term(const double& value)
{
	this->value = value;
	this->variables = vector<Variable>();
	this->createLabel();
}

Term::Term(const double& value, const vector<Variable>& variables)
{
	this->value = value;
	this->variables = variables;
	this->createLabel();
}

Term::Term(const double& value, string varName)
{
	this->value = value;
	this->variables = vector<Variable>();
	Variable var = Variable(1, varName);
	this->variables.push_back(var);
	this->createLabel();
}

void Term::createLabel()
{
	label = "";
	for (const auto& var : this->variables) {
		label += var.getLabel();
	}
	this->label = label; 
}

string Term::getLabel() const
{
	return this->label;
}

double Term::getValue() const
{
	return this->value;
}

vector<Variable> Term::getVariables() const
{
	return this->variables;
}

void Term::addNewVariable(string name, const double& power)
{
	if (power == 0) {
		return;
	}

	Variable newVar = Variable(power, name);
	const auto& iter = lower_bound(this->variables.begin(), this->variables.end(), newVar,
		[](const Variable& a, const Variable& b) {return a.getLabel().compare(b.getLabel()); });
	if (this->variables.size() > 0 && (iter - this->variables.begin()) < this->variables.size()) {
		if (this->variables.at(iter - this->variables.begin()).getName() == name) {
			throw invalid_argument("Variable with same name already exist in term");
		}
	}

	variables.insert(iter, newVar);
	this->createLabel();
}

void Term::addVariableAtEnd(string name, const double& power)
{
	if (power == 0) {
		return;
	}
	this->variables.push_back(Variable(power, name));
	this->createLabel();
}

Term Term::createReverseCopy() const
{
	return Term(-this->value, this->variables);
}

Term Term::createDiffValCopy(const double& newValue) const
{
	return Term(newValue, this->variables);
}

Term Term::createInverseCopy() const
{
	Term newTerm = Term(1 / this->value, this->variables);
	for (int i = 0; i < newTerm.size(); i++) {
		double power = newTerm.variables.at(i).getPower();
		newTerm.variables.at(i).setPower(-power);
	}
	return newTerm;
}

Term Term::createAbsCopy() const
{
	return Term(abs(this->value), this->variables);
}

void Term::setValue(const double& value)
{
	this->value = value;
	this->createLabel();
}

Variable Term::getVariable(const int& idx) const
{
	return this->variables.at(idx);
}

int Term::size() const
{
	return (int)(this->variables.size());
}

string Term::getSign() const
{
	return this->value >= 0 ? "+" : "-";
}

std::ostream& operator<<(std::ostream& os, const Term& term)
{
	if (term.value == 0) {
		os << "0";
		return os;
	}
	else if (term.value != 1) {
		os << term.value;
	}

	for (const auto& var : term.variables) {
		os << var.getName() << "^" << to_string(var.getPower());
	}	
	return os;
}
