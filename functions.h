
#ifndef CRISP_FUNCTIONS_H_
#define CRISP_FUNCTIONS_H_

#include "tree.h"

namespace crisp {

class CallNode : public CallableNode {
public:
	CallNode(Node::State *s) : state(s) {}
protected:
	Node::State *state;
};

// callable node that adds symbols to a symbol table.
class DefineFunc : public CallNode {
public:
	DefineFunc(Node::State *s) : CallNode(s) {}
	virtual std::string PPrint() const;
	virtual Node *Call(std::vector<Node *>& params);
};

class LambdaFunc : public CallNode {
public:
	LambdaFunc(Node::State *s) : CallNode(s) {}

	class Instance : public CallableNode {
	public:
		Instance(Node::State::SymbolTableInterface *t, IdentNode *n, Node *expression);
		virtual std::string PPrint() const;
		virtual Node *Call(std::vector<Node *>& params);
	private:
		Node::State::SymbolTableInterface *table;
		IdentNode *name;
		Node *exp;
	};

	virtual std::string PPrint() const { return "{lambda}"; }
	virtual Node *Call(std::vector<Node *>& params);
};

class NotFunc : public CallNode {
public:
	NotFunc(Node::State *s) : CallNode(s) {}
	virtual std::string PPrint() const { return "{not}"; }
	virtual Node *Call(std::vector<Node *>& params);
};

class QuoteFunc : public CallNode {
public:
	QuoteFunc(Node::State *s) : CallNode(s) {}
	virtual std::string PPrint() const { return "{quote}"; }
	virtual Node *Call(std::vector<Node *>& params);
};

}; // namespace crisp

#endif // CRISP_FUNCTIONS_H_
