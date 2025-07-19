#include <iostream>

#include "Crossover.h"
#include "Random.h"

using namespace std;

TwoPointCrossover::TwoPointCrossover() : TwoPointCrossover(0.1){}

TwoPointCrossover::TwoPointCrossover(double leafPickProb)
{
	this->leafPickProb = leafPickProb;
}

Individual TwoPointCrossover::createOffspring(const Individual& parent1, const Individual& parent2, const int & maxDepth) const
{
	Node* offspring = nullptr;
	Individual newI;
	while(TRUE){
		double seed1 = Random::randProb();
		double seed2 = Random::randProb();
		int idx1, idx2;

		if (seed1 <= this->leafPickProb) {
			idx1 = parent1.pickRandomLeafIdx();
		}
		else {
			idx1 = parent1.pickRandomInnerNodeIdx();
		}
		if (seed2 <= this->leafPickProb) {
			idx2 = parent2.pickRandomLeafIdx();
		}
		else {
			idx2 = parent2.pickRandomInnerNodeIdx();
		}

		Node* subtree = point2->createTreeCopy();
		offspring = parent1.getRoot()->createTreeCopyWithReplacePoint(point1, subtree); 
		
		newI = Individual(offspring);
		if (newI.getMaxDepth() <= maxDepth)
			break;
	}
	return newI;
}
