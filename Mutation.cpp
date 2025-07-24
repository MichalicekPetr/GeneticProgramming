#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Individual.h"
#include "Mutation.h"
#include "Random.h"

using namespace std;

SubtreeMutation::SubtreeMutation() : SubtreeMutation(0.0, FunctionSet(), TerminalSet())
{
}

SubtreeMutation::SubtreeMutation(const double& mutationProb, const FunctionSet& funcSet, const TerminalSet& termSet)
{
	this->mutationProb = mutationProb;
	this->termSet = termSet;
	this->funcSet = funcSet;
}

void SubtreeMutation::mutate(Individual& individual, const int& maxDepth)
{
	double seed = Random::randProb();
	if (seed <= this->mutationProb) {
		int mutNodeIdx = individual.pickRandomNodeIdx();
		int mutNodeDepth = Individual::calculateDepthFromIdx(mutNodeIdx);
		int treeDepth = individual.getMaxDepth();

		int newDepthUpperbound = max(1, min(treeDepth + 3 - mutNodeDepth, maxDepth - mutNodeDepth));
		int newDepth = Random::randInt(1, newDepthUpperbound	);

		double seed2 = Random::randProb();
		Individual subTree = (seed2 <= 0.25)
			? Individual::generateRandomTreeFullMethod(newDepth, this->funcSet, this->termSet)
			: Individual::generateRandomTreeGrowMethod(newDepth, this->funcSet, this->termSet);

		individual.replaceNodeWithSubTree(subTree, mutNodeIdx, mutNodeDepth);
		individual.validateTreeStructure();
	}

	if (individual.getMaxDepth() > maxDepth) {
		cout << "individual.getMaxDepth(): " << individual.getMaxDepth() << " maxDepth: " << maxDepth << endl;
		throw invalid_argument("Depth is greater than max depth after subtree mutation");
		exit(1);
	}
	
}

void SubtreeMutation::setTerminalSet(TerminalSet termSet)
{
	this->termSet = termSet;
}

NodeReplacementMutation::NodeReplacementMutation() : NodeReplacementMutation(0.0, FunctionSet(), TerminalSet())
{
}

NodeReplacementMutation::NodeReplacementMutation(const double& mutationProb, const FunctionSet& funcSet, const TerminalSet& termSet)
{
	this->mutationProb = mutationProb;
	this->termSet = termSet;
	this->funcSet = funcSet;
}

void NodeReplacementMutation::setTerminalSet(TerminalSet termSet)
{
	this->termSet = termSet;
}

void NodeReplacementMutation::mutate(Individual& individual, const int & maxDepth)
{
	for (int i = 0; i <= individual.getLastNodeIdx(); i++) {
		Node* current = individual.getNodeAt(i);
		if (current != nullptr) {
			double seed = Random::randProb();
			if (seed <= this->mutationProb) {
				if (current->isFunctionNode()) {
					Function newFunc = this->funcSet.getRandomFunction();
					FunctionNode* fnode = dynamic_cast<FunctionNode*>(current);
					fnode->setFunc(newFunc);
				}
				else if (current->isTerminalNode()) {
					Terminal newTerm = this->termSet.getRandomTerminal();
					TerminalNode* tnode = dynamic_cast<TerminalNode*>(current);
					
					// One more constant or one less constant
					if (newTerm.isDataPoint() != tnode->getTerminalReference().isDataPoint())
						individual.resetConstantTable();

					tnode->setTerminal(newTerm);
				}
				else {
					throw invalid_argument("Node isnt terminal neither func");
				}
			}
		}	
	}
	individual.validateTreeStructure();	
}

CombinedMutation::CombinedMutation(const double& nodeReplacementMutationProb, const double & subTreeMutationProb, const FunctionSet& funcSet, const TerminalSet& termSet)
{
	this->mutationProb = mutationProb;
	this->termSet = termSet;
	this->funcSet = funcSet;
	this->nodeReplacementMutation = NodeReplacementMutation(nodeReplacementMutationProb, funcSet, termSet);
	this->subTreeMutation = SubtreeMutation(subTreeMutationProb, funcSet, termSet);
}

void CombinedMutation::mutate(Individual& individual, const int& maxDepth)
{
	this->subTreeMutation.mutate(individual, maxDepth);
	this->nodeReplacementMutation.mutate(individual, maxDepth);
}

void CombinedMutation::setTerminalSet(TerminalSet termSet)
{
	this->termSet = termSet;
	this->subTreeMutation.setTerminalSet(termSet);
	this->nodeReplacementMutation.setTerminalSet(termSet);
}
