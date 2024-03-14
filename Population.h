#pragma once

#include "Individual.h"

#include <memory>
#include <vector>

using namespace std;

class PopulationInitMethod {
public:
	virtual vector<Individual> initPopulation(const int& depth, const FunctionSet& funcSet, const TerminalSet& termSet, const int& populationSize) = 0;
};

class RandomHalfFullHalfGrowInitialization : public PopulationInitMethod {
public:
	vector<Individual> initPopulation(const int& depth, const FunctionSet& funcSet, const TerminalSet& termSet, const int& populationSize) override;
};

class Population
{
private:
	vector<Individual> individuals;
	int size;
	unique_ptr<PopulationInitMethod> populationInitMethod;
	
public:
	Population();
	Population(const int& size, unique_ptr<PopulationInitMethod> populationInitMethod);
	
	void initPopulation(const int& depth, FunctionSet funcSet, TerminalSet termSet); 
	void setPopulation(vector<Individual> newPopulation);

	int getSize() const;

	Individual & at(const int& idx);
};

