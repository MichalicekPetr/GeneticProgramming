#pragma once

#include <array>

#include "ConstantTable.h"
#include "Connection.h"
#include "Node.h"

using namespace std;

class Individual
{
private:
	vector<unique_ptr<Node>> nodeVec;
	int nodeCnt;
	int reserved;
	int depth;
	int lastNodeIdx;
	ConstantTable constantTable;
	bool constantTableCreated;

	void addNodeToVectorRec(vector<Node*>& flattenTree, Node* currentNode) const;

	void fillLayersVectorRec(Node* current, const int& depth, vector<vector<string>>& layers, vector<int>& maxSizes) const;
	string addSpacesToElement(const string& originalElement, int elementSize) const;
	void addBranchLines(vector<string>& lines, const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	string createBranchLineHorizontal(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	string createBranchLineVertical(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;

	void createConstantTableRec(Node* current);

public:
	Individual();
	Individual(const Individual& original);
	Individual(const vector<int>& structure, const int& depth, const int& nodeCnt, const int& reserved, const int& lastNodeIdx, const FunctionSet& functionSet, const TerminalSet& terminalSet);

	Node* pickRandomNode() const;
	Node* pickRandomLeaf() const;
	Node* pickRandomInnerNode() const;

	static Individual generateRandomTreeGrowMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static Individual generateRandomTreeFullMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static int getParentIdx(const int& idx);
	static int getLeftChildIdx(const int& idx);

	double evaluate(shared_ptr<Connection>& conn, string dbName, string tableName, const int& rowIdx) const;
	double evaluate(const map<string, double>& rowMap) const;
	double evaluateRec(const int& idx) const;
	void assignValueToDataPoints(const map<string, double>& rowMap) const;


	friend std::ostream& operator<<(std::ostream& os, const Individual& individual);

	int getMaxDepth() const;
	int getNodeCnt() const;
	int getReservedCnt() const;
	int getLastNodeIdx() const;

	int calculateTakenSpace() const;
	

	void setRoot(Node* newRoot);
	void setDepth(int depth);
	void setNodeCnt(int nodeCnt);

	void getTreeInfoRec(Node* current, int& nodeCntAcc, int& maxDepth, const int& depth);

	void createConstantTable();
	ConstantTable& getConstantTableRef();
	bool hasConstantTable() const;
	void resetConstantTable();

	Individual& operator=(const Individual& original);

	bool isLeafAtIdx(const int& idx) const;
	bool isInnerNodeAtIdx(const int& idx) const;
};

