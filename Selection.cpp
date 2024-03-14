#include <iostream>

#include "Random.h"
#include "Selection.h"

using namespace std;

TournamentSelection::TournamentSelection() : TournamentSelection(1)
{
}

TournamentSelection::TournamentSelection(const int& tournamentSize)
{
	this->tournamentSize = tournamentSize;
}

Individual & TournamentSelection::selectIndividual(Population& population, const vector<double>& fitnessValues) const
{
	vector<int> seeds = Random::randInts(0, population.getSize() - 1, this->tournamentSize);
	int chosenIdx = -1;
	double maxFitness = 0;
	bool fitnessSet = false;
	
	for (const auto& idx : seeds) {
		if (!fitnessSet) {
			chosenIdx = idx;
			maxFitness = fitnessValues.at(idx);
			fitnessSet = true;
		}
		else {
			double currentFitness = fitnessValues.at(idx);
			if(currentFitness > maxFitness){
				chosenIdx = idx;
				maxFitness = currentFitness;
			}
		}
	}

	return population.at(chosenIdx);
}
