#pragma once

#include <string>

#include "Individual.h"

using namespace std;

class Crossover
{
	string name;

public:
	virtual Individual createOffspring(const Individual & parent1, const Individual& parent2, const int & maxDepth) const = 0;
};

class TwoPointCrossover : public Crossover {
private:
	string name = "Two point crossover";
	double leafPickProb;
	double subtreeLeafPickProb; 
	double parentLeafPickProb ;

public:
	TwoPointCrossover();
	TwoPointCrossover(const double& leafPickProb, const double& subtreeLeafPickProb, const double& parentLeafPickProbb);
	Individual createOffspring(const Individual& parent1, const Individual& parent2, const int& maxDepth) const override;
};







