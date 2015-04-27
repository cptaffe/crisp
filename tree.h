
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_TREE_H_
#define CRISP_TREE_H_

#include "token.h"

#include <map>

namespace crisp {

// generic table item for encapsulating different data types.
class SymbolTableItem {
public:
	enum SymbolTableCategory {
		kInteger,
		kFloat,
		kString,
		kIdent
	};
	SymbolTableItem(int i) : category_(kInteger), integer(i) {}
	SymbolTableItem(float f) : category_(kFloat), floating_point(f) {}
	SymbolTableItem(SymbolTableCategory cat, std::string *str) : category_(cat), string(str) {}
private:
	SymbolTableCategory category_;
	union {
		int integer;
		float floating_point;
		std::string *string;
	};
};

class NodeInterface {
public:
	// NodeCategory allows for casting by indicating
	// which subclass a node is.
	enum NodeCategory {
		kSymbol,
	};

	virtual ~NodeInterface() {}

	virtual int NumChildren() = 0;
	virtual NodeInterface *Child(int n) = 0;
	virtual NodeInterface *Parent() = 0;

	virtual NodeCategory Category() = 0;
};

class SymbolNode : public NodeInterface {
public:
	SymbolNode(NodeInterface *p) : parent(p) {}

	virtual int NumChildren() { return children.size(); }

	virtual NodeInterface *GetChild(int n) {
		// simple bounds checking.
		if (n < children.size()) {
			return children[n];
		} else {
			return nullptr;
		}
	}

	virtual NodeCategory Category() { return category; }
private:
	static const NodeCategory category = kSymbol;
	std::vector<NodeInterface *> children;
	NodeInterface *parent;
};

class TreeGenerator {
public:
	void Put(Token *t);
private:
	NodeInterface *root;
	NodeInterface *current;
};

} // namespace crisp

#endif // CRISP_TREE_H_
