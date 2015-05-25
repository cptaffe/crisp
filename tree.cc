
#include "tree.h"
#include "functions.h"
#include <string>

namespace crisp {

Node::State::State() : symbol_table_(new Scope(nullptr)) {
	symbol_table_->Put("def", new DefineFunc(this));
	symbol_table_->Put("lambda", new LambdaFunc(this));
	symbol_table_->Put("#t", new BooleanNode(true));
	symbol_table_->Put("#f", new BooleanNode(false));
	symbol_table_->Put("not", new NotFunc(this));
	symbol_table_->Put("quote", new QuoteFunc(this));
}

std::string Scope::PPrint() const {
	std::string s;
	for (auto i = table.begin(); i != table.end(); i++) {
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

bool Node::isTrue() {
	// a node is true if it is not false.
	auto b = dynamic_cast<const BooleanNode *>(this);
	return b == nullptr || b->value() == true;
}

Node *NullNode::Eval(State *state) const {
	return const_cast<NullNode *>(this); // evalutate to self
}

std::string NullNode::PPrint() const {
	// an empty list is considered null.
	return "()";
}

void ParentNode::Put(Node *node) {
	children_.push_back(node);
}

std::string ConstNode::PPrint() const {
	return std::string("'") + child->PPrint();
}

void ConstNode::Put(Node *node) {
	child = node;
}

Node *CallableNode::Eval(State *state) const {
	// callable nodes evaluate to themselves.
	return const_cast<CallableNode *>(this);
}

Node *RootNode::Eval(State *state) const {
	RootNode *root = new RootNode(); // copy self
	for (auto i: children_) {
		root->Put(i->Eval(state));
	}
	return root;
}

void RootNode::Put(Node *node) {
	children_.push_back(node);
}

std::string RootNode::PPrint() const {
	std::string s;
	for (auto i: children_) { s += i->PPrint() + " "; }
	return s;
}

Node *ListNode::Eval(State *state) const {
	// list nodes attempt to call the first atom
	// with the following atoms as parameters.
	if (children_.begin() != children_.end()) {
		// assure first atom is callable
		Node *callNode = (*children_.begin())->Eval(state);
		if (dynamic_cast<CallableNode *>(callNode) != nullptr) {
			// execute call
			// create a vector of parameters.
			std::vector<Node *> params(children_.begin() + 1, children_.end());
			return static_cast<CallableNode *>(callNode)->Call(params);
		} else {
			return new ErrorNode(std::string("List: first atom must be Callable not '") + callNode->PPrint() + "'");
		}
	} else {
		// empty list evaluates to null.
		return new NullNode();
	}
}

void ListNode::Put(Node *node) {
	children_.push_back(node);
}

std::string ListNode::PPrint() const {
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

ErrorNode::ErrorNode(std::string msg) : msg_(msg) {}

Node *ErrorNode::Eval(State *state) const {
	return const_cast<ErrorNode *>(this); // error nodes evaluate to themselves
}

std::string ErrorNode::PPrint() const {
	return std::string("Error: ") + msg_;
}

IdentNode::IdentNode(std::string id) : str_(id) {}

Node *IdentNode::Eval(State *state) const {
	// lookup in symbol table
	Node *def = state->symbol_table()->Get(str_);
	return def == nullptr ? new ErrorNode(std::string("variable '") + str() + "' is undefined") : def->Eval(state);
}

std::string IdentNode::PPrint() const {
	return str();
}

NumNode::NumNode(int n) : num(n) {}

Node *NumNode::Eval(State *state) const {
	return const_cast<NumNode *>(this); // num nodes evaluate to themselves
}

std::string NumNode::PPrint() const {
	std::stringstream os;
	os << num;
	return os.str();
}

StringNode::StringNode(std::string str) : str_(str) {}

Node *StringNode::Eval(State *state) const {
	return const_cast<StringNode *>(this); // string nodes evaluate to themselves
}

std::string StringNode::PPrint() const {
	return std::string("\"") + str() + "\"";
}

BooleanNode::BooleanNode(bool val) : value_(val) {}

Node *BooleanNode::Eval(State *state) const {
	return const_cast<BooleanNode *>(this); // boolean evaluates to itself
}

std::string BooleanNode::PPrint() const {
	return value_ ? "#t" : "#f";
}

} // namespace
