
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

	// Category, each pertaining to its class.
	// Use this to cast appropriately from NodeInterface.
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
	virtual enum Category category() const = 0;
};

class NullNode : public NodeInterface {
public:
	virtual std::string PPrint() const {
		// an empty list is considered null.
		return "()";
	}

	virtual enum Category category() const {
		return kNull;
	}
};

class ParentNode : public NodeInterface {
public:
	ParentNode(bool constant = false) {}
	virtual void Put(NodeInterface *node) = 0;

	virtual std::vector<NodeInterface *>::iterator Begin() {
		return children.begin();
	}

	virtual std::vector<NodeInterface *>::iterator End() {
		return children.end();
	}
protected:
	std::vector<NodeInterface *> children;
};

class CallableNode : public NodeInterface {
public:
	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) = 0;

	virtual enum Category category() const {
		return kCallable;
	}
};

class RootNode : public ParentNode {
	virtual void Put(NodeInterface *node) {
		children.push_back(node);
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

	virtual enum Category category() const {
		return kRoot;
	}
};

class ListNode : public ParentNode {
public:
	ListNode(Token *tok, bool con = false) : constant_(con) {}

	virtual void Put(NodeInterface *node) {
		children.push_back(node);
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

	virtual enum Category category() const {
		return kList;
	}

	virtual bool constant() const {
		return constant_;
	}

private:
	const bool constant_;
};

class IdentNode : public NodeInterface {
public:
	IdentNode(Token *tok, bool con = false) : constant_(con), str_(tok->GetLexeme()) {}

	virtual std::string PPrint() const {
		std::stringstream os;
		if (constant_) {
			os << "\'";
		}
		os << str();
		return os.str();
	}

	virtual bool constant() const {
		return constant_;
	}

	virtual enum Category category() const {
		return kIdent;
	}

	std::string str() const {
		return str_;
	}

private:
	std::string str_;
	const bool constant_;
};

class NumNode : public NodeInterface {
public:
	NumNode(Token *tok) : num([&]{
		// crappy number conversion
		int n;
		std::stringstream s;
		s << tok->GetLexeme();
		s >> n;
		return n;
	}()) {}

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

class StringNode : public NodeInterface {
public:
	StringNode(Token *tok) : str_(tok->GetLexeme()) {}

	virtual std::string PPrint() const {
		std::stringstream os;
		os << "\"" << str() << "\"";
		return os.str();
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

class ErrorNode : public NodeInterface {
public:
	ErrorNode(std::string msg) : msg_(msg) {}

	virtual std::string PPrint() const {
		std::stringstream os;
		os << "Error{" << msg_ << "}";
		return os.str();
	}

	virtual enum Category category() const {
		return kError;
	}

private:
	std::string msg_;
};

// returns specific class instantiations for non-list nodes.
class NodeFactory {
public:
	static NodeFactory *GetInstance() { return &NodeFactory_; }

	NodeInterface *CreateNode(Token *tok, bool constant) {
		if (tok->GetCategory() == Token::kIdent) {
			return new IdentNode(tok, constant);
		} else if (tok->GetCategory() == Token::kNum) {
			return new NumNode(tok);
		} else if (tok->GetCategory() == Token::kString) {
			return new StringNode(tok);
		} else {
			std::stringstream s;
			s << "Unknown token type '" << tok->String() << "'";
			return new ErrorNode(s.str());
		}
	}
private:
	static NodeFactory NodeFactory_;
};

class SymbolTable {
public:
	SymbolTable() = default;
	SymbolTable(const SymbolTable& s) {
		table = s.table;
	}

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

private:
	std::map<std::string, NodeInterface *> table;
};

} // namespace crisp

#endif // CRISP_TREE_H_
