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

vector<Node*> Individual::createFlattenTree() const
{
	vector<Node*> treeFlatten(0);
	this->addNodeToVectorRec(treeFlatten, this->root);
	return treeFlatten;
}

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

void Individual::freeNodesRec(Node* current)
{
	if (current == nullptr) {
		return;
	}
	else {
		Node* left = current->getLeftOffspring();
		Node* right = current->getRightOffspring();
		free(current);
		freeNodesRec(left);
		freeNodesRec(right);
	}
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

Individual::Individual()
{
	this->root = nullptr;
	this->nodeCnt = 0;
	this->maxDepth = 0;
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

Individual::Individual(const Individual& original)
{
	if (original.getRoot() == nullptr) {
		this->root = nullptr;
		this->nodeCnt = 0;
		this->maxDepth = 0;
		this->constantTable = ConstantTable();
		this->constantTableCreated = false;
	}
	else {
		this->root = original.getRoot()->createTreeCopy();
		int nodeCnt = 0, maxDepth = 0;
		this->getTreeInfoRec(this->root, nodeCnt, maxDepth, 1);
		this->nodeCnt = nodeCnt;
		this->maxDepth = maxDepth;
		this->constantTable = original.constantTable;
		this->constantTableCreated = original.constantTableCreated;
	}

}

Individual::Individual(Node* root)
{
	this->root = root;
	int nodeCnt = 0, maxDepth = 0;
	this->getTreeInfoRec(this->root, nodeCnt, maxDepth, 1);
	this->nodeCnt = nodeCnt;
	this->maxDepth = maxDepth;
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

Individual::Individual(Node* root, const int& nodeCnt, const int& maxDepth)
{
	this->root = root;
	this->nodeCnt = nodeCnt;
	this->maxDepth = maxDepth;
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

Individual::~Individual()
{
	this->freeNodesRec(this->root);
}

Node* Individual::pickRandomNode() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}

	vector<Node*> treeFlatten = this->createFlattenTree();
	int seed = Random::randInt(0, this->nodeCnt - 1);
	return treeFlatten.at(seed);
}

Node* Individual::pickRandomLeaf() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}
	vector<Node*> treeFlatten = this->createFlattenTree();
	Node* leaf;

	while (true) {
		int seed = Random::randInt(0, this->nodeCnt - 1);
		if (treeFlatten.at(seed)->isLeaf()) {
			leaf = treeFlatten.at(seed);
			break;
		}
	}

	return leaf;
}

Node* Individual::pickRandomInnerNode() const
{
	if (this->nodeCnt == 0) {
		return nullptr;
	}
	if (this->nodeCnt == 1) {
		// Only 1 node, its root, no inner node then
		return this->root;
	}
	vector<Node*> treeFlatten = this->createFlattenTree();
	Node* innerNode;

	while (true) {
		int seed = Random::randInt(0, this->nodeCnt - 1);
		if (!treeFlatten.at(seed)->isLeaf()) {
			innerNode = treeFlatten.at(seed);
			break;
		}
	}
	return innerNode;
}

Individual Individual::generateRandomTreeGrowMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
	}
	else if (depth == 1) {
		return Individual(TerminalNode::createRandomTerminalNode(terminalSet));
	}
	else {
		vector <Node*> nodeVec;
		vector <NodeDirection> dirVec;
		vector <int> depthVec;

		Node* root = FunctionNode::createRandomFunctionNode(functionSet);

		nodeVec.push_back(root); dirVec.push_back(NodeDirection::Left); depthVec.push_back(2);
		nodeVec.push_back(root); dirVec.push_back(NodeDirection::Right); depthVec.push_back(2);
		int nodeCnt = Random::randInt(depth, (size_t)(pow(2, depth) - 2));
		int maxReachedDepth = 1;

		for (int i = 0; i < nodeCnt; i++) {
			int seed = Random::randInt(0, nodeVec.size() - 1);
			Node* current = nodeVec.at(seed);
			NodeDirection dir = dirVec.at(seed);
			int currentDepth = depthVec.at(seed);
			if (currentDepth > maxReachedDepth) {
				maxReachedDepth = currentDepth;
			}

			nodeVec.erase(nodeVec.begin() + seed);
			dirVec.erase(dirVec.begin() + seed);
			depthVec.erase(depthVec.begin() + seed);

			Node* newNode;
			if (currentDepth < depth) {
				newNode = FunctionNode::createRandomFunctionNode(functionSet);
				nodeVec.push_back(newNode); dirVec.push_back(NodeDirection::Left); depthVec.push_back(currentDepth + 1);
				nodeVec.push_back(newNode); dirVec.push_back(NodeDirection::Right); depthVec.push_back(currentDepth + 1);
			}
			else {
				newNode = TerminalNode::createRandomTerminalNode(terminalSet);
			}
			newNode->createParentLink(current, dir);
		}
		return Individual(root, nodeCnt, maxReachedDepth);
	}

}

Individual Individual::generateRandomTreeFullMethod(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
	}
	else if (depth == 1) {
		return Individual(TerminalNode::createRandomTerminalNode(terminalSet));
	}
	else {
		Node* root = FunctionNode::createRandomFunctionNode(functionSet);
		generateRandomTreeFullMethodRec(root, 2, depth, functionSet, terminalSet);
		return Individual(root, (int)(pow(depth, 2) - 1), depth);
	}
}

Node* Individual::generateRandomTreeGrowMethodNode(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
	}
	else if (depth == 1) {
		return TerminalNode::createRandomTerminalNode(terminalSet);
	}
	else {
		vector <Node*> nodeVec;
		vector <NodeDirection> dirVec;
		vector <int> depthVec;

		Node* root = FunctionNode::createRandomFunctionNode(functionSet);

		nodeVec.push_back(root); dirVec.push_back(NodeDirection::Left); depthVec.push_back(2);
		nodeVec.push_back(root); dirVec.push_back(NodeDirection::Right); depthVec.push_back(2);
		int nodeCnt = Random::randInt(depth, (size_t)(pow(2, depth) - 2));
		int maxReachedDepth = 1;

		for (int i = 0; i < nodeCnt; i++) {
			int seed = Random::randInt(0, nodeVec.size() - 1);
			Node* current = nodeVec.at(seed);
			NodeDirection dir = dirVec.at(seed);
			int currentDepth = depthVec.at(seed);
			if (currentDepth > maxReachedDepth) {
				maxReachedDepth = currentDepth;
			}

			nodeVec.erase(nodeVec.begin() + seed);
			dirVec.erase(dirVec.begin() + seed);
			depthVec.erase(depthVec.begin() + seed);

			Node* newNode;
			if (currentDepth < depth) {
				newNode = FunctionNode::createRandomFunctionNode(functionSet);
				nodeVec.push_back(newNode); dirVec.push_back(NodeDirection::Left); depthVec.push_back(currentDepth + 1);
				nodeVec.push_back(newNode); dirVec.push_back(NodeDirection::Right); depthVec.push_back(currentDepth + 1);
			}
			else {
				newNode = TerminalNode::createRandomTerminalNode(terminalSet);
			}
			newNode->createParentLink(current, dir);
		}
		return root;
	}
}

Node* Individual::generateRandomTreeFullMethodNode(const int& depth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth < 1) {
		throw invalid_argument("Depth has to be greater than 0");
	}
	else if (depth == 1) {
		return TerminalNode::createRandomTerminalNode(terminalSet);
	}
	else {
		Node* root = FunctionNode::createRandomFunctionNode(functionSet);
		generateRandomTreeFullMethodRec(root, 2, depth, functionSet, terminalSet);
		return root;
	}
}

void Individual::generateRandomTreeFullMethodRec(Node* parrent, const int& depth, const int& maxDepth, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	if (depth > maxDepth) {
		return;
	}
	Node* left, * right;
	if (depth == maxDepth) {
		left = TerminalNode::createRandomTerminalNode(terminalSet);
		right = TerminalNode::createRandomTerminalNode(terminalSet);
	}
	else {
		left = FunctionNode::createRandomFunctionNode(functionSet);
		right = FunctionNode::createRandomFunctionNode(functionSet);
	}

	left->createParentLink(parrent, NodeDirection::Left);
	right->createParentLink(parrent, NodeDirection::Right);



	generateRandomTreeFullMethodRec(left, depth + 1, maxDepth, functionSet, terminalSet);
	generateRandomTreeFullMethodRec(right, depth + 1, maxDepth, functionSet, terminalSet);
}

double Individual::evaluateTree(shared_ptr<Connection>& conn, string dbName, string tableName, const int& rowIdx) const
{
	map <string, double> rowMap = conn->getRow(dbName, tableName, rowIdx);
	this->assingValueToDataPointsRec(this->root, 1, rowMap);
	double result;
	if (this->root->isFunctionNode()) {
		result = dynamic_cast<FunctionNode*>(this->root)->evaulateNodeRec();
	}
	else {
		return dynamic_cast<TerminalNode*>(this->root)->getValue();
	}

	return result;
}

double Individual::evaluateTree(const map<string, double>& rowMap) const
{
	this->assingValueToDataPointsRec(this->root, 1, rowMap);
	double result;
	if (this->root->isFunctionNode()) {
		result = dynamic_cast<FunctionNode*>(this->root)->evaulateNodeRec();
	}
	else {
		return dynamic_cast<TerminalNode*>(this->root)->getValue();
	}
	return result;
}

void Individual::assingValueToDataPointsRec(Node* current, const int& depth, const map<string, double>& rowMap) const
{
	if (depth == maxDepth) {
		if (current->isTerminalNode()) {
			TerminalNode* node = dynamic_cast<TerminalNode*>(current);
			Terminal& term = node->getTerminalReference();
			if (term.isDataPoint()) {
				term.setValue(rowMap);
			}
		}
	}
	else {
		if (current->getLeftOffspring() != nullptr) {
			this->assingValueToDataPointsRec(current->getLeftOffspring(), depth + 1, rowMap);
		}
		if (current->getRightOffspring() != nullptr) {
			this->assingValueToDataPointsRec(current->getRightOffspring(), depth + 1, rowMap);
		}
	}
}

int Individual::getMaxDepth() const
{
	return this->maxDepth;
}

int Individual::getNodeCnt() const
{
	return this->nodeCnt;
}

Node* Individual::getRoot() const
{
	return this->root;
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
