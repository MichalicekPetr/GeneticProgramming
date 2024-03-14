#pragma once

using namespace std;

#include <string>

#include "Population.h"

class Selection
{
	string name;

public:
	virtual Individual & selectIndividual(Population& population, const vector<double> & fitnessValues) const = 0;
};

class RouletteSelection : Selection {
	string name = "Roulette selection";

public:
	Individual & selectIndividual(Population& population, const vector<double> & fitnessValues) const override;
};

class TournamentSelection : public  Selection {
	string name = "Tournament selection";
	int tournamentSize;

public:
	TournamentSelection();
	TournamentSelection(const int & tournamentSize);
	Individual & selectIndividual(Population& population, const vector<double> & fitnessValues) const override;
};

