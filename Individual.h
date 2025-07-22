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

	void fillLayersVector(vector<vector<string>>& layers, vector<int>& maxSizes) const;
	string addSpacesToElement(const string& originalElement, int elementSize) const;
	void addBranchLines(vector<string>& lines, const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	string createBranchLineHorizontal(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	string createBranchLineVertical(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const;
	void eraseSubtree(const int& idx);
	void updateStats();
	void ensureFullBinaryStructure();


public:
	Individual();
	Individual(const Individual& original);
	Individual(const vector<int>& structure, const int& depth, const int& nodeCnt, const int& reserved, const int& lastNodeIdx, const FunctionSet& functionSet, const TerminalSet& terminalSet);

	Node* pickRandomNode() const;
	Node* pickRandomLeaf() const;
	Node* pickRandomInnerNode() const;
	int pickRandomNodeIdx() const;
	int pickRandomLeafIdx() const;
	int pickRandomInnerNodeIdx() const;

	static Individual generateRandomTreeGrowMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static Individual generateRandomTreeFullMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet);
	static Individual generateRandomTreePCT1(int maxDepth, double expectedSize, const FunctionSet& funcSet, const TerminalSet& termSet, const std::map<std::string, double>& probabilityMap);
	
	static int getParentIdx(const int& idx);
	static int getLeftChildIdx(const int& idx);
	static int calculateDepthFromIdx(const int& idx);
	static int calculateInsertIdx(const int& subtreeIdx, const int& replacePointIdx);

	double evaluate(shared_ptr<Connection>& conn, string dbName, string tableName, const int& rowIdx) const;
	double evaluate(const map<string, double>& rowMap) const;
	double evaluateRec(const int& idx) const;
	void assignValueToDataPoints(const map<string, double>& rowMap) const;

	friend std::ostream& operator<<(std::ostream& os, const Individual& individual);

	int getMaxDepth() const;
	int getNodeCnt() const;
	int getReservedCnt() const;
	int getLastNodeIdx() const;
	Node* getNodeAt(const int& idx) const;

	void setDepth(int depth);
	void setNodeCnt(int nodeCnt);
	void setNodeAt(int idx, unique_ptr<Node> newNode);

	void createConstantTable();
	ConstantTable& getConstantTableRef();
	bool hasConstantTable() const;
	void resetConstantTable();

	Individual& operator=(const Individual& original);

	bool isLeafAtIdx(const int& idx) const;
	bool isInnerNodeAtIdx(const int& idx) const;

	void replaceNodeWithSubTree(const Individual& subtree, const int& replacePointIdx, const int &replacePointDepth);
	Individual extractSubtree(const int& idx) const;
	
	void setNodeVec(vector<unique_ptr<Node>>&& newVec);
	int predictOffspringDepthAfterSubtreeReplace(int replaceIdx, int replaceNodeDepth, int subtreeDepth) const;

};