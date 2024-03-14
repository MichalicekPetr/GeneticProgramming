#pragma once

#include <vector>
#include <memory>

using namespace std;

class ConstantTable
{
private:
	vector<shared_ptr<double>> table;

public:
	ConstantTable();
	
	void reset();
	shared_ptr<double> createEmptyElementAtEnd();
	void debugPrint() const;
	void setTable(const vector<double>& arr);
	int getSize() const;
	vector<double> getTable() const;
};

