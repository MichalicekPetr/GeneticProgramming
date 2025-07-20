#include <cmath>
#include <iostream>
#include <stdexcept>

#include "node.h"
#include "random.h"

using namespace std;


// P�eps�no
bool Node::isFunctionNode() const
{
	return this->type == FunctionNodeType;
}

// P�eps�no
bool Node::isTerminalNode() const
{
	return this->type == TerminalNodeType;
}

// P�eps�no
TerminalNode::TerminalNode()
{
	this->type = TerminalNodeType;
	this->terminal = Terminal(0.0);
}

// P�eps�no
TerminalNode::TerminalNode(Terminal& terminal)
{
	this->type = TerminalNodeType;
	this->terminal = terminal;
}

// P�eps�no
TerminalNode::TerminalNode(const TerminalNode& original)
{
	this->type = TerminalNodeType;
	this->terminal = Terminal(original.terminal);
}

// P�eps�no
bool TerminalNode::isFunctionNode() const
{
	return false;
}

// P�eps�no
bool TerminalNode::isTerminalNode() const 
{
	return true;
}

// P�eps�no
Node* TerminalNode::createRandomTerminalNode(const TerminalSet& terminalSet, const double & varProb)
{
	Terminal terminal = terminalSet.getRandomTerminal(1 - varProb);
	Node* createdNode = new TerminalNode(terminal);
	return createdNode;
}

// P�eps�no
void TerminalNode::setTerminal(const Terminal& terminal)
{
	this->terminal = terminal;
}

// P�eps�no
string TerminalNode::toString() const
{
	return this->terminal.getLabel();
}

// P�eps�no
Terminal & TerminalNode::getTerminalReference()
{
	return this->terminal;
}

// P�eps�no
Node* TerminalNode::createDeepCopy() const
{
	Terminal terminalCopy = Terminal(this->terminal);
	TerminalNode* deepCopy = new TerminalNode(terminalCopy);
	return deepCopy;
}

// P�eps�no
double TerminalNode::getValue() const
{
	return this->terminal.getValue();
}

// P�eps�no
bool TerminalNode::isConstant() const
{
	return !this->terminal.isDataPoint();
}

// P�eps�no
unique_ptr<Node> TerminalNode::clone() const
{
	return make_unique<TerminalNode>(*this);
}

// P�eps�no
FunctionNode::FunctionNode()
{
	this->type = FunctionNodeType;
	this->function = Function();
}

// P�eps�no
FunctionNode::FunctionNode(const Function& func)
{
	this->type = FunctionNodeType;
	this->function = Function(func);
}

// P�eps�no
FunctionNode::FunctionNode(const FunctionNode& original)
{
	this->type = FunctionNodeType;
	this->function = Function(original.function);
}

// P�eps�no
bool FunctionNode::isFunctionNode() const
{
	return true;
}

// P�eps�no
bool FunctionNode::isTerminalNode() const
{
	return false;
}

// P�eps�no
Node* FunctionNode::createRandomFunctionNode(const FunctionSet& functionSet)
{
	Function func = functionSet.getRandomFunction();
	Node* funcNode = new FunctionNode(func);
	return funcNode;
}

// P�eps�no
string FunctionNode::toString() const
{
	return this->function.getName();
}

// P�eps�no
double FunctionNode::evaluateFunction(const double& left, const double& right, bool leftValid, bool rightValid)
{
	if (!leftValid && !rightValid)
		return nan("0");
	else if (!leftValid && rightValid) {
		if (this->function.getParity() == 1) {
			if (!isnan(right)) {
				return this->function.evaluate(right, 0.0);
			}
		}
		if (this->function.getName() == "-")
			return -right;
		else if (this->function.getName() == "%")
			return 1 / right;
		else
			return  right;
	}
	else if (leftValid && !rightValid) {
		if (this->function.getParity() == 1) {
			if (!isnan(left)) {
				return this->function.evaluate(left, 0.0);
			}
		}
		return left;
	}
	else {
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
		else {
			return this->function.evaluate(left, right);
		}
	}
}

// P�eps�no
Node* FunctionNode::createDeepCopy() const
{
	FunctionNode* deepCopy = new FunctionNode(Function(this->function));
	return deepCopy;
}

// P�eps�no
void FunctionNode::setFunc(Function newFunc)
{
	this->function = newFunc;
}

// P�eps�no
unique_ptr<Node> FunctionNode::clone() const
{
	return make_unique<FunctionNode>(*this);
}
