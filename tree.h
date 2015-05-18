
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

class NodeInterface {
public:
	virtual ~NodeInterface() {}

	// prints a pretty printed representation of this
	// node and its child nodes to an ostream.
	virtual std::string PPrint() const = 0;

	// returns if this node is constant, e.g. '(a b c) or 'a.
	virtual bool IsConstant() const = 0;

	virtual Position *GetPosition() = 0;

	enum Category {
		kRoot,
		kList,
		kNum,
		kIdent,
		kString,
		kError,
		kCallable,
		kNull,
		kOther
	};

	// returns category
	virtual enum Category GetCategory() const = 0;
};

class Node : public NodeInterface {
public:
	Node(bool constant = false) : constant_(constant) {}

	// returns if this atom is constant.
	virtual bool IsConstant() const {
		return constant_;
	}
protected:
	const bool constant_;
};

class NullNode : public NodeInterface {
public:
	virtual std::string PPrint() const {
		// an empty list is considered null.
		return "()";
	}

	virtual enum Category GetCategory() const {
		return kNull;
	}

	virtual bool IsConstant() const {
		return false;
	}

	virtual Position *GetPosition() {
		return nullptr;
	}
};

class ParentNode : public Node {
public:
	ParentNode(bool constant = false) : Node(constant) {}
	virtual void Put(NodeInterface *node) = 0;

	virtual std::vector<NodeInterface *>::iterator ChildBegin() {
		return children.begin();
	}

	virtual std::vector<NodeInterface *>::iterator ChildEnd() {
		return children.end();
	}
protected:
	std::vector<NodeInterface *> children;
};

class CallableNode : public Node {
public:
	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) = 0;

	virtual enum Category GetCategory() const {
		return kCallable;
	}

	virtual Position *GetPosition() {
		return nullptr;
	}
};

class RootNode : public ParentNode {
	virtual void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual Position *GetPosition() {
		return nullptr;
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		for (auto i = children.begin(); i != children.end(); i++) {
			if (*i != nullptr) {
				os << (*i)->PPrint();
			} else {
				os << "null";
			}
			if ((i + 1) != children.end()) {
				os << " ";
			}
		}
		return os.str();
	}

	virtual enum Category GetCategory() const {
		return kRoot;
	}
};

class ListNode : public ParentNode {
public:
	ListNode(Token *tok, bool con) : pos(tok->GetPosition()), ParentNode(con) {
		delete tok;
	}

	virtual void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual Position *GetPosition() {
		return &pos;
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		if (constant_) {
			os << "\'";
		}
		os << "(";
		for (auto i = children.begin(); i != children.end(); i++) {
			if (*i != nullptr) {
				os << (*i)->PPrint();
			} else {
				os << "null";
			}
			if ((i + 1) != children.end()) {
				os << " ";
			}
		}
		os << ")";
		return os.str();
	}

	virtual enum Category GetCategory() const {
		return kList;
	}

private:
	Position pos;
};

class IdentNode : public Node {
public:
	IdentNode(Token *tok, bool constant) : str(tok->GetLexeme()), pos(tok->GetPosition()), Node(constant) {
		delete tok;
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		if (constant_) {
			os << "\'";
		}
		os << str;
		return os.str();
	}

	virtual Position *GetPosition() {
		return &pos;
	}

	virtual enum Category GetCategory() const {
		return kIdent;
	}

	std::string GetIdentString() const {
		return str;
	}

private:
	std::string str;
	Position pos;
};

class NumNode : public Node {
public:
	NumNode(Token *tok, bool constant) : num([&]{
		// crappy number conversion
		int n;
		std::stringstream s;
		s << tok->GetLexeme();
		s >> n;
		return n;
	}()), pos(tok->GetPosition()), Node(constant) {
		delete tok;
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		if (constant_) {
			os << "\'";
		}
		os << num;
		return os.str();
	}

	virtual Position *GetPosition() {
		return &pos;
	}

	virtual enum Category GetCategory() const {
		return kNum;
	}

private:
	int num;
	Position pos;
};

class ErrorNode : public Node {
public:
	ErrorNode(std::string msg) : msg_(msg) {}

	virtual std::string PPrint() const {
		std::stringstream os;
		os << "Error{" << msg_ << "}";
		return os.str();
	}

	virtual Position *GetPosition() {
		return nullptr;
	}

	virtual enum Category GetCategory() const {
		return kError;
	}

private:
	std::string msg_;
};

class GeneralNode : public Node {
public:
	GeneralNode(Token *tok, bool con) : tok_(tok), Node(con) {}
	~GeneralNode() {
		delete tok_;
	}

	virtual std::string PPrint() const {
		std::stringstream os;
		if (constant_) {
			os << "\'";
		}
		os << tok_->String() << "{" << tok_->GetLexeme() << "}";
		return os.str();
	}

	virtual Position *GetPosition() {
		return nullptr;
	}

	virtual enum NodeInterface::Category GetCategory() const {
		switch (tok_->GetCategory()) {
		case Token::kNum:
			return kNum;
		case Token::kIdent:
			return kIdent;
		case Token::kString:
			return kString;
		default:
			return kOther;
		}
	}

private:
	Token *tok_;
};

// returns specific class instantiations for non-list nodes.
class NodeFactory {
public:
	static NodeFactory *GetInstance() { return &NodeFactory_; }

	NodeInterface *CreateNode(Token *tok, bool constant) {
		if (tok->GetCategory() == Token::kIdent) {
			return new IdentNode(tok, constant);
		} else if (tok->GetCategory() == Token::kNum) {
			return new NumNode(tok, constant);
		} else {
			return new GeneralNode(tok, constant);
		}
	}
private:
	static NodeFactory NodeFactory_;
};

class SymbolTable {
public:
	void Put(std::string str, NodeInterface *node) {
		table[str] = node;
	}

	NodeInterface *Get(std::string str) {
		try {
			return table.at(str);
		} catch (...) {
			return nullptr;
		}
	}

	std::string PPrint() const {
		std::stringstream os;
		for (auto i = table.begin(); i != table.end(); i++) {
			os << (*i).first << ": ";
			if ((*i).second != nullptr) {
				os << (*i).second->PPrint();
			} else {
				os << "null";
			}
			auto j = i;
			if (i++ != table.end()) {
				os << " ";
			}
			i = j;
			os << std::endl;
		}
		return os.str();
	}

	SymbolTable *Copy() {
		auto symb = new SymbolTable();
		// copy
		symb->table = table;
		return symb;
	}

private:
	std::map<std::string, NodeInterface *> table;
};

} // namespace crisp

#endif // CRISP_TREE_H_
