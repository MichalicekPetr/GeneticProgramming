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
		Node * point1, * point2;

		if (seed1 <= this->leafPickProb) {
			point1 = parent1.pickRandomLeaf();
		}
		else {
			point1 = parent1.pickRandomInnerNode();
		}
		if (seed2 <= this->leafPickProb) {
			point2 = parent2.pickRandomLeaf();
		}
		else {
			point2 = parent2.pickRandomInnerNode();
		}

		Node* subtree = point2->createTreeCopy();
		offspring = parent1.getRoot()->createTreeCopyWithReplacePoint(point1, subtree); 
		
		newI = Individual(offspring);
		if (newI.getMaxDepth() <= maxDepth)
			break;
	}
	return newI;
}
