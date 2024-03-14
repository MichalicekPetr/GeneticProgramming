#pragma once
#include "Connection.h"
#include "Individual.h"

class FitnessFunction
{
public:
	virtual double evaluate(const Individual & individual, shared_ptr<Connection> & conn, string dbName, string tableName, string targetVarName, string primaryKeyName) const = 0;
	virtual double evaluate(const Individual& individual, shared_ptr<Connection> & conn, string dbName, string tableName, const vector<pair<int, double>> & targetVarValues) const = 0;
	virtual double evaluate(const Individual& individual, shared_ptr<map<int ,map<string, double>>> dbMapPtr, const vector<pair<int, double>>& targetVarValues) const = 0;
};

class ClassicFitnessFunction : public FitnessFunction {
public:
	double evaluate(const Individual& individual, shared_ptr<Connection> & conn, string dbName, string tableName, string targetVarName, string primaryKeyName) const override;
	double evaluate(const Individual& individual, shared_ptr<Connection> & conn, string dbName, string tableName, const vector<pair<int, double>>& targetVarValues) const override;
	double evaluate(const Individual& individual, shared_ptr<map<int, map<string, double>>> dbMapPtr, const vector<pair<int, double>>& targetVarValues) const override;
};