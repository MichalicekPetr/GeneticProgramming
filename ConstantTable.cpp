#include <iostream>

#include "ConstantTable.h"

using namespace std;

ConstantTable::ConstantTable()
{
	this->table = vector<shared_ptr<double>>(0);
}

void ConstantTable::reset()
{
	this->table = vector<shared_ptr<double>>(0);
}

shared_ptr<double> ConstantTable::createEmptyElementAtEnd()
{
	this->table.push_back(shared_ptr<double>(new double(0.0)));
	return table.at(table.size() - 1);
}

void ConstantTable::debugPrint() const
{
	cout << "Number of elements: " << this->table.size() << endl;
	for (const auto& x : this->table) {
		cout <<  *x << endl;
	}
}

void ConstantTable::setTable(const vector<double>& arr)
{
	for (int i = 0; i < this->table.size(); i++) {
		*this->table.at(i) = arr[i];
	}
}

int ConstantTable::getSize() const
{
	return this->table.size();
}

vector<double> ConstantTable::getTable() const
{
	vector<double> table(0);
	for (int i = 0; i < this->table.size(); i++) {
		table.push_back(*this->table.at(i));
	}
	return table;
}
