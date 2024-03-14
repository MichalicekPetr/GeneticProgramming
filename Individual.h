#pragma once

#include <vector>

#include "ConstantTable.h"
#include "Connection.h"
#include "Node.h"

using namespace std;

class Individual
{
private:
	Node* root;
	int nodeCnt;
	int maxDepth;
	ConstantTable constantTable;
	bool constantTableCreated;

	void addNodeToVectorRec(vector<Node*> & flattenTree, Node * currentNode) const;

	void fillLayersVectorRec(Node* current, const int& depth, vector<vector<string>>& layers, vector<int>& maxSizes) const;
	string addSpacesToElement(const string & originalElement, int elementSize) const;
	void addBranchLines(vector<string> & lines, const int & depth, const int & elementSize, const vector<bool> & emptyIndexes) const;
	string createBranchLineHorizontal(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	string createBranchLineVertical(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;

	void createConstantTableRec(Node* current);

public:
	Individual();
	Individual(const Individual& original);
	Individual(Node* root);
	Individual(Node* root, const int& nodeCnt, const int& maxDepth);
	~Individual();
	Node* pickRandomNode() const;
	Node* pickRandomLeaf() const;
	Node* pickRandomInnerNode() const;

	vector<Node*> createFlattenTree() const;

	static Individual generateRandomTreeGrowMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static Individual generateRandomTreeFullMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);

	static Node * generateRandomTreeGrowMethodNode(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static Node * generateRandomTreeFullMethodNode(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);

	static void generateRandomTreeFullMethodRec(Node * parrent, const int& depth, const int & maxDepth, const FunctionSet& functionSet, const TerminalSet& terminalSet);

	double evaluateTree(shared_ptr<Connection> & conn, string dbName, string tableName, const int & rowIdx) const;
	double evaluateTree(const map<string, double>& rowMap) const;
	void assingValueToDataPointsRec(Node * current, const int & depth, const map<string,double> & rowMap) const;

	friend std::ostream& operator<<(std::ostream& os, const Individual& individual);

	int getMaxDepth() const;
	int getNodeCnt() const;
	Node* getRoot() const;

	void setRoot(Node* newRoot);
	void setDepth(int depth);
	void setNodeCnt(int nodeCnt);

	void freeNodesRec(Node* current);
	void getTreeInfoRec(Node* current, int& nodeCntAcc, int& maxDepth, const int& depth);

	void createConstantTable();
	ConstantTable& getConstantTableRef();
	bool hasConstantTable() const;
	void resetConstantTable();

	Individual & operator=(const Individual & original);
};

