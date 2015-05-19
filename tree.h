
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

		class SymbolTable {
		public:
			SymbolTable() = default;
			SymbolTable(const SymbolTable& s) {
				table = s.table;
			}

			void Put(std::string str, Node *node) {
				table[str] = node;
			}

			Node *Get(std::string str) {
				try {
					return table.at(str);
				} catch (...) {
					return nullptr;
				}
			}

			std::string PPrint() const {
				std::string s;
				for (auto i = table.begin(); i != table.end(); i++) {
					if (i != table.begin()) {
						s += " ";
					}
					s += (*i).first + ": ";
					if ((*i).second != nullptr) {
						s += (*i).second->PPrint();
					} else {
						s += "null";
					}
					s += '\n';
				}
				return s;
			}

		private:
			std::map<std::string, Node *> table;
		};

		State(SymbolTable s) : symbol_table_(s) {}

		SymbolTable& symbol_table() {
			return symbol_table_;
		}

	private:
		SymbolTable symbol_table_;
	};

	// prints a pretty printed representation of this
	// node and its child nodes to an ostream.
	virtual std::string PPrint() const = 0;

	// returns the result of evaluating itself
	virtual Node *Eval(State *state) const = 0;

	// Category, each pertaining to its class.
	// Use this to cast appropriately from Node.
	enum Category {
		kRoot,
		kList,
		kNum,
		kIdent,
		kString,
		kError,
		kCallable,
		kNull,
		kConst,
		kOther
	};

	// returns category
	virtual enum Category category() const = 0;
};

class NullNode : public Node {
public:
	virtual Node *Eval(State *state) const {
		return const_cast<NullNode *>(this); // evalutate to self
	}

	virtual std::string PPrint() const {
		// an empty list is considered null.
		return "()";
	}

	virtual enum Category category() const {
		return kNull;
	}
};

class ParentNodeInterface : public Node {
public:
	virtual void Put(Node *node) = 0;
};

class ParentNode : public ParentNodeInterface {
public:
	virtual void Put(Node *node) {
		children_.push_back(node);
	}
protected:
	std::vector<Node *> children_;
};

class ConstNode : public ParentNode {
public:

	virtual Node *Eval(State *state) const {
		return child; // remove one layer of const-ness
	}

	virtual std::string PPrint() const {
		return std::string("'") + child->PPrint();
	}

	virtual enum Category category() const {
		return kConst;
	}

	virtual void Put(Node *node) {
		child = node;
	}
private:
	Node *child;
};

class CallableNode : public Node {
public:
	virtual Node *Eval(State *state) const {
		return const_cast<CallableNode *>(this); // callable nodes evaluate to themselves.
	}

	virtual Node *Call(std::vector<Node *>& params) = 0;

	virtual enum Category category() const {
		return kCallable;
	}
};

class RootNode : public ParentNode {
	virtual Node *Eval(State *state) const {
		RootNode *root = new RootNode(); // copy self
		for (auto i = children_.begin(); i != children_.end(); i++) {
			root->Put((*i)->Eval(state));
		}
		return root;
	}

	virtual void Put(Node *node) {
		children_.push_back(node);
	}

	virtual std::string PPrint() const {
		std::string s;
		for (auto i = children_.begin(); i != children_.end(); i++) {
			if (i != children_.begin()) {
				s += " ";
			}
			if (*i != nullptr) {
				s += (*i)->PPrint();
			} else {
				s += "null";
			}
		}
		return s;
	}

	virtual enum Category category() const {
		return kRoot;
	}
};

class ListNode : public ParentNode {
public:
	ListNode(Token *tok, bool con = false) {}

	virtual Node *Eval(State *state) const;

	virtual void Put(Node *node) {
		children_.push_back(node);
	}

	virtual std::string PPrint() const {
		std::string s;
		s += "(";
		for (auto i = children_.begin(); i != children_.end(); i++) {
			if (i != children_.begin()) {
				s += " ";
			}
			if (*i != nullptr) {
				s += (*i)->PPrint();
			} else {
				s += "null";
			}
		}
		s += ")";
		return s;
	}

	std::vector<Node *> children() const {
		return children_;
	}

	virtual enum Category category() const {
		return kList;
	}
};

class ErrorNode : public Node {
public:
	ErrorNode(std::string msg) : msg_(msg) {}

	virtual Node *Eval(State *state) const {
		return const_cast<ErrorNode *>(this); // error nodes evaluate to themselves
	}

	virtual std::string PPrint() const {
		return std::string("Error: ") + msg_;
	}

	virtual enum Category category() const {
		return kError;
	}

private:
	std::string msg_;
};

class IdentNode : public Node {
public:
	IdentNode(Token *tok) : str_(tok->GetLexeme()) {}

	virtual Node *Eval(State *state) const {
		// lookup in symbol table
		Node *def = state->symbol_table().Get(str_);
		return def ? def->Eval(state) : new ErrorNode(std::string("variable '") + str() + "' is undefined");
	}

	virtual std::string PPrint() const {
		return str();
	}

	virtual enum Category category() const {
		return kIdent;
	}

	std::string str() const {
		return str_;
	}

private:
	std::string str_;
};

class NumNode : public Node {
public:
	NumNode(Token *tok) : num([&]{
		// crappy number conversion
		int n;
		std::stringstream s;
		s << tok->GetLexeme();
		s >> n;
		return n;
	}()) {}

	virtual Node *Eval(State *state) const {
		return const_cast<NumNode *>(this); // num nodes evaluate to themselves
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		os << num;
		return os.str();
	}

	virtual enum Category category() const {
		return kNum;
	}

private:
	int num;
};

class StringNode : public Node {
public:
	StringNode(Token *tok) : str_(tok->GetLexeme()) {}

	virtual Node *Eval(State *state) const {
		return const_cast<StringNode *>(this); // string nodes evaluate to themselves
	}

	virtual std::string PPrint() const {
		return std::string("\"") + str() + "\"";
	}

	virtual enum Category category() const {
		return kString;
	}

	std::string str() const {
		return str_;
	}
private:
	std::string str_;
};

// Callable nodes.

// (def name thing)
class Define : public CallableNode {
public:
	Define(Node::State::SymbolTable& t);
	virtual std::string PPrint() const;
	virtual Node *Call(std::vector<Node *>& params);
private:
	Node::State::SymbolTable& table;
};

class LambdaInstance : public CallableNode {
public:
	LambdaInstance(Node::State::SymbolTable& t, ListNode *f, Node *body);
	virtual std::string PPrint() const;
	virtual Node *Call(std::vector<Node *>& params);
private:
	Node::State::SymbolTable& table;
	ListNode *func;
	Node *func_body;
};

class Lambda : public CallableNode {
public:
	Lambda(Node::State::SymbolTable& t) : table(t) {}

	virtual std::string PPrint() const {
		return "Builtin<Lambda>";
	}

	virtual Node *Call(std::vector<Node *>& params) {
		// this callable takes arguments to create a lambda,
		// then returns another callable that executes the lambda.
		if (params.size() == 2) {
			if (params[0]->category() == Node::kList) {
				return new LambdaInstance(table, static_cast<ListNode *>(params[0]), params[1]);
			} else {
				return new ErrorNode(std::string("Lambda: first atom must be  List, not '") + params[0]->PPrint() + "'");
			}
		} else {
			return new ErrorNode("Lambda: takes two atoms");
		}
	}
private:
	Node::State::SymbolTable& table;
};

} // namespace crisp

#endif // CRISP_TREE_H_
