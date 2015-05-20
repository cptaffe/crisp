
#include "functions.h"

namespace crisp {

std::string DefineFunc::PPrint() const {
	return "{def}";
}

Node *DefineFunc::Call(std::vector<Node *>& params) {
	// add an entry to the symbol table.
	std::string str = static_cast<IdentNode *>(params[0])->str();
	state->symbol_table()->Put(str, params[1]);
	return params[0];
}

LambdaFunc::Instance::Instance(Node::State::SymbolTableInterface *t, ListNode *f, Node *body) : table(t), func(f), func_body(body) {}

std::string LambdaFunc::Instance::PPrint() const {
	return "lambda";
}

Node *LambdaFunc::Instance::Call(std::vector<Node *>& params) {
	// Define these parameters against the ids in the
	// call to Lambda's 1st parameter (the function definition list).
	// this is done by looping over the two vectors simultaneosly.

	// localize data by creating a new symbol table.
	Node::State::SymbolTableInterface *symb = new Scope(table);
	std::vector<Node *> children = func->children();

	if (children.size() != params.size()) {
		return new ErrorNode(std::string((params.size() > children.size()) ? "Too many" : "Too few") + " arguments for function call");
	}

	auto i = children.begin();
	auto j = params.begin();
	while (i != children.end() && j != params.end()) {
		if (dynamic_cast<IdentNode *>(*i)) {
			std::string str = static_cast<IdentNode *>(*i)->str();
			symb->Put(str, *j); // add definition to local symbol table
		} else {
			return new ErrorNode("Function misdefinition, non-ident in parameters list");
		}
		i++; j++;
	}

	// execute func_body with new symbol table.
	Node::State s(symb);
	return func_body->Eval(&s);
}

Node *LambdaFunc::Call(std::vector<Node *>& params) {
	// this callable takes arguments to create a lambda,
	// then returns another callable that executes the lambda.
	if (params.size() == 2) {
		if (dynamic_cast<ListNode *>(params[0])) {
			return new Instance(state->symbol_table(), static_cast<ListNode *>(params[0]), params[1]);
		} else {
			return new ErrorNode(std::string("Lambda: first atom must be List, not '") + params[0]->PPrint() + "'");
		}
	} else {
		return new ErrorNode(PPrint() + " takes two atoms");
	}
}

Node *NotFunc::Call(std::vector<Node *>& params) {
	if (params.size() == 1) {
		return (!(params[0]->Eval(state))->isTrue()) ? new BooleanNode(true) : new BooleanNode(false);
	} else {
		return new ErrorNode(PPrint() + " takes one atom");
	}
}

Node *QuoteFunc::Call(std::vector<Node *>& params) {
	if (params.size() != 1) {
		return new ErrorNode(PPrint() + " takes one atom");
	} else {
		// quote (do not evaluate)
		return params[0];
	}
}

} // namespace crisp
