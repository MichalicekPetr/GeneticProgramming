#include <vector>
#include <stdexcept>
#include <math.h>
#include <tuple>
#include <iostream>
#include <ostream>
#include <queue>
#include <map>
#include <string>

#include "Individual.h"
#include "Node.h"
#include "Random.h"
#include "Function.h"

using namespace std;

// Přepsáno
void Individual::fillLayersVector(vector<vector<string>>& layers, vector<int>& maxSizes) const
{
	Node* current;
	int currentDepth = 1;
	int remaining = 1;
	bool newDepth = true;
	for (int i = 0; i <= this->lastNodeIdx; i++) {
		newDepth = false;
		if (this->nodeVec.at(i) == nullptr)
			current = nullptr;
		else
			current = this->nodeVec.at(i).get();

		string nodeStr;
		if (current == nullptr) {
			nodeStr = "";
		}
		else {
			nodeStr = current->toString();
		}

		if (maxSizes.at(currentDepth - 1) < (int)nodeStr.length()) {
			maxSizes.at(currentDepth - 1) = (int)nodeStr.length();
		}
		layers.at(currentDepth - 1).push_back(nodeStr);

		remaining--;
		if (remaining == 0) {
			currentDepth++;
			remaining = pow(2, currentDepth - 1);
			newDepth = true;
		}

	}

	if (!newDepth) {
		for (int i = 0; i < remaining; i++) {
			layers.at(currentDepth - 1).push_back("");
		}
	}
}

// Přepsáno
void Individual::createConstantTable()
{
	this->constantTable.reset();

	for (int i = 0; i <= this->lastNodeIdx; i++) {
		if (this->nodeVec.at(i) != nullptr) {
			Node* current = this->nodeVec.at(i).get();
			if (current->isTerminalNode()) {
				TerminalNode* terminalNode = dynamic_cast<TerminalNode*>(current);
				if (terminalNode->isConstant()) {
					terminalNode->getTerminalReference().setConstantLink(this->constantTable);
				}
			}
		}
	}

	this->constantTableCreated = true;
}

// Přepsáno
ConstantTable& Individual::getConstantTableRef()
{
	return this->constantTable;
}

// Přepsáno 
bool Individual::hasConstantTable() const
{
	return this->constantTableCreated;
}

// Přepsáno
void Individual::resetConstantTable()
{
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
}

// Přepsáno
Individual& Individual::operator=(const Individual& original)
{
	// 1. Ochrana proti samopřiřazení (kopírování sebe sama)
	if (this == &original) return *this;

	// 2. Vyčištění stávajícího obsahu
	nodeVec.clear();

	// 3. Zkopírování uzlů (deep copy přes clone)
	lastNodeIdx = original.getLastNodeIdx();
	nodeVec.reserve(lastNodeIdx);

	for (int i = 0; i <= lastNodeIdx; i++) {
		const Node* originalNode = original.nodeVec.at(i).get();
		if (originalNode) {
			nodeVec.push_back(originalNode->clone());  // clone() → unique_ptr<Node>
		}
		else {
			nodeVec.push_back(nullptr);
		}
	}

	// 4. Zkopírování ostatních atributů
	nodeCnt = original.nodeCnt;
	depth = original.depth;
	reserved = lastNodeIdx;
	this->lastNodeIdx = lastNodeIdx;

	constantTable = ConstantTable();   // nebo original.constantTable pokud je to žádané
	constantTableCreated = false;

	this->dagMapCreated = false;
	this->dagLinks = map<int, int>();

	return *this;
}

// Přepsáno
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

// Přepsáno
bool Individual::isInnerNodeAtIdx(const int& idx) const
{
	return !this->isLeafAtIdx(idx);
}

void Individual::replaceNodeWithSubTree(const Individual& subtree, const int& replacePointIdx, const int& replacePointDepth)
{
	this->eraseSubtree(replacePointIdx);

	int newDepth = max(replacePointDepth + subtree.getMaxDepth() - 1, this->depth);
	if (newDepth > this->depth) {
		this->reserved = pow(2, newDepth) - 1;
		this->nodeVec.reserve(this->reserved); // rezervace kapacity
	}

	for (int i = 0; i < subtree.nodeVec.size(); i++) {
		if (subtree.nodeVec.at(i) != nullptr) {
			int insertIdx = Individual::calculateInsertIdx(i, replacePointIdx);

			if (insertIdx >= this->nodeVec.size()) {
				int diff = insertIdx - this->nodeVec.size() + 1;
				for (int j = 0; j < diff; ++j) {
					this->nodeVec.push_back(nullptr);
				}
			}

			Node* original = subtree.nodeVec.at(i).get();
			this->nodeVec.at(insertIdx) = std::unique_ptr<Node>(original->clone());
		}
	}

	this->updateStats();
	this->resetConstantTable();
	this->ensureFullBinaryStructure();
}

void Individual::ensureFullBinaryStructure()
{
	int requiredSize = (1 << this->depth) - 1; // ekvivalent pow(2, depth) - 1

	if (static_cast<int>(nodeVec.size()) < requiredSize) {
		for (int i = static_cast<int>(nodeVec.size()); i < requiredSize; ++i) {
			nodeVec.push_back(nullptr);
		}
	}

	this->reserved = requiredSize; // ✅ aktualizuj reserved
}



// Přepsáno
int Individual::getParentIdx(const int& idx)
{
	if (idx < 0) {
		cout << "Index nemůže být záporný" << endl;
		exit(1);
	}
	else if (idx == 0) {
		return -1;
	}
	else {
		return ((idx + 1) / 2) - 1;
	}
}

// Přepsáno
int Individual::getLeftChildIdx(const int& idx)
{
	return ((idx + 1) * 2) - 1;
}

// Přepsáno
int Individual::calculateDepthFromIdx(const int& idx)
{
	return static_cast<int>(floor(log2(idx + 1))) + 1;
}

// Přepsáno
int Individual::calculateInsertIdx(const int& subtreeIdx, const int& replacePointIdx)
{
    if (subtreeIdx == 0)
        return replacePointIdx;

    int parentSubtreeIdx = (subtreeIdx - 1) / 2;
    int parentInsertIdx = calculateInsertIdx(parentSubtreeIdx, replacePointIdx);

    if (subtreeIdx == 2 * parentSubtreeIdx + 1) {
        // Levý potomek
        return 2 * parentInsertIdx + 1;
    } else {
        // Pravý potomek
        return 2 * parentInsertIdx + 2;
    }
}

// Přepsáno
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

// Přepsáno
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

// Přepsáno
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

// Přepsáno
void Individual::eraseSubtree(const int& idx)
{
	if (idx < 0 || idx > lastNodeIdx || !nodeVec.at(idx)) return;

	vector<int> idxQueue;
	idxQueue.push_back(idx);

	while (!idxQueue.empty()) {
		int currentIdx = idxQueue.back();
		idxQueue.pop_back();

		int leftChildIdx = getLeftChildIdx(currentIdx);
		int rightChildIdx = leftChildIdx + 1;

		if (leftChildIdx <= lastNodeIdx && nodeVec.at(leftChildIdx)) {
			idxQueue.push_back(leftChildIdx);
		}
		if (rightChildIdx <= lastNodeIdx && nodeVec.at(rightChildIdx)) {
			idxQueue.push_back(rightChildIdx);
		}

		nodeVec.at(currentIdx) = nullptr;
	}
}

// Přepsáno
void Individual::updateStats()
{
	int nodeCntAcc = 0;
	int lastIdx = -1;

	for (size_t i = 0; i < nodeVec.size(); i++) {
		if (nodeVec[i]) {
			nodeCntAcc++;
			lastIdx = static_cast<int>(i);
		}
	}

	this->lastNodeIdx = lastIdx;
	this->nodeCnt = nodeCntAcc;
	this->depth = (lastIdx >= 0) ? this->calculateDepthFromIdx(lastIdx) : 0;
}

// Přepsáno
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

// Přepsané
Individual::Individual()
{
	this->nodeVec = vector<unique_ptr<Node>>(0);
	this->reserved = 0;
	this->nodeCnt = 0;
	this->depth = 0;
	this->constantTable = ConstantTable();
	this->constantTableCreated = false;
	this->lastNodeIdx = -1;
	this->dagMapCreated = false;
	this->dagLinks = map<int, int>();
}

// Přepsané
Individual::Individual(const Individual& original)
{
	lastNodeIdx = original.getLastNodeIdx();
	nodeVec.reserve(lastNodeIdx);

	for (int i = 0; i <= lastNodeIdx; i++) {
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
	this->reserved = lastNodeIdx;

	this->constantTable = ConstantTable();
	this->constantTableCreated = false;

	if (original.dagMapCreated) {
		this->dagMapCreated = true;
		this->dagLinks = original.dagLinks;
	}
	else {
		this->dagMapCreated = false;
		this->dagLinks = map<int, int>();
	}
}

// Přepsáno
Individual::Individual(const vector<int>& structure, const int& depth, const int& nodeCnt, const int& reserved, const int& lastNodeIdx, const FunctionSet& functionSet, const TerminalSet& terminalSet)
{
	this->nodeCnt = nodeCnt;
	this->depth = depth;
	this->reserved = reserved;
	this->lastNodeIdx = lastNodeIdx;
	this->constantTableCreated = false;
	this->constantTable = ConstantTable();

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
			std::cerr << "Nesprávná hodnota ve struktuře na indexu " << i << "!" << std::endl;
			throw std::runtime_error("Chyba ve struktuře stromu");
		}
	}

	this->dagMapCreated = false;
	this->dagLinks = map<int, int>();
}

// Přepsáno
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

// Přepsáno
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

// Přepsáno
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

// Přepsáno
int Individual::pickRandomNodeIdx() const
{
	if (this->nodeCnt == 0) {
		return -1;
	}

	int seed = -1;
	while (true) {
		seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			break;
		}
	}
	return seed;
}

// Přepsáno
int Individual::pickRandomLeafIdx() const
{
	if (this->nodeCnt == 0) {
		return -1;
	}

	int seed = -1;
	while (true) {
		seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			if (this->isLeafAtIdx(seed)) {
				break;
			}
		}
	}
	return seed;
}

// Přepsáno
int Individual::pickRandomInnerNodeIdx() const
{
	if (this->nodeCnt == 0) {
		return -1;
	}
	if (this->nodeCnt == 1) {
		return 0;
	}

	int seed = -1;
	while (true) {
		seed = Random::randInt(0, this->lastNodeIdx);
		if (this->nodeVec.at(seed) != nullptr) {
			if (this->isInnerNodeAtIdx(seed)) {
				break;
			}
		}
	}
	return seed;
}

// Přepsáno
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

		// Struktura je naplněná, jde se tvořit strom
		return Individual(structure, treeDepth, nodeCnt, pow(2, depth) - 1, lastIdx, functionSet, terminalSet);
	}
}

// Přepsáno
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

Individual Individual::generateRandomTreePCT1(int maxDepth, double expectedSize, const FunctionSet& funcSet, const TerminalSet& termSet, const std::map<std::string, double>& probabilityMap)
{
	if (maxDepth < 1 || expectedSize < 1.0) {
		throw std::invalid_argument("maxDepth and expectedSize must be positive.");
	}

	// Přepočet pravděpodobnosti výběru funkce
	const auto& functions = funcSet.getFunctions();
	double b = 0.0;
	std::vector<double> funcProbs;

	for (const auto& func : functions) {
		double prob = probabilityMap.at(func.getName());
		funcProbs.push_back(prob);
		b += prob * func.getParity();
	}

	double p = 1.0 - (1.0 / (expectedSize * b));
	if (p < 0.0) p = 0.0;
	else if (p > 1.0) p = 1.0;

	std::vector<std::unique_ptr<Node>> nodeVec;
	std::function<void(int, int)> ptc1Rec;

	ptc1Rec = [&](int idx, int depth) {
		if (idx >= static_cast<int>(nodeVec.size()))
			nodeVec.resize(idx + 1);

		if (depth >= maxDepth) {
			Terminal term = termSet.getRandomTerminal();
			nodeVec[idx] = std::make_unique<TerminalNode>(term);
			return;
		}

		if (Random::randProb() <= p) {
			Function f = funcSet.getRandomFunction(probabilityMap);
			nodeVec[idx] = std::make_unique<FunctionNode>(f);

			bool leftDone = false;
			bool rightDone = false;

			int leftIdx = 2 * idx + 1;
			int rightIdx = 2 * idx + 2;

			if (Random::randProb() <= 0.5) {
				ptc1Rec(leftIdx, depth + 1);
				leftDone = true;
			}
			if (Random::randProb() <= 0.5) {
				ptc1Rec(rightIdx, depth + 1);
				rightDone = true;
			}

			// Zaručíme, že alespoň jeden potomek bude vytvořen
			if (!leftDone && !rightDone) {
				if (Random::randProb() <= 0.5) {
					ptc1Rec(leftIdx, depth + 1);
				}
				else {
					ptc1Rec(rightIdx, depth + 1);
				}
			}
		}
		else {
			Terminal term = termSet.getRandomTerminal();
			nodeVec[idx] = std::make_unique<TerminalNode>(term);
		}
		};

	ptc1Rec(0, 1);

	Individual result;
	result.setNodeVec(std::move(nodeVec));
	result.updateStats();
	return result;
}

Individual Individual::generateRandomTreePCT2(int maxDepth, vector<double>& sizeDistribution, const FunctionSet& funcSet, const TerminalSet& termSet, const map<string, double>& probabilityMap)
{

	// 1. Vyber požadovanou velikost stromu podle distribuce
	double r = Random::randProb();
	int targetSize = 1;
	double cumulative = 0.0;
	for (int i = 0; i < sizeDistribution.size(); i++) {
		cumulative += sizeDistribution[i];
		if (r <= cumulative) {
			targetSize = i + 1;
			break;
		}
	}

	// 2. Počáteční kořen
	vector<unique_ptr<Node>> nodeVec;
	queue<pair<int, int> > boundary; // {index, depth}
	int nodeCount = 0;

	Function rootFunc = funcSet.getRandomFunction(probabilityMap);
	nodeVec.push_back(make_unique<FunctionNode>(rootFunc));
	nodeCount++;

	// Zadej boundary pozice pro děti kořene
	for (int i = 0; i < rootFunc.getParity(); i++) {
		int childIdx = 2 * 0 + 1 + i;
		boundary.push(make_pair(childIdx, 2));
	}

	// 3. Dokud lze přidávat funkce (v závislosti na velikosti a hloubce)
	while (!boundary.empty() && nodeCount < targetSize) {
		pair<int, int> front = boundary.front();
		boundary.pop();
		int idx = front.first;
		int depth = front.second;

		if (depth >= maxDepth) {
			Terminal t = termSet.getRandomTerminal();
			if (idx >= (int)nodeVec.size()) nodeVec.resize(idx + 1);
			nodeVec[idx] = make_unique<TerminalNode>(t);
			continue;
		}

		if (nodeCount + 1 >= targetSize) break;

		Function f = funcSet.getRandomFunction(probabilityMap);
		if (idx >= (int)nodeVec.size()) nodeVec.resize(idx + 1);
		nodeVec[idx] = make_unique<FunctionNode>(f);
		nodeCount++;

		for (int i = 0; i < f.getParity(); i++) {
			int childIdx = 2 * idx + 1 + i;
			boundary.push(make_pair(childIdx, depth + 1));
		}
	}

	// 4. Zbytek boundary vyplň terminály
	while (!boundary.empty()) {
		pair<int, int> front = boundary.front();
		boundary.pop();
		int idx = front.first;
		int depth = front.second;

		Terminal t = termSet.getRandomTerminal();
		if (idx >= (int)nodeVec.size()) nodeVec.resize(idx + 1);
		nodeVec[idx] = make_unique<TerminalNode>(t);
	}

	Individual result;
	result.setNodeVec(move(nodeVec));
	result.updateStats();
	return result;
}

// Přepsáno
double Individual::evaluate(shared_ptr<Connection>& conn, string dbName, string tableName, const int& rowIdx) const
{
	map <string, double> rowMap = conn->getRow(dbName, tableName, rowIdx);
	return this->evaluate(rowMap);
}

// Přepsáno
double Individual::evaluate(const map<string, double>& rowMap) const
{
	this->assignValueToDataPoints(rowMap);
	if (this->dagMapCreated) {
		std::map<int, double> dagCache;
		return this->evaluateRecDAG(0, dagCache);
	}
	else{
		return this->evaluateRec(0);
	}
}

// Přepsáno
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



// Přepsáno
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


double Individual::evaluateRecDAG(const int& idx, std::map<int, double>& dagCache) const
{
	// Pokud je uzel sdílený, použij výsledek z cache
	auto it = dagLinks.find(idx);
	if (it != dagLinks.end() && it->second != -1) {
		int sharedIdx = it->second;
		auto cacheIt = dagCache.find(sharedIdx);
		if (cacheIt != dagCache.end()) {
			//std::cout << "[DAG DEBUG] Cache hit na indexu " << idx << " (sdílený s " << sharedIdx << "), hodnota: " << cacheIt->second << std::endl;
			//std::cout << "[DAG DEBUG] Aktuální Individual:\n" << *this << std::endl;
			//std::cout << "Pokračujte stisknutím libovolné klávesy..." << std::endl;
			//std::cin.get();
			return cacheIt->second;
		}
		// Pokud není v cache, spočítej a ulož
		double val = evaluateRecDAG(sharedIdx, dagCache);
		dagCache[sharedIdx] = val;
		return val;
	}

	Node* current = this->nodeVec.at(idx).get();
	if (current->isTerminalNode()) {
		TerminalNode* node = dynamic_cast<TerminalNode*>(current);
		double val = node->getValue();
		dagCache[idx] = val;
		return val;
	}

	FunctionNode* node = dynamic_cast<FunctionNode*>(current);
	int leftChildIdx = Individual::getLeftChildIdx(idx);
	int rightChildIdx = leftChildIdx + 1;
	bool hasRightChild = TRUE, hasLeftChild = TRUE;

	if (leftChildIdx > this->lastNodeIdx || this->nodeVec.at(leftChildIdx) == nullptr)
		hasLeftChild = FALSE;
	if (rightChildIdx > this->lastNodeIdx || this->nodeVec.at(rightChildIdx) == nullptr)
		hasRightChild = FALSE;

	double result;
	if (!hasLeftChild && !hasRightChild)
		result = nan("0");
	else if (!hasLeftChild && hasRightChild)
		result = node->evaluateFunction(0.0, evaluateRecDAG(rightChildIdx, dagCache), false, true);
	else if (hasLeftChild && !hasRightChild)
		result = node->evaluateFunction(evaluateRecDAG(leftChildIdx, dagCache), 0.0, true, false);
	else
		result = node->evaluateFunction(evaluateRecDAG(leftChildIdx, dagCache), evaluateRecDAG(rightChildIdx, dagCache), true, true);

	dagCache[idx] = result;
	return result;
}
// Přepsáno
int Individual::getMaxDepth() const
{
	return this->depth;
}

// Přepsáno
int Individual::getNodeCnt() const
{
	return this->nodeCnt;
}

// Přepsáno
int Individual::getReservedCnt() const
{
	return this->reserved;
}

//Přepsáno
int Individual::getLastNodeIdx() const
{
	return this->lastNodeIdx;
}

// Přepsáno
void Individual::setDepth(int depth)
{
	this->depth = depth;
}

// Přepsáno
void Individual::setNodeCnt(int nodeCnt)
{
	this->nodeCnt = nodeCnt;
}

void Individual::setNodeAt(int idx, unique_ptr<Node> newNode)
{
	if (idx < 0) return;

	if (idx >= static_cast<int>(nodeVec.size())) {
		for (size_t i = nodeVec.size(); i <= idx; ++i) {
			nodeVec.push_back(nullptr);
		}
	}

	nodeVec.at(idx) = std::move(newNode);
}

// Přepsáno
std::ostream& operator<<(std::ostream& os, const Individual& individual)
{
	if (individual.nodeCnt == 0) {
		os << "Empty tree" << endl;
		return os;
	}

	vector<vector<string>> layers;
	vector<int> maxSizes;
	int lineCnt = 0;

	for (int i = 0; i <= individual.depth; i++) {
		layers.push_back(vector<string>(0));
		maxSizes.push_back(-1);
	}

	individual.fillLayersVector(layers, maxSizes);
	int maxLineSize = (int)(pow(2, individual.depth - 1) * (maxSizes.at(individual.depth - 1) + 1));
	int originElementSize = maxSizes.at(individual.depth - 1);
	vector <string> lines;
	int elementSize = 0;

	for (int i = individual.depth - 1; i >= 0; i--) {
		vector<bool> emptyIndexes;

		if (i == individual.depth - 1) {
			elementSize = originElementSize;
		}
		else {
			elementSize = (elementSize * 2) + 1;
		}

		vector<string> elements = layers.at(i);
		string line = "";

		for (const auto& element : elements) {
			if (element.empty()) {
				emptyIndexes.push_back(true);
			}
			else {
				emptyIndexes.push_back(false);
			}
			string elementResized = individual.addSpacesToElement(element, elementSize);
			line += elementResized + " ";
		}

		// DOPLNĚNÍ na požadovanou velikost vrstvy
		int expectedSize = pow(2, i);
		while ((int)emptyIndexes.size() < expectedSize) {
			emptyIndexes.push_back(true);
			line += individual.addSpacesToElement("", elementSize) + " ";
		}

		lines.insert(lines.begin(), line);
		lineCnt++;

		if (i > 0) {
			individual.addBranchLines(lines, i, elementSize, emptyIndexes);
			lineCnt += 3;
		}
	}

	for (int i = 0; i < lineCnt; i++) {
		os << i << ": " << lines[i] << endl;
	}
	return os;
}

bool operator==(const Individual& lhs, const Individual& rhs)
{
	for (int i = 0; i <= lhs.lastNodeIdx; ++i) {
		const Node* lptr = lhs.nodeVec[i] ? lhs.nodeVec[i].get() : nullptr;
		const Node* rptr = rhs.nodeVec[i] ? rhs.nodeVec[i].get() : nullptr;

		if (!lptr && !rptr)
			continue;
		if (!lptr || !rptr)
			return false;

		// Porovnání typu uzlu
		if (lptr->isFunctionNode() && rptr->isFunctionNode()) {
			const FunctionNode* lf = dynamic_cast<const FunctionNode*>(lptr);
			const FunctionNode* rf = dynamic_cast<const FunctionNode*>(rptr);
			if (!(lf->getFunction() == rf->getFunction()))
				return false;
		}
		else if (lptr->isTerminalNode() && rptr->isTerminalNode()) {
			const TerminalNode* lt = dynamic_cast<const TerminalNode*>(lptr);
			const TerminalNode* rt = dynamic_cast<const TerminalNode*>(rptr);
			if (!(lt->getTerminalConst() == rt->getTerminalConst()))
				return false;
		}
		else {
			// Jeden je funkce, druhý terminál, nebo jiný typ
			return false;
		}
	}
	return true;
}

// Přepsáno
Node* Individual::getNodeAt(const int& idx) const
{
	if (idx < 0 || idx >= static_cast<int>(nodeVec.size())) {
		return nullptr;
	}
	if (!nodeVec.at(idx)) {
		return nullptr; // ochrana proti nullptr ve vektoru
	}
	return nodeVec.at(idx).get();
}

Individual Individual::extractSubtree(const int& idx) const
{
	if (idx < 0 || idx >= static_cast<int>(nodeVec.size()) || nodeVec.at(idx) == nullptr) {
		cout << "nodeVec.size(): " << nodeVec.size() << " idx: " << idx << endl;
		throw std::invalid_argument("Invalid subtree root index.");
	}

	vector<unique_ptr<Node>> newNodeVec;
	queue<pair<int, int>> toVisit;  // pair: <original_idx, new_idx>
	map<int, int> indexMap;         // maps old indices → new indices

	toVisit.push({ idx, 0 });
	newNodeVec.resize(1);

	while (!toVisit.empty()) {
		pair<int, int> front = toVisit.front();
		int oldIdx = front.first;
		int newIdx = front.second;
		toVisit.pop();

		if (!nodeVec.at(oldIdx)) {
			newNodeVec[newIdx] = nullptr;
			continue;
		}

		newNodeVec[newIdx] = nodeVec.at(oldIdx)->clone();

		// Levý potomek
		int oldLeft = getLeftChildIdx(oldIdx);
		if (oldLeft <= lastNodeIdx && oldLeft < static_cast<int>(nodeVec.size()) && nodeVec.at(oldLeft)) {
			int newLeft = getLeftChildIdx(newIdx);
			if (newLeft >= static_cast<int>(newNodeVec.size()))
				newNodeVec.resize(newLeft + 1);

			toVisit.push({ oldLeft, newLeft });
		}

		// Pravý potomek
		int oldRight = getLeftChildIdx(oldIdx) + 1;
		if (oldRight <= lastNodeIdx && oldRight < static_cast<int>(nodeVec.size()) && nodeVec.at(oldRight)) {
			int newRight = getLeftChildIdx(newIdx) + 1;
			if (newRight >= static_cast<int>(newNodeVec.size()))
				newNodeVec.resize(newRight + 1);

			toVisit.push({ oldRight, newRight });
		}
	}

	// Odvoď parametry pro konstruktor
	int newLastIdx = static_cast<int>(newNodeVec.size()) - 1;
	int newNodeCnt = 0;
	for (auto& ptr : newNodeVec)
		if (ptr) newNodeCnt++;

	int newDepth = calculateDepthFromIdx(newLastIdx);
	int newReserved = static_cast<int>(newNodeVec.size());

	// Sestav nový Individual pomocí interního konstruktoru
	Individual result;
	result.nodeVec = std::move(newNodeVec);
	result.nodeCnt = newNodeCnt;
	result.depth = newDepth;
	result.lastNodeIdx = newLastIdx;
	result.reserved = newReserved;
	result.constantTableCreated = false;

	return result;
}

void Individual::setNodeVec(std::vector<std::unique_ptr<Node>>&& newVec)
{
	this->nodeVec = std::move(newVec); // nutné pro přesun vlastnictví
}

int Individual::predictOffspringDepthAfterSubtreeReplace(int replaceIdx, int replaceNodeDepth, int subtreeDepth) const
{
	// 1. Urči indexy všech uzlů v podstromu, který bude odstraněn
	std::vector<bool> marked(this->nodeVec.size(), false);
	std::queue<int> toVisit;
	toVisit.push(replaceIdx);
	marked[replaceIdx] = true;

	while (!toVisit.empty()) {
		int idx = toVisit.front();
		toVisit.pop();

		if (idx >= (int)this->nodeVec.size() || this->nodeVec[idx] == nullptr)
			continue;

		int left = Individual::getLeftChildIdx(idx);
		int right = Individual::getLeftChildIdx(idx) + 1;

		if (left < (int)this->nodeVec.size()) {
			toVisit.push(left);
			marked[left] = true;
		}

		if (right < (int)this->nodeVec.size()) {
			toVisit.push(right);
			marked[right] = true;
		}
	}

	// 2. Projdi zbývající uzly a zjisti jejich maximální hloubku
	int maxDepthOutside = 0;
	for (int i = 0; i < (int)this->nodeVec.size(); i++) {
		if (!marked[i] && this->nodeVec[i] != nullptr) {
			int depth = Individual::calculateDepthFromIdx(i);
			if (depth > maxDepthOutside) {
				maxDepthOutside = depth;
			}
		}
	}

	// 3. Spočítej hloubku nové větve po náhradě
	int newSubtreeDepth = replaceNodeDepth + subtreeDepth - 1;

	// 4. Výsledek je maximum
	return max(maxDepthOutside, newSubtreeDepth);
}

void Individual::optimizeSelf(const bool& mergeConstants, const bool& removeUselessBranches, const bool& DAG)
{
	if (mergeConstants) {
		this->mergeConstants();
	}
	if (removeUselessBranches) {
		this->removeUselessBranches();
	}
	if (DAG) {
		this->createDAG();
	}
}

void Individual::mergeConstants()
{
	for (int i = 0; i <= this->lastNodeIdx; ++i) {
		Node* node = this->nodeVec.at(i).get();
		if (!node || !node->isFunctionNode())
			continue;

		int leftIdx = Individual::getLeftChildIdx(i);
		int rightIdx = leftIdx + 1;

		// Ověření existence potomků
		if (leftIdx > this->lastNodeIdx || rightIdx > this->lastNodeIdx)
			continue;
		Node* leftNode = this->nodeVec.at(leftIdx).get();
		Node* rightNode = this->nodeVec.at(rightIdx).get();
		if (!leftNode || !rightNode)
			continue;

		// Ověření, že oba potomci jsou konstantní TerminalNode
		TerminalNode* leftTerm = dynamic_cast<TerminalNode*>(leftNode);
		TerminalNode* rightTerm = dynamic_cast<TerminalNode*>(rightNode);
		if (!leftTerm || !rightTerm)
			continue;
		if (!leftTerm->isConstant() || !rightTerm->isConstant())
			continue;

		// Spočítání výsledku funkce
		FunctionNode* funcNode = dynamic_cast<FunctionNode*>(node);
		double result = funcNode->evaluateFunction(leftTerm->getValue(), rightTerm->getValue(), true, true);

		// Vytvoření nového TerminalNode s výsledkem
		Terminal newTerm(result);
		std::unique_ptr<Node> newTermNode = std::make_unique<TerminalNode>(newTerm);
		this->nodeVec.at(i) = std::move(newTermNode);

		// Smazání potomků
		this->nodeVec.at(leftIdx) = nullptr;
		this->nodeVec.at(rightIdx) = nullptr;
	}

	this->updateStats();
}

void Individual::removeUselessBranches()
{
	for (int i = 0; i <= this->lastNodeIdx; ++i) {
		Node* node = this->nodeVec.at(i).get();
		if (!node || !node->isFunctionNode())
			continue;

		FunctionNode* funcNode = dynamic_cast<FunctionNode*>(node);
		std::string funcName = funcNode->getFunction().getName();

		int leftIdx = Individual::getLeftChildIdx(i);
		int rightIdx = leftIdx + 1;

		if (leftIdx > this->lastNodeIdx || rightIdx > this->lastNodeIdx)
			continue;

		Node* leftNode = this->nodeVec.at(leftIdx).get();
		Node* rightNode = this->nodeVec.at(rightIdx).get();

		// Ověření existence potomků
		if (!leftNode || !rightNode)
			continue;

		// Ověření, že potomci jsou terminály
		TerminalNode* leftTerm = dynamic_cast<TerminalNode*>(leftNode);
		TerminalNode* rightTerm = dynamic_cast<TerminalNode*>(rightNode);
		if (!leftTerm || !rightTerm)
			continue;

		double leftVal = leftTerm->getValue();
		double rightVal = rightTerm->getValue();

		// + nebo - s 0
		if (funcName == "+") {
			if (leftTerm->isConstant() && leftVal == 0.0) {
				// nahradit pravým potomkem
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(*rightTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
			if (rightTerm->isConstant() && rightVal == 0.0) {
				// nahradit levým potomkem
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(*leftTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
		}
		if (funcName == "-") {
			if (leftTerm->isConstant() && leftVal == 0.0) {
				// 0 - x => -x
				Terminal negTerm(-rightVal);
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(negTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
			if (rightTerm->isConstant() && rightVal == 0.0) {
				// x - 0 => x
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(*leftTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
		}

		// * nebo / s 1
		if ((funcName == "*" || funcName == "/")) {
			if (leftTerm->isConstant() && leftVal == 1.0) {
				// nahradit pravým potomkem
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(*rightTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
			if (rightTerm->isConstant() && rightVal == 1.0) {
				// nahradit levým potomkem
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(*leftTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
		}

		// * s 0
		if (funcName == "*") {
			if ((leftTerm->isConstant() && leftVal == 0.0) || (rightTerm->isConstant() && rightVal == 0.0)) {
				Terminal zeroTerm(0.0);
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(zeroTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
		}

		// / s levým potomkem 0
		if (funcName == "/") {
			if (leftTerm->isConstant() && leftVal == 0.0) {
				Terminal zeroTerm(0.0);
				this->nodeVec.at(i) = std::make_unique<TerminalNode>(zeroTerm);
				this->nodeVec.at(leftIdx) = nullptr;
				this->nodeVec.at(rightIdx) = nullptr;
				continue;
			}
		}
	}

	this->updateStats();
}

void Individual::createDAG()
{
	dagLinks.clear();
	std::map<std::string, int> subtreeHashes; // hash → index

	for (int i = 0; i <= lastNodeIdx; ++i) {
		if (!nodeVec.at(i)) continue;

		// Ignoruj terminály (podstromy hloubky 1)
		if (nodeVec.at(i)->isTerminalNode()) {
			dagLinks[i] = -1;
			continue;
		}

		std::string hash = serializeSubtree(i);

		auto it = subtreeHashes.find(hash);
		if (it != subtreeHashes.end()) {
			dagLinks[i] = it->second; // odkaz na první výskyt
		}
		else {
			dagLinks[i] = -1; // není sdílený
			subtreeHashes[hash] = i;
		}
	}
	this->dagMapCreated = true;
}

void Individual::validateTreeStructure() const
{
	for (int i = 0; i <= this->lastNodeIdx; ++i) {
		Node* node = this->nodeVec.at(i).get();
		if (!node) continue;

		bool isLeaf = this->isLeafAtIdx(i);

		if (isLeaf && node->isFunctionNode()) {
			cout << *this;
			throw std::runtime_error("Chyba: Leaf na indexu " + std::to_string(i) + " je FunctionNode!");
			exit(1);
		}
		if (!isLeaf && node->isTerminalNode()) {
			cout << *this;
			throw std::runtime_error("Chyba: Vnitřní uzel na indexu " + std::to_string(i) + " je TerminalNode!");
			exit(1);
		}
	}
}

string Individual::serializeSubtree(int idx) const
{
	if (idx < 0 || idx > lastNodeIdx || !nodeVec.at(idx)) return "#";
	std::string s = nodeVec.at(idx)->toString();
	int left = getLeftChildIdx(idx);
	int right = left + 1;
	s += "(" + serializeSubtree(left) + "," + serializeSubtree(right) + ")";
	return s;
}

void Individual::resetDagMap()
{
	this->dagMapCreated = false;
}

bool Individual::isDagMapCreated() const
{
	return this->dagMapCreated; 
}
