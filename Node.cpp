#include <cmath>
#include <iostream>
#include <stdexcept>

#include "node.h"
#include "random.h"

using namespace std;


// Pøepsáno
bool Node::isFunctionNode() const
{
	return this->type == FunctionNodeType;
}

// Pøepsáno
bool Node::isTerminalNode() const
{
	return this->type == TerminalNodeType;
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

// Pøepsáno
Node* TerminalNode::createRandomTerminalNode(const TerminalSet& terminalSet, const double & varProb)
{
	Terminal terminal = terminalSet.getRandomTerminal(1 - varProb);
	Node* createdNode = new TerminalNode(terminal);
	return createdNode;
}

// Pøepsáno
void TerminalNode::setTerminal(const Terminal& terminal)
{
	this->terminal = terminal;
}

// Pøepsáno
string TerminalNode::toString() const
{
	return this->terminal.getLabel();
}

// Pøepsáno
Terminal & TerminalNode::getTerminalReference()
{
	return this->terminal;
}

// Pøepsáno
Node* TerminalNode::createDeepCopy() const
{
	Terminal terminalCopy = Terminal(this->terminal);
	TerminalNode* deepCopy = new TerminalNode(terminalCopy);
	return deepCopy;
}

// Pøepsáno
double TerminalNode::getValue() const
{
	return this->terminal.getValue();
}

// Pøepsáno
bool TerminalNode::isConstant() const
{
	return !this->terminal.isDataPoint();
}

// Pøepsáno
unique_ptr<Node> TerminalNode::clone() const
{
	return make_unique<TerminalNode>(*this);
}

// Pøepsáno
FunctionNode::FunctionNode()
{
	this->type = FunctionNodeType;
	this->function = Function();
}

// Pøepsáno
FunctionNode::FunctionNode(const Function& func)
{
	this->type = FunctionNodeType;
	this->function = Function(func);
}

// Pøepsáno
FunctionNode::FunctionNode(const FunctionNode& original)
{
	this->type = FunctionNodeType;
	this->function = Function(original.function);
}

// Pøepsáno
bool FunctionNode::isFunctionNode() const
{
	return true;
}

// Pøepsáno
bool FunctionNode::isTerminalNode() const
{
	return false;
}

// Pøepsáno
Node* FunctionNode::createRandomFunctionNode(const FunctionSet& functionSet)
{
	Function func = functionSet.getRandomFunction();
	Node* funcNode = new FunctionNode(func);
	return funcNode;
}

// Pøepsáno
string FunctionNode::toString() const
{
	return this->function.getName();
}

// Pøepsáno
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

// Pøepsáno
Node* FunctionNode::createDeepCopy() const
{
	FunctionNode* deepCopy = new FunctionNode(Function(this->function));
	return deepCopy;
}

// Pøepsáno
void FunctionNode::setFunc(Function newFunc)
{
	this->function = newFunc;
}

// Pøepsáno
unique_ptr<Node> FunctionNode::clone() const
{
	return make_unique<FunctionNode>(*this);
}
