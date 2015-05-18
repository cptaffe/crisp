
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_TREE_H_
#define CRISP_TREE_H_

#include "token.h"

#include <map>
#include <vector>
#include <ostream>

namespace crisp {

class NodeInterface {
public:
	virtual ~NodeInterface() {}

	// prints a pretty printed representation of this
	// node and its child nodes to an ostream.
	virtual void PPrint(std::ostream& os) = 0;

	// returns if this node is constant, e.g. '(a b c) or 'a.
	virtual bool IsConstant() const = 0;

	enum Category {
		kRoot,
		kList,
		kNum,
		kIdent,
		kString,
		kOther
	};

	// returns category
	virtual enum Category GetCategory() const = 0;
};

class ParentNodeInterface : public NodeInterface {
public:
	virtual void Put(NodeInterface *node) = 0;

	// returns child nodes, returns nullptr when
	// a number larger than the array of children is
	// passed.
	virtual NodeInterface *GetChild(int n) = 0;
};

class RootNode : public ParentNodeInterface {
	~RootNode() {
		for (auto i = children.begin(); i != children.end(); i++) {
			delete *i;
		}
	}

	virtual void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual NodeInterface *GetChild(int n) {
		if (n < children.size() && n >= 0) {
			return children[n];
		} else {
			return nullptr;
		}
	}

	virtual void PPrint(std::ostream& os) {
		for (auto i = children.begin(); i != children.end(); i++) {
			(*i)->PPrint(os);
			if ((i + 1) != children.end()) {
				os << " ";
			}
		}
	}

	virtual bool IsConstant() const {
		return false;
	}

	virtual enum Category GetCategory() const {
		return kRoot;
	}

private:
	std::vector<NodeInterface *> children;
};

class ListNode : public ParentNodeInterface {
public:
	ListNode(Token *tok, bool con) : pos(tok->GetPosition()), constant_(con) {
		delete tok;
		for (auto i = children.begin(); i != children.end(); i++) {
			delete *i;
		}
	}
	~ListNode() {
		for (auto i = children.begin(); i != children.end(); i++) {
			delete *i;
		}
	}

	virtual void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual NodeInterface *GetChild(int n) {
		if (n < children.size() && n >= 0) {
			return children[n];
		} else {
			return nullptr;
		}
	}

	virtual void PPrint(std::ostream& os) {
		if (constant_) {
			os << "\'";
		}
		os << "(";
		for (auto i = children.begin(); i != children.end(); i++) {
			(*i)->PPrint(os);
			if ((i + 1) != children.end()) {
				os << " ";
			}
		}
		os << ")";
	}

	virtual bool IsConstant() const {
		return constant_;
	}

	virtual enum Category GetCategory() const {
		return kList;
	}

private:
	bool constant_;
	std::vector<NodeInterface *> children;
	Position pos;
};

class IdentNode : public NodeInterface {
public:
	IdentNode(Token *tok, bool constant) : str(tok->GetLexeme()), pos(tok->GetPosition()), constant_(constant) {
		delete tok;
	}

	virtual void PPrint(std::ostream& os) {
		if (constant_) {
			os << "\'";
		}
		os << str;
	}

	virtual enum Category GetCategory() const {
		return kIdent;
	}

	virtual bool IsConstant() const {
		return constant_;
	}

private:
	std::string str;
	Position pos;
	const bool constant_;
};

class Node : public NodeInterface {
public:
	Node(Token *tok, bool con) : tok_(tok), constant_(con) {}
	~Node() {
		delete tok_;
	}

	virtual void PPrint(std::ostream& os) {
		if (constant_) {
			os << "\'";
		}
		os << tok_->String() << "{" << tok_->GetLexeme() << "}";
	}

	virtual bool IsConstant() const {
		return constant_;
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
	bool constant_;
};

// returns specific class instantiations for non-list nodes.
class NodeFactory {
public:
	static NodeFactory *GetInstance() { return &NodeFactory_; }

	NodeInterface *CreateNode(Token *tok, bool constant) {
		if (tok->GetCategory() == Token::kIdent) {
			return new IdentNode(tok, constant);
		} else {
			return new Node(tok, constant);
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
private:
	std::map<std::string, NodeInterface *> table;
};

} // namespace crisp

#endif // CRISP_TREE_H_
