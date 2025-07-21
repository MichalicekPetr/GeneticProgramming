#include <cmath>
#include <iomanip>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "Terminal.h"
#include "Random.h"

using namespace std;


Terminal::Terminal()
{
	this->value = 0;
	this->label = "Empty terminal";
	this->valuePtr = nullptr;
	this->constant = true;
	this->empty = true;
	this->link = false;
}

Terminal::Terminal(const double& value)
{
	this->value = value;

	stringstream stream;
	stream << fixed << setprecision(2) << value;
	this->label = stream.str();
	
	this->constant = true;
	this->empty = false;

	this->valuePtr = nullptr;
	this->link = false;
}

Terminal::Terminal(DataPoint dataPoint)
{
	this->value = 0;
	this->dataPoint = dataPoint;
	this->label = dataPoint.getName();
	this->constant = false;
	this->empty = false;

	this->valuePtr = nullptr;
	this->link = false;
}

Terminal::Terminal(const Terminal& terminal)
{
	this->value = terminal.value;
	this->dataPoint = DataPoint(terminal.dataPoint);
	this->label = terminal.label;
	this->constant = terminal.constant;
	this->empty = terminal.empty;

	this->valuePtr = terminal.valuePtr;
	this->link = terminal.link;
}


double Terminal::getValue() const
{
	if (this->link) {
		return *this->valuePtr;
	}
	else {
		return this->value;
	}
}

string Terminal::getLabel() const
{
	if (this->link) {
		return to_string(*this->valuePtr);
	}
	else {
		return this->label;
	}
}

bool Terminal::isDataPoint() const
{
	return !this->constant;
}

void Terminal::setValue(const map<string, double>& rowMap)
{
	string colName = this->dataPoint.getName();
	this->value = rowMap.at(colName);
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(0) << this->value;  // 2 desetinná místa
	this->label = colName + "(" + stream.str() + ")";
}

void Terminal::setConstantLink(ConstantTable& table)
{
	this->link = true;
	this->valuePtr = table.createEmptyElementAtEnd();
}

Terminal Terminal::inversion() const
{
	return Terminal(1/this->getValue());
}

Terminal Terminal::operator+(const Terminal& other) const
{
	return Terminal(this->getValue() + other.getValue());
}

Terminal Terminal::operator-() const
{
	return Terminal(-this->getValue());
}

Terminal Terminal::operator-(const Terminal& other) const
{
	return Terminal(this->getValue() - other.getValue());
}

Terminal Terminal::operator*(const Terminal& other) const
{
	return Terminal(this->getValue() * other.getValue());
}


Terminal Terminal::operator/(const Terminal& other) const
{
	return Terminal(this->getValue() / other.getValue());
}

std::ostream& operator<<(std::ostream& os, const Terminal& terminal)
{
	os << terminal.getLabel();
	return os;
}


TerminalSet::TerminalSet()
{
	this->constMin = 0.0;
	this->constMax = 0.0;
	this->realNumbers = false;
	this->colNames = vector<string>(0);
}

TerminalSet::TerminalSet(const double& constMin, const double& constMax, const bool& realNumbers, vector<string> colNames)
{
	this->constMin = constMin;
	this->constMax = constMax;
	this->realNumbers = realNumbers;
	this->colNames = colNames;
}

Terminal TerminalSet::getRandomTerminal(double constantProb) const
{
	double seed = Random::randProb();
	if (seed < constantProb) {
		// constant
		if (this->realNumbers) {
			return Terminal(Random::rand((size_t)constMin, (size_t)constMax));
		}
		else {
			return Terminal(Random::randInt(constMin, constMax));
		}
	}
	else {
		// data point
		int seed = Random::randInt(0, this->colNames.size() - 1);
		string colName = colNames.at(seed);
		DataPoint dataPoint = DataPoint(colName);
		return Terminal(dataPoint);
	}
}

double TerminalSet::getMax() const
{
	return this->constMax;
}

double TerminalSet::getMin() const
{
	return this->constMin;
}

bool TerminalSet::containsRealNumbers() const
{
	return this->realNumbers;
}

vector<string> TerminalSet::getColNames() const
{
	return this->colNames;
}
