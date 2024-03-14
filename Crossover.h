#pragma once

#include <string>

#include "Individual.h"

using namespace std;

class Crossover
{
	string name;

public:
	virtual Individual createOffspring(const Individual & parent1, const Individual& parent2) const = 0;
};

class TwoPointCrossover : public Crossover {
private:
	string name = "Two point crossover";
	double leafPickProb;

public:
	TwoPointCrossover();
	TwoPointCrossover(double leafPickProb);
	Individual createOffspring(const Individual& parent1, const Individual& parent2) const override;
};







