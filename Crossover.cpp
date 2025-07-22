#include <iostream>

#include "Crossover.h"
#include "Random.h"

using namespace std;

TwoPointCrossover::TwoPointCrossover() : TwoPointCrossover(0.1){}

TwoPointCrossover::TwoPointCrossover(double leafPickProb)
{
	this->leafPickProb = leafPickProb;
}

Individual TwoPointCrossover::createOffspring(const Individual& parent1, const Individual& parent2, const int& maxDepth) const
{
	while(true){
		//cout << "Create offspring start" << endl;
		//cout << "A" << endl;
		// 1. Vyber body pro køížení
		int idx1 = (Random::randProb() <= this->leafPickProb)
			? parent1.pickRandomLeafIdx()
			: parent1.pickRandomInnerNodeIdx();

		int idx2 = (Random::randProb() <= this->leafPickProb)
			? parent2.pickRandomLeafIdx()
			: parent2.pickRandomInnerNodeIdx();

		// 2. Spoèítej hloubky obou uzlù
		int depth1 = Individual::calculateDepthFromIdx(idx1);
		int depth2 = Individual::calculateDepthFromIdx(idx2);
		//cout << "parant1.depth: " << parent1.getMaxDepth() << " parant2.depth: " << parent2.getMaxDepth() << endl;
		//cout << "depth1: " << depth1 << ", depth2: " << depth2 << endl;

		// 3. Spoèítej výšku podstromu z parent2 (od idx2 dolù)
		int subtreeDepth = parent2.getMaxDepth() - depth2 + 1;

		// 4. Spoèítej oèekávanou hloubku offspringa

		int predictedDepth = parent1.predictOffspringDepthAfterSubtreeReplace(idx1, depth1, subtreeDepth);

		// 5. Pokud bude offspring validní, vytvoø ho

		
		if (predictedDepth <= maxDepth) {
			Individual subtree = parent2.extractSubtree(idx2);
			Individual offspring = Individual(parent1);
			offspring.replaceNodeWithSubTree(std::move(subtree), idx1, depth1);	
			return offspring;
		}
	}
}
