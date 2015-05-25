
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_TREE_H_
#define CRISP_TREE_H_

#include "token.h"

#include <map>
#include <vector>
#include <sstream>

namespace crisp {

class Node {
public:
	virtual ~Node() {}

	class State {
	public:
		State();

		class SymbolTableInterface {
		public:
			virtual ~SymbolTableInterface() {};
			virtual void Put(std::string str, Node *node) = 0;
			virtual Node *Get(std::string str) = 0;
			virtual std::string PPrint() const = 0;
		protected:
			std::map<std::string, Node *> table;
		};
		State(SymbolTableInterface *s) : symbol_table_(s) {}
		SymbolTableInterface *symbol_table() {
			return symbol_table_;
		}
	private:
		SymbolTableInterface *symbol_table_;
	};

	// prints a pretty printed representation of this
	// node and its child nodes to an ostream.
	virtual std::string PPrint() const = 0;

	// returns the result of evaluating itself
	virtual Node *Eval(State *state) const = 0;

	// NOTE: helper functions.

	bool isTrue();

};

class Scope : public Node::State::SymbolTableInterface {
public:
	Scope(Node::State::SymbolTableInterface *s) : parent(s) {}
	virtual void Put(std::string str, Node *node) {
		table[str] = node;
	}
	virtual Node *Get(std::string str) {
		Node *n = [&]()->Node* {
			try {
				return table.at(str);
			} catch (...) {
				return nullptr;
			}
		}();
		return n ? n : (parent ? parent->Get(str) : nullptr);
	}
	virtual std::string PPrint() const;
private:
	Node::State::SymbolTableInterface *parent;
};

class NullNode : public Node {
public:
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
};

class ParentNodeInterface : public Node {
public:
	virtual void Put(Node *node) = 0;
};

class ParentNode : public ParentNodeInterface {
public:
	virtual void Put(Node *node);
protected:
	std::vector<Node *> children_;
};

class ConstNode : public ParentNode {
public:
	virtual Node *Eval(State *state) const { return child; }
	virtual std::string PPrint() const;
	virtual void Put(Node *node);
private:
	Node *child;
};

class CallableNode : public Node {
public:
	virtual Node *Eval(State *state) const;
	virtual Node *Call(std::vector<Node *>& params) = 0;
};

class RootNode : public ParentNode {
	virtual Node *Eval(State *state) const;
	virtual void Put(Node *node);
	virtual std::string PPrint() const;
};

class ListNode : public ParentNode {
public:
	ListNode() {}
	virtual Node *Eval(State *state) const;
	virtual void Put(Node *node);
	virtual std::string PPrint() const;
	std::vector<Node *> children() const { return children_; }
};

class ErrorNode : public Node {
public:
	ErrorNode(std::string msg);
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
private:
	std::string msg_;
};

class IdentNode : public Node {
public:
	IdentNode(std::string ident);
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
	std::string str() const { return str_; }
private:
	std::string str_;
};

class NumNode : public Node {
public:
	NumNode(int n);
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
private:
	int num;
};

class StringNode : public Node {
public:
	StringNode(std::string str);
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
	std::string str() const { return str_; }
private:
	std::string str_;
};

class BooleanNode : public Node {
public:
	BooleanNode(bool val);
	virtual Node *Eval(State *state) const;
	virtual std::string PPrint() const;
	bool value() const { return value_; }
private:
	bool value_;
};

} // namespace crisp

#endif // CRISP_TREE_H_
