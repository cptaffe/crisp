
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
	virtual void Put(NodeInterface *node) = 0;
	virtual void PPrint(std::ostream& os) = 0;
	virtual Token *tok() const = 0;
};

class ListNode : public NodeInterface {
public:
	ListNode(Token *tok) : tok_(tok) {}

	virtual void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual void PPrint(std::ostream& os) {
		os << "(";
		for (auto i = children.begin(); i != children.end(); i++) {
			(*i)->PPrint(os);
			if ((i + 1) != children.end()) {
				os << " ";
			}
		}
		os << ")";
	}

	virtual Token *tok() const {
		return tok_;
	}
private:
	Token *tok_;
	std::vector<NodeInterface *> children;
};

class Node : public NodeInterface {
public:
	Node(Token *tok) : tok_(tok) {}
	~Node() {
		delete tok_;
		for (auto i = children.begin(); i != children.end(); i++) {
			delete *i;
		}
	}

	void Put(NodeInterface *node) {
		children.push_back(node);
	}

	virtual void PPrint(std::ostream& os) {
		os << tok_->String() << "{" << tok_->lexeme() << "}";
	}

	Token *tok() const {
		return tok_;
	}

private:
	Token *tok_;
	std::vector<NodeInterface *> children;
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
