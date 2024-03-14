#pragma once

#include <string>

#include "Individual.h"
#include "Node.h"

using namespace std;

class Mutation
{
	string name;

public:
	virtual void mutate(Individual & individual) = 0;
	virtual void setTerminalSet(TerminalSet termSet) = 0;
};


class SubtreeMutation : public Mutation
{
private:
	string name = "Sub tree mutation";
	double mutationProb;
	FunctionSet funcSet;
	TerminalSet termSet;

public:
	SubtreeMutation();
	SubtreeMutation(const double& mutationProb, const FunctionSet& funcSet, const TerminalSet& termSet);
	void mutate(Individual & individual) override;
	void setTerminalSet(TerminalSet termSet) override;
};


class NodeReplacementMutation : public Mutation
{
private:
	string name = "Node replacement mutation";
	double mutationProb;
	FunctionSet funcSet;
	TerminalSet termSet;

public:
	NodeReplacementMutation();
	NodeReplacementMutation(const double & mutationProb, const FunctionSet & funcSet, const TerminalSet & termSet);
	void setTerminalSet(TerminalSet termSet) override;
	void mutate(Individual & individual) override;
};


class CombinedMutation : public Mutation {
	string name = "Combined mutation";
	double mutationProb;
	FunctionSet funcSet;
	TerminalSet termSet;
	SubtreeMutation subTreeMutation;
	NodeReplacementMutation nodeReplacementMutation;

public:
	CombinedMutation(const double& nodeReplacementMutationProb, const double& subTreeMutationProb, const FunctionSet& funcSet, const TerminalSet& termSet);
	void mutate(Individual& individual) override;
	void setTerminalSet(TerminalSet termSet) override;
};