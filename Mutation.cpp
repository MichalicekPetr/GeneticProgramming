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

void SubtreeMutation::mutate(Individual& individual, const int & maxDepth)
{
	double seed = Random::randProb();
	if (seed <= this->mutationProb) {
		Node* point = individual.pickRandomNode();
		int nodeCnt = 0;
		int depth = 0;
		individual.getTreeInfoRec(point, nodeCnt, depth, 1);
		int newDepthUpperbound = min(ceil(maxDepth + 2), maxDepth);

		int newDepth = Random::randInt(1, newDepthUpperbound);

		Node* subTree;
		double seed2 = Random::randProb();
		if (seed2 <= 0.25) {
			subTree = Individual::generateRandomTreeFullMethodNode(newDepth, this->funcSet, this->termSet);
		}
		else {
			subTree = Individual::generateRandomTreeGrowMethodNode(newDepth, this->funcSet, this->termSet);
		}

		Node* parent = point->getParent();
		if (parent == nullptr) {
			Node* oldRoot = individual.getRoot();
			individual.setRoot(subTree);
			individual.freeNodesRec(oldRoot);
		}
		else {
			NodeDirection dir = point->getDirection();
			subTree->createParentLink(parent, dir);
			individual.freeNodesRec(point);
		}

		nodeCnt = 0;
		depth = 0;
		individual.getTreeInfoRec(individual.getRoot(), nodeCnt, depth, 1);
		individual.setDepth(depth);
		individual.setNodeCnt(nodeCnt);
		individual.resetConstantTable();
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
	int nodeCnt = individual.getNodeCnt();
	for (int i = 0; i < nodeCnt; i++) {
		double seed = Random::randProb();
		bool flattenTreeCreated = false;
		vector<Node*> flattenTree = vector<Node*>(0);

		if (seed <= this->mutationProb) {
			if (!flattenTreeCreated) {
				flattenTree = individual.createFlattenTree();
			}
			Node * node = flattenTree.at(i);

			if (node->isFunctionNode()) {
				Function newFunc = this->funcSet.getRandomFunction();
				FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
				fnode->setFunc(newFunc);
			}
			else if (node->isTerminalNode()) {
				Terminal newTerm = this->termSet.getRandomTerminal();
				TerminalNode* tnode = dynamic_cast<TerminalNode*>(node);
				tnode->setTerminal(newTerm);
			}
			else {
				throw invalid_argument("Node isnt terminal neither func");
			}
			individual.resetConstantTable();
		}
	}
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
