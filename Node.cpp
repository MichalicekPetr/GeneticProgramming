#include <cmath>
#include <iostream>
#include <stdexcept>

#include "node.h"
#include "random.h"

using namespace std;

void Node::createTreeCopyRec(Node* original, Node* parrent, NodeDirection dir) const
{
	Node* copy = original->createDeepCopy();
	copy->left = nullptr;
	copy->right = nullptr;
	copy->createParentLink(parrent, dir);

	if (original->left != nullptr) {
		createTreeCopyRec(original->left, copy, NodeDirection::Left);
	}
	if (original->right != nullptr) {
		createTreeCopyRec(original->right, copy, NodeDirection::Right);
	}

	return;
}

void Node::createTreeCopyWithReplacePointRec(Node* original, Node* parrent, NodeDirection dir, Node* replaceOriginal, Node* replacePoint) const
{
	if (original == replaceOriginal) {
		replacePoint->createParentLink(parrent, dir);
	}
	else {
		Node* copy = original->createDeepCopy();
		copy->left = nullptr;
		copy->right = nullptr;
		copy->createParentLink(parrent, dir);

		if (original->left != nullptr) {
			createTreeCopyWithReplacePointRec(original->left, copy, NodeDirection::Left, replaceOriginal, replacePoint);
		}
		if (original->right != nullptr) {
			createTreeCopyWithReplacePointRec(original->right, copy, NodeDirection::Right, replaceOriginal, replacePoint);
		}
	}
	return;
}

bool Node::isFunctionNode() const
{
	return this->type == FunctionNodeType;
}

bool Node::isTerminalNode() const
{
	return this->type == TerminalNodeType;
}

Node* Node::getLeftOffspring()
{
	return this->left;
}

Node* Node::getRightOffspring()
{
	return this->right;
}

Node* Node::getParent()
{
	return this->parent;
}

void Node::createParentLink(Node* parent, NodeDirection dir)
{
	this->parent = parent;
	if (dir == NodeDirection::Left) {
		parent->left = this;
	}
	else if (dir == NodeDirection::Right) {
		parent->right = this;
	}
	else {
		throw invalid_argument("Unknown direction");
	}
}

Node* Node::createTreeCopy() const
{
	Node* copy = this->createDeepCopy();

	copy->left = nullptr;
	copy->right = nullptr;
	copy->parent = nullptr;

	if (this->left != nullptr) {
		createTreeCopyRec(this->left, copy, NodeDirection::Left);
	}
	if (this->right != nullptr) {
		createTreeCopyRec(this->right, copy, NodeDirection::Right);
	}

	return copy;
}

Node* Node::createTreeCopyWithReplacePoint(Node* replaceOriginal, Node* replacePoint) const
{
	Node* copy = this->createDeepCopy();

	copy->left = nullptr;
	copy->right = nullptr;
	copy->parent = nullptr;

	if (this->left != nullptr) {
		createTreeCopyWithReplacePointRec(this->left, copy, NodeDirection::Left, replaceOriginal, replacePoint);
	}
	if (this->right != nullptr) {
		createTreeCopyWithReplacePointRec(this->right, copy, NodeDirection::Right, replaceOriginal, replacePoint);
	}

	return copy;
}

NodeDirection Node::getDirection()
{
	if (this->parent == nullptr) {
		throw invalid_argument("No parent");
	}
	else {
		if (this->parent->left == this) {
			return NodeDirection::Left;
		}
		else if (this->parent->right == this) {
			return NodeDirection::Right;
		}
		else {
			throw invalid_argument("No parent");
		}
	}
}

// Pøepsáno
TerminalNode::TerminalNode()
{
	this->type = TerminalNodeType;
	this->terminal = Terminal(0.0);
}

// Pøepsáno
TerminalNode::TerminalNode(Terminal& terminal)
{
	this->type = TerminalNodeType;
	this->terminal = terminal;
}

// Pøepsáno
TerminalNode::TerminalNode(const TerminalNode& original)
{
	this->type = TerminalNodeType;
	this->terminal = Terminal(original.terminal);
}

// Pøepsáno
bool TerminalNode::isFunctionNode() const
{
	return false;
}

// Pøepsáno
bool TerminalNode::isTerminalNode() const 
{
	return true;
}

Node* TerminalNode::createRandomTerminalNode(const TerminalSet& terminalSet, const double & varProb)
{
	Terminal terminal = terminalSet.getRandomTerminal(0.5);
	Node* createdNode = new TerminalNode(terminal);
	return createdNode;
}

void TerminalNode::setTerminal(const Terminal& terminal)
{
	this->terminal = terminal;
}

string TerminalNode::toString() const
{
	return this->terminal.getLabel();
}

Terminal & TerminalNode::getTerminalReference()
{
	return this->terminal;
}

double TerminalNode::evaulateNodeRec() const
{
	return this->terminal.getValue();
}

Node* TerminalNode::createDeepCopy() const
{
	Terminal terminalCopy = Terminal(this->terminal);
	TerminalNode* deepCopy = new TerminalNode(terminalCopy);
	return deepCopy;
}

double TerminalNode::getValue() const
{
	return this->terminal.getValue();
}

bool TerminalNode::isConstant() const
{
	return !this->terminal.isDataPoint();
}

unique_ptr<Node> TerminalNode::clone() const
{
	return make_unique<TerminalNode>(*this);
}

FunctionNode::FunctionNode()
{
	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
	this->type = FunctionNodeType;
	this->function = Function();
}

FunctionNode::FunctionNode(const Function& func)
{
	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
	this->type = FunctionNodeType;
	this->function = func;
}

// Pøepsáno
FunctionNode::FunctionNode(const FunctionNode& original)
{
	this->type = FunctionNodeType;
	this->function = Function(original.function);
}

bool FunctionNode::isFunctionNode() const
{
	return true;
}

bool FunctionNode::isTerminalNode() const
{
	return false;
}

Node* FunctionNode::createRandomFunctionNode(const FunctionSet& functionSet)
{
	Function func = functionSet.getRandomFunction();
	Node* funcNode = new FunctionNode(func);
	return funcNode;
}

string FunctionNode::toString() const
{
	return this->function.getName();
}

double FunctionNode::evaulateNodeRec() const
{
	double right = 0.0;
	double left = 0.0;
	if((this->left == nullptr) && (this->right == nullptr)){
		return nan("0");
	}
	else if (this->left == nullptr && this->right != nullptr) {
		if (this->function.getParity() == 1) {
			right = this->right->evaulateNodeRec();
			if (!isnan(right)) {
				return this->function.evaluate(right, 0.0);
			}	
		}
		return  right;	
	}
	else if (this->left != nullptr && this->right == nullptr) {
		if (this->function.getParity() == 1) {
			left = this->left->evaulateNodeRec();
			if (!isnan(left)) {
				return this->function.evaluate(left, 0.0);
			}
		}
		return left;
	}
	else {
		left = this->left->evaulateNodeRec();
		right = this->right->evaulateNodeRec();
		if (isnan(left) && isnan(right)) {
			return  nan("0");
		}
		else if (!isnan(left) && isnan(right)) {
			if (this->function.getParity() == 1) {
				return this->function.evaluate(left, 0.0);
			}
			else {
				return left;
			}
		}
		else if (isnan(left) && !isnan(right)) {
			if (this->function.getParity() == 1) {
				return this->function.evaluate(right, 0.0);
			}
			else {
				return right;
			}
		}
		else{
			return this->function.evaluate(left , right);
		}
	}
}

Node* FunctionNode::createDeepCopy() const
{
	FunctionNode* deepCopy = new FunctionNode(Function(this->function));
	return deepCopy;
}

Node* FunctionNode::getLeftOffspring()
{
	return this->left;
}

Node* FunctionNode::getRightOffspring()
{
	return this->right;
}

void FunctionNode::setFunc(Function newFunc)
{
	this->function = newFunc;
}

unique_ptr<Node> FunctionNode::clone() const
{
	return make_unique<FunctionNode>(*this);
}
