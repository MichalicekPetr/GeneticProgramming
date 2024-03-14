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
	Node* left;
	Node* right;
	Node* parent;

	void createTreeCopyRec(Node* original, Node * parrent, NodeDirection dir) const;
	void createTreeCopyWithReplacePointRec(Node* original, Node* parrent, NodeDirection dir, Node * replaceOriginal, Node * replacePoint) const;

public:
	virtual bool isFunctionNode() const = 0;
	virtual bool isTerminalNode() const = 0;

	virtual Node* getLeftOffspring();
	virtual Node* getRightOffspring();
	virtual Node* getParent();

	void createParentLink(Node* parent, NodeDirection dir);

	virtual string toString() const = 0;

	virtual double evaulateNodeRec() const = 0;
	bool isLeaf();

	Node* createTreeCopy() const;
	Node* createTreeCopyWithReplacePoint(Node* replaceOriginal, Node* replacePoint) const;
	virtual Node* createDeepCopy() const = 0;

	NodeDirection getDirection();
};

class FunctionNode : public Node
{
	using Node::left;
	using Node::right;
	using Node::parent;
private:
	Function function;
	NodeType type;

public:
	FunctionNode();
	FunctionNode(const Function& func);

	bool isFunctionNode() const override;
	bool isTerminalNode() const override;
	int getParity();
	static Node* createRandomFunctionNode(const FunctionSet& functionSet);
	string toString() const override;
	double evaulateNodeRec() const override;
	Node* createDeepCopy() const override;
	Node* getLeftOffspring() override;
	Node* getRightOffspring() override;
	void setFunc(Function newFunc);
};


class TerminalNode : public Node
{
	using Node::left;
	using Node::right;
	using Node::parent;
private:
	Terminal terminal;
	NodeType type;

public:
	TerminalNode();
	TerminalNode(Terminal & terminal);

	bool isFunctionNode() const override;
	bool isTerminalNode() const override;
	
	static Node * createRandomTerminalNode(const TerminalSet& terminalSet, const double & varProb = 0.5);

	void setTerminal(const Terminal & terminal);
	string toString() const override;
	
	Terminal & getTerminalReference();
	double evaulateNodeRec() const override;
	Node* createDeepCopy() const override;

	double getValue() const;

	bool isConstant() const;
};
