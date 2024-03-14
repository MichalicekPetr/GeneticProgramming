#include <stdexcept>
#include <iostream>

#include "Variable.h"

using namespace std;

Variable::Variable(const double& power, string name)
{
	if (name.size() == 0) {
		throw invalid_argument("You trying to create variable with no name");
	}

	this->power = power;
	this->name = name;
	this->createLabel();
}

double Variable::getPower() const
{
	return this->power;
}

string Variable::getName() const
{
	return this->name;
}

string Variable::getLabel() const
{
	return this->label;
}

void Variable::createLabel()
{
	string label = "";
	label += this->name;
	label += to_string(this->power);
	this->label = label;
}

void Variable::setPower(const double& newPower)
{
	this->power = newPower;
}
