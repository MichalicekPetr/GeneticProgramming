#pragma once

#include "Function.h"
#include "Terminal.h"

enum NodeType {
	FunctionNodeType,
	TerminalNodeType
};

enum NodeDirection {
	Left, 
	Right
};

class Node
{
protected:
	NodeType type;

	

public:
	virtual bool isFunctionNode() const = 0;
	virtual bool isTerminalNode() const = 0;


	virtual string toString() const = 0;

	virtual Node* createDeepCopy() const = 0;

	virtual unique_ptr<Node> clone() const = 0;
};

class FunctionNode : public Node
{
private:
	Function function;
	NodeType type;

public:
	FunctionNode();
	FunctionNode(const Function& func);
	FunctionNode(const FunctionNode& original);

	bool isFunctionNode() const override;
	bool isTerminalNode() const override;
	static Node* createRandomFunctionNode(const FunctionSet& functionSet);
	string toString() const override;
	double evaluateFunction(const double& left, const double& right, bool leftValid, bool rightValid);
	Node* createDeepCopy() const override;
	void setFunc(Function newFunc);
	unique_ptr<Node> clone() const override;
	const Function& getFunction() const;
};


class TerminalNode : public Node
{
private:
	Terminal terminal;
	NodeType type;

public:
	TerminalNode();
	TerminalNode(Terminal & terminal);
	TerminalNode(const TerminalNode& original);

	bool isFunctionNode() const override;
	bool isTerminalNode() const override;
	
	static Node * createRandomTerminalNode(const TerminalSet& terminalSet, const double & varProb = 0.5);

	void setTerminal(const Terminal & terminal);
	string toString() const override;
	
	Terminal & getTerminalReference();
	const Terminal & getTerminalConst() const;
	Node* createDeepCopy() const override;

	double getValue() const;

	bool isConstant() const;
	unique_ptr<Node> clone() const override;
};
