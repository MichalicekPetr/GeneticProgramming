#include <vector>
#include <stdexcept>
#include <math.h>
#include <tuple>
#include <iostream>
#include <ostream>

#include "Individual.h"
#include "Node.h"
#include "Random.h"

using namespace std;

void Individual::addNodeToVectorRec(vector<Node*>& flattenTree, Node* currentNode) const
{

	if (currentNode == nullptr) {
		return;
	}
	flattenTree.push_back(currentNode);

	if (currentNode->getLeftOffspring() != nullptr) {
		this->addNodeToVectorRec(flattenTree, currentNode->getLeftOffspring());
	}

	if (currentNode->getRightOffspring() != nullptr) {
		this->addNodeToVectorRec(flattenTree, currentNode->getRightOffspring());
	}
}

void Individual::getTreeInfoRec(Node* current, int& nodeCntAcc, int& maxDepth, const int& depth)
{
	if (current == nullptr) {
		return;
	}
	else {
		if (depth > maxDepth) {
			maxDepth = depth;
		}
		nodeCntAcc++;
		this->getTreeInfoRec(current->getLeftOffspring(), nodeCntAcc, maxDepth, depth + 1);
		this->getTreeInfoRec(current->getRightOffspring(), nodeCntAcc, maxDepth, depth + 1);
		return;
	}
}

void Individual::createConstantTable()
{
	this->constantTable.reset();
	this->createConstantTableRec(this->root);
}

ConstantTable& Individual::getConstantTableRef()
{
	return this->constantTable;
}

bool Individual::hasConstantTable() const
{
	return this->constantTableCreated;
}

void Individual::resetConstantTable()
{
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

Individual& Individual::operator=(const Individual& original)
{
	this->root = original.getRoot()->createTreeCopy();
	int nodeCnt = 0, maxDepth = 0;
	this->getTreeInfoRec(this->root, nodeCnt, maxDepth, 1);
	this->nodeCnt = nodeCnt;
	this->maxDepth = maxDepth;
	this->constantTable = original.constantTable;
	this->constantTableCreated = original.constantTableCreated;
	return *this;
}

// Pøepsáno
bool Individual::isLeafAtIdx(const int& idx) const
{
	int child1idx = ((idx + 1) * 2) - 1;
	int child2idx = child1idx + 1;
	if ((this->lastNodeIdx + 1) < ((idx + 1) * 2))
		return TRUE;
	else if ((this->lastNodeIdx + 1) == ((idx + 1) * 2)) {
		if (this->nodeVec.at(child1idx) == nullptr)
			return TRUE;
		else
			return FALSE;
	}
	else {
		if ((this->nodeVec.at(child1idx) == nullptr) && ((this->nodeVec.at(child2idx) == nullptr)))
			return TRUE;
		else
			return FALSE;
	}
}

// Pøepsáno
bool Individual::isInnerNodeAtIdx(const int& idx) const
{
	return !this->isLeafAtIdx(idx);
}



// Pøepsáno
int Individual::getParentIdx(const int& idx)
{
	if (idx < 0) {
		cout << "Index nemùže být záporný" << endl;
		exit(1);
	}
	else if (idx == 0) {
		return -1;
	}
	else {
		return ((idx + 1) / 2) - 1;
	}
}

// Pøepsáno
int Individual::getLeftChildIdx(const int& idx)
{
	return ((idx + 1) * 2) - 1;
}

void Individual::fillLayersVectorRec(Node* current, const int& depth, vector<vector<string>>& layers, vector<int>& maxSizes) const
{
	string nodeStr;
	if (current == nullptr) {
		nodeStr = "";
	}
	else {
		nodeStr = current->toString();
	}

	if (maxSizes.at(depth) < (int)nodeStr.length()) {
		maxSizes.at(depth) = (int)nodeStr.length();
	}

	layers.at(depth).push_back(nodeStr);
	if (depth == this->maxDepth) {
		return;
	}
	else {
		if (current == nullptr) {
			this->fillLayersVectorRec(nullptr, depth + 1, layers, maxSizes);
			this->fillLayersVectorRec(nullptr, depth + 1, layers, maxSizes);
		}
		else {
			this->fillLayersVectorRec(current->getLeftOffspring(), depth + 1, layers, maxSizes);
			this->fillLayersVectorRec(current->getRightOffspring(), depth + 1, layers, maxSizes);
		}
	}
}

string Individual::addSpacesToElement(const string& originalElement, int elementSize) const
{
	string element;
	if (originalElement.size() > elementSize) {
		element = originalElement.substr(0, elementSize);
	}
	else {
		element = originalElement.c_str();
		int diff = (int)(elementSize - originalElement.size());
		if ((diff % 2) == 1) {
			diff -= 1;
			element += " ";
		}
		int spaceNum = diff / 2;
		element.insert(0, spaceNum, ' ');
		element.append(spaceNum, ' ');
	}
	return element;
}

void Individual::addBranchLines(vector<string>& lines, const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const
{
	int elementCnt = (int)pow(2, depth - 1);
	vector<bool> emptyIndexes2(0);
	for (int i = 0; i < elementCnt; i++) {
		bool a = emptyIndexes.at(i * 2);
		bool b = emptyIndexes.at((i * 2) + 1);
		if (a && b) {
			emptyIndexes2.push_back(true);
		}
		else {
			emptyIndexes2.push_back(false);
		}
	}

	string line1 = this->createBranchLineVertical(depth, elementSize, emptyIndexes);
	string line2 = this->createBranchLineHorizontal(depth, elementSize, emptyIndexes);
	string line3 = this->createBranchLineVertical(depth - 1, ((elementSize + 1) * 2) - 1, emptyIndexes2);

	lines.insert(lines.begin(), line1);
	lines.insert(lines.begin(), line2);
	lines.insert(lines.begin(), line3);

}

string Individual::createBranchLineVertical(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const
{
	string line = "";
	int firstPartLen = (int)((elementSize - 1) / 2);
	int lastPartLen = (int)((elementSize - 1) / 2);
	if (((elementSize - 1) % 2) == 1) {
		lastPartLen += 1;
	}
	line.append(firstPartLen, ' ');

	int elementCnt = (int)pow(2, depth) - 1;
	for (int i = 0; i <= elementCnt - 1; i++) {
		if (emptyIndexes.at(i)) {
			line += ' ';
		}
		else {
			line += '|';
		}
		line.append(elementSize, ' ');
	}

	int lastIdx = (int)(emptyIndexes.size() - 1);
	if (emptyIndexes.at(lastIdx)) {
		line += ' ';
	}
	else {
		line += '|';
	}

	line.append(lastPartLen, ' ');

	return line;
}

void Individual::createConstantTableRec(Node* current)
{
	if (current->isTerminalNode()) {
		TerminalNode* terminalNode = dynamic_cast<TerminalNode*>(current);
		if (terminalNode->isConstant()) {
			terminalNode->getTerminalReference().setConstantLink(this->constantTable);
		}
	}

	Node* left = current->getLeftOffspring();
	Node* right = current->getRightOffspring();

	if (left != nullptr) {
		this->createConstantTableRec(left);
	}

	if (right != nullptr) {
		this->createConstantTableRec(right);
	}

	return;
}

string Individual::createBranchLineHorizontal(const int& depth, const int& elementSize, const vector<bool>& emptyIndexes) const
{
	string line = "";
	int firstPartLen = (int)((elementSize - 1) / 2);
	int lastPartLen = (int)((elementSize - 1) / 2);
	if (((elementSize - 1) % 2) == 1) {
		lastPartLen += 1;
	}
	line.append(firstPartLen, ' ');
	int elementCnt = (int)(pow(2, depth) - 1);
	for (int i = 0; i <= elementCnt / 2; i++) {
		int idx1 = i * 2;
		int idx2 = (i * 2) + 1;
		if (emptyIndexes.at(idx1)) {
			line.append((elementSize / 2) + 1, ' ');
		}
		else {
			line.append((elementSize / 2) + 1, '-');
		}

		if (emptyIndexes.at(idx2)) {
			if ((elementSize % 2) == 1) {
				line.append((elementSize / 2) + 2, ' ');
			}
			else {
				line.append((elementSize / 2) + 1, ' ');
			}
		}
		else {
			if ((elementSize % 2) == 1) {
				line.append((elementSize / 2) + 2, '-');
			}
			else {
				line.append((elementSize / 2) + 1, '-');
			}
		}

		line.append(elementSize, ' ');
	}

	line.append(lastPartLen, ' ');
	return line;
}

// Pøepsané
Individual::Individual()
{
	this->nodeVec = vector<unique_ptr<Node>>(0);
	this->reserved = 0;
	this->nodeCnt = 0;
	this->depth = 0;
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
	this->lastNodeIdx = -1;
}

// Pøepsané
Individual::Individual(const Individual& original)
{
	lastNodeIdx = original.getLastNodeIdx();
	nodeVec.reserve(lastNodeIdx);

	for (int i = 0; i < lastNodeIdx; i++) {
		const Node* originalNode = original.nodeVec.at(i).get();
		if (originalNode) {
			nodeVec.push_back(originalNode->clone());  
		}
		else {
			nodeVec.push_back(nullptr);
		}
	}

	this->nodeCnt = original.nodeCnt;
	this->depth = original.depth;
	this->lastNodeIdx = lastNodeIdx - 1;
	this->reserved = lastNodeIdx;

	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

// Pøepsáno
Individual::Individual(const vector<int>& structure, const int& depth, const int& nodeCnt, const int& reserved, const int& lastNodeIdx, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	this->nodeCnt = nodeCnt;
	this->depth = depth;
	this->reserved = reserved;
	this->lastNodeIdx = lastNodeIdx;

	nodeVec.reserve(reserved);

	for (int i = 0; i <= lastNodeIdx; i++) {
		int nodeTypeIdx = structure.at(i);
		if (nodeTypeIdx == 0) {
			nodeVec.push_back(nullptr);
		}
		else if (nodeTypeIdx == 1) {
			nodeVec.push_back(std::unique_ptr<Node>(FunctionNode::createRandomFunctionNode(functionSet)));
		}
		else if (nodeTypeIdx == 2) {
			nodeVec.push_back(std::unique_ptr<Node>(TerminalNode::createRandomTerminalNode(terminalSet)));
		}
		else {
			std::cerr << "Nesprávná hodnota ve struktuøe na indexu " << i << "!" << std::endl;
			throw std::runtime_error("Chyba ve struktuøe stromu");
		}
	}
}

// Pøepsáno
Node* Individual::pickRandomNode() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}

	Node* pick = nullptr;
	while (pick == nullptr) {
		int seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			pick = nodeVec.at(seed).get();
			break;
		}
	}
	return pick;
}

// Pøepsáno
Node* Individual::pickRandomLeaf() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}

	Node* pick = nullptr;
	while (pick == nullptr) {
		int seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			if (this->isLeafAtIdx(seed)) {
				pick = nodeVec.at(seed).get();
				break;
			}
		}
	}
	return pick;
}

// Pøepsáno
Node* Individual::pickRandomInnerNode() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}
	if (this->nodeCnt == 1) {
		return nodeVec.at(0).get();
	}

	Node* pick = nullptr;
	while (pick == nullptr) {
		int seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			if (this->isInnerNodeAtIdx(seed)) {
				pick = nodeVec.at(seed).get();
				break;
			}
		}
	}
	return pick;
}

// Pøepsáno
Individual Individual::generateRandomTreeGrowMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
	}
	else {
		vector<int> idxPool;
		idxPool.push_back(0);
		int treeDepth = 0;
		int lastIdx = -1;

		// 0 = nullptr, 1 = innernode, 2 = leaf
		vector<int> structure = vector<int>(0);
		structure.reserve(pow(2, depth) - 1);
		for (int i = 0; i < (pow(2, depth) - 1); i++)
			structure.push_back(0);

		int nodeCnt = Random::randInt(1, (size_t)(pow(2, depth) - 1));
		int maxIdx = pow(2, depth) - 2;

		for (int i = 0; i < nodeCnt; i++) {
			int seed = Random::randInt(0, idxPool.size() - 1);
			int idx = idxPool.at(seed);
			idxPool.erase(idxPool.begin() + seed);

			int childIdx = ((idx + 1) * 2) - 1;
			if (childIdx <= maxIdx) {
				idxPool.push_back(childIdx);
				idxPool.push_back(childIdx + 1);
			}

			structure.at(idx) = 2;
			if (idx > 0) {
				int parentIdx = Individual::getParentIdx(idx);
				structure.at(parentIdx) = 1;
			}
			if ((idx + 1) > (pow(2, treeDepth) - 1))
				treeDepth++;
			if (idx > lastIdx)
				lastIdx = idx;
		}

		// Struktura je naplnìná, jde se tvoøit strom
		return Individual(structure, treeDepth, nodeCnt, pow(2, depth) - 1, lastIdx, functionSet, terminalSet);
	}
}

// Pøepsáno
Individual Individual::generateRandomTreeFullMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
		exit(1);
	}
	else {
		int nodeCnt = pow(2, depth) - 1;
		int lastLayerIdx = pow(2, depth - 1) - 1;
		int lastIdx = nodeCnt - 1;

		vector<int> structure = vector<int>(0);
		structure.reserve(pow(2, depth) - 1);
		for (int i = 0; i < lastLayerIdx; i++) {
			structure.push_back(1);
		}
		for (int i = lastLayerIdx; i < nodeCnt; i++) {
			structure.push_back(2);
		}

		return Individual(structure, depth, nodeCnt, nodeCnt, lastIdx, functionSet, terminalSet);
	}
}

// Pøepsáno
double Individual::evaluate(shared_ptr<Connection>& conn, string dbName, string tableName, const int& rowIdx) const
{
	map <string, double> rowMap = conn->getRow(dbName, tableName, rowIdx);
	return this->evaluate(rowMap);
}

// Pøepsáno
double Individual::evaluate(const map<string, double>& rowMap) const
{
	this->assignValueToDataPoints(rowMap);
	return this->evaluateRec(0);
	 
}

// Pøepsáno
double Individual::evaluateRec(const int& idx) const
{
	Node* current = this->nodeVec.at(idx).get();
	if (current->isTerminalNode()) {
		TerminalNode* node = dynamic_cast<TerminalNode*>(current);
		return node->getValue();
	}

	FunctionNode* node = dynamic_cast<FunctionNode*>(current);
	int leftChildIdx = Individual::getLeftChildIdx(idx);
	int rightChildIdx = Individual::getLeftChildIdx(idx) + 1;
	bool hasRightChild = TRUE; bool hasLeftChild = TRUE;

	if (leftChildIdx > this->lastNodeIdx)
		hasLeftChild = FALSE;
	else {
		if (this->nodeVec.at(leftChildIdx) == nullptr)
			hasLeftChild = FALSE;
	}
	if (rightChildIdx > this->lastNodeIdx)
		hasRightChild = FALSE;
	else {
		if (this->nodeVec.at(rightChildIdx) == nullptr)
			hasRightChild = FALSE;
	}

	if (!hasLeftChild && !hasRightChild)
		return nan("0");
	else if (!hasLeftChild && hasRightChild) {
		double right = evaluateRec(rightChildIdx);
		return node->evaluateFunction(0.0, right, false, true);
	}
	else if (hasLeftChild && !hasRightChild) {
		double left = evaluateRec(leftChildIdx);
		return node->evaluateFunction(left, 0.0, true, false);
	}
	else {
		double left = evaluateRec(leftChildIdx);
		double right = evaluateRec(rightChildIdx);
		return node->evaluateFunction(left, right, true, true);
	}
}

// Pøepsáno
void Individual::assignValueToDataPoints(const std::map<std::string, double>& rowMap) const
{
	for (int i = 0; i <= lastNodeIdx; i++) {
		if (!nodeVec.at(i)) continue;

		Node* current = nodeVec.at(i).get();
		if (current->isTerminalNode()) {
			if (TerminalNode* node = dynamic_cast<TerminalNode*>(current)) {
				Terminal& term = node->getTerminalReference();
				if (term.isDataPoint()) {
					term.setValue(rowMap);
				}
			}
		}
	}
}



// Pøepsáno
int Individual::getMaxDepth() const
{
	return this->depth;
}

// Pøepsáno
int Individual::getNodeCnt() const
{
	return this->nodeCnt;
}

// Pøepsáno
int Individual::getReservedCnt() const
{
	return this->reserved;
}

//Pøepsáno
int Individual::getLastNodeIdx() const
{
	return this->lastNodeIdx;
}


int Individual::calculateTakenSpace() const
{
	return pow(2, depth) - 1;
}


void Individual::setRoot(Node* newRoot)
{
	this->root = newRoot;
}

void Individual::setDepth(int depth)
{
	this->maxDepth = depth;
}

void Individual::setNodeCnt(int nodeCnt)
{
	this->nodeCnt = nodeCnt;
}

std::ostream& operator<<(std::ostream& os, const Individual& individual)
{
	vector<vector<string>> layers(individual.maxDepth);
	vector<int> maxSizes;
	int lineCnt = 0;

	for (int i = 0; i <= individual.maxDepth; i++) {
		layers.push_back(vector<string>(0));
		maxSizes.push_back(-1);
	}

	individual.fillLayersVectorRec(individual.root, 0, layers, maxSizes);

	int maxLineSize = (int)(pow(2, individual.maxDepth - 1) * (maxSizes.at(individual.maxDepth) + 1));
	int originElementSize = maxSizes.at(individual.maxDepth - 1);
	vector <string> lines;
	int elementSize = 0;

	for (int i = individual.maxDepth - 1; i >= 0; i--) {
		vector<bool> emptyIndexes;
		if (i == individual.maxDepth - 1) {
			elementSize = originElementSize;
		}
		else {
			elementSize = (elementSize * 2) + 1;
		}

		vector<string> elements = layers.at(i);
		string line = "";;
		int idx = -1;

		int idx11 = 0;
		for (const auto& element : elements) {
			if (element == "") {
				emptyIndexes.push_back(true);
			}
			else {
				emptyIndexes.push_back(false);
			}
			string elementResized = individual.addSpacesToElement(element, elementSize);
			line += elementResized + " ";
		}
		lines.insert(lines.begin(), line);
		lineCnt++;
		if (i > 0) {
			individual.addBranchLines(lines, i, elementSize, emptyIndexes);
			lineCnt = lineCnt + 3;
		}
	}

	for (int i = 0; i < lineCnt; i++) {
		os << i << ": " << lines[i] << endl;
	}

	return os;
}
