#pragma once

#include <string>

using namespace std;

class DataPoint
{
private:
	string colName;

public:
	DataPoint();
	DataPoint(string colName);
	DataPoint(const DataPoint& dataPoint);

	double getData(const int & rowID) const;
	string getName() const;
};

