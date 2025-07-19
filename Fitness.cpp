#include <iostream>
#include <limits>

#include "Fitness.h"
#include "Individual.h"

using namespace std;

double ClassicFitnessFunction::evaluate(const Individual& individual, shared_ptr<Connection> & conn, string dbName, string tableName, string targetVarName, string primaryKeyName) const
{
	vector<pair<int,double>> targetVarNames = conn->getTargetVarValues(targetVarName, primaryKeyName, tableName);
	return this->evaluate(individual, conn, dbName, tableName, targetVarNames);
}

double ClassicFitnessFunction::evaluate(const Individual& individual, shared_ptr<Connection> & conn, string dbName, string tableName, const vector<pair<int, double>>& targetVarValues) const
{
	double acc = 0.0;
	int rowCnt = 0;

	for (const auto& x : targetVarValues) {
		rowCnt += 1;
		int rowIdx = x.first;
		double target = x.second;
		double result = individual.evaluate(conn, dbName, tableName, rowIdx);
		acc += abs(target - result);
	}

	double score = ( - (acc / rowCnt)) * (1 + (individual.getMaxDepth() * 0.01));
	if (isnan(score)) {
		return - numeric_limits<double>::infinity();
	}

	return score;
}

double ClassicFitnessFunction::evaluate(const Individual& individual, shared_ptr<map<int, map<string, double>>> dbMapPtr, const vector<pair<int, double>>& targetVarValues) const
{
	double acc = 0.0;
	int rowCnt = 0;

	for (const auto& x : targetVarValues) {
 		rowCnt += 1;
		int rowIdx = x.first;
		double target = x.second;
		double result = individual.evaluate(dbMapPtr->at(rowIdx));
		acc += abs(target - result);
	}

	double score = (-(acc / rowCnt)) * (1 + (individual.getMaxDepth() * 0.01));
	if (isnan(score)) {
		return -numeric_limits<double>::infinity();
	}

	return score;
}



