#include <iostream>

#include "Population.h"
#include "Individual.h"
#include <vector>

using namespace std;


vector<Individual> RandomHalfFullHalfGrowInitialization::initPopulation(const int& depth, const FunctionSet& funcSet, const TerminalSet& termSet, const int & populationSize)
{
	bool fullTree = false;
	vector<Individual> individuals(0);

	for (int i = 0; i < populationSize; i++) {
		if (fullTree) {
			individuals.push_back(move(Individual::generateRandomTreeFullMethod(depth, funcSet, termSet)));
		}
		else {
			individuals.push_back(move(Individual::generateRandomTreeGrowMethod(depth, funcSet, termSet)));
		}
		fullTree = !fullTree;
		cout << &individuals.at(i) << endl;
	}

	cout << "Last indexes " << endl;
	for (auto x : individuals) {
		cout << x.getLastNodeIdx() << endl;
	}
	return individuals;
}

Population::Population() : Population(0, unique_ptr<PopulationInitMethod>(nullptr))
{
}

Population::Population(const int& size, unique_ptr<PopulationInitMethod> populationInitMethod)
{
	this->size = size;
	this->populationInitMethod = move(populationInitMethod);
	this->individuals = vector<Individual>(0);
}

void Population::initPopulation(const int & depth, FunctionSet funcSet, TerminalSet termSet)
{
	this->individuals = this->populationInitMethod->initPopulation(depth, funcSet, termSet, this->size);
}

void Population::setPopulation(vector<Individual> newPopulation)
{
	this->individuals = move(newPopulation);
}

int Population::getSize() const
{
	return this->size;
}

Individual & Population::at(const int & idx)
{
	return this->individuals.at(idx);
}
