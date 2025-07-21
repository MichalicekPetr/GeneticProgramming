#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "ConstantTable.h"
#include "Term.h"
#include "Datapoint.h"

using namespace std;


class Terminal
{
private:
	double value;
	DataPoint dataPoint;
	string label;
	shared_ptr<double> valuePtr;

	bool constant;
	bool empty;
	bool link;

public:
	Terminal();
	Terminal(const double & value);
	Terminal(DataPoint dataPoint);
	Terminal(const Terminal& terminal);


	double getValue() const;
	string getLabel() const;
	bool isDataPoint() const;
	void setValue(const map<string, double>& rowMap);
	void setConstantLink(ConstantTable& table);

	Terminal inversion() const;
	Terminal operator+(const Terminal& other) const;
	Terminal operator-(const Terminal& other) const;
	Terminal operator*(const Terminal& other) const;
	Terminal operator/(const Terminal& other) const;
	Terminal operator-() const;

	friend std::ostream& operator<<(std::ostream& os, const Terminal& terminal);


};


class TerminalSet {
private:
	double constMin;
	double constMax;
	bool realNumbers;
	vector<string> colNames;

public:
	TerminalSet();
	TerminalSet(const double& constMin, const double& constMax, const bool& realNumbers, vector<string> colNames);
	Terminal getRandomTerminal(double constantProb = 0.5) const;

	double getMax() const;
	double getMin() const;
	bool containsRealNumbers() const;
	vector<string> getColNames() const;

};
