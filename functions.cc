
#include "functions.h"

namespace crisp {

std::string DefineFunc::PPrint() const {
	return "{def}";
}

Node *DefineFunc::Call(std::vector<Node *>& params) {
	// add an entry to the symbol table.
	if (params.size() == 2) {
		auto id = dynamic_cast<IdentNode *>(params[0]);
		if (id) {
			std::string str = id->str();
			state->symbol_table()->Put(str, params[1]);
			return params[0];
		} else {
			return new ErrorNode(PPrint() + ": first argument must be identifier not '" + params[0]->PPrint() + "'");
		}
	} else {
		return new ErrorNode(PPrint() + ": takes two arguments not " + std::to_string(params.size()));
	}
}

LambdaFunc::Instance::Instance(Node::State::SymbolTableInterface *t, IdentNode *n, Node *expression) : table(t), name(n), exp(expression) {}

std::string LambdaFunc::Instance::PPrint() const {
	return "lambda";
}

Node *LambdaFunc::Instance::Call(std::vector<Node *>& params) {
	// Define these parameters against the ids in the
	// call to Lambda's 1st parameter (the function definition list).
	// this is done by looping over the two vectors simultaneosly.

	// localize data by creating a new symbol table.
	Node::State::SymbolTableInterface *symb = new Scope(table);

	if (params.size() != 1) {
		return new ErrorNode("lambdas accept one parameter");
	}

	symb->Put(name->str(), params[0]); // add definition to local symbol table

	// execute func_body with new symbol table.
	Node::State s(symb);
	return exp->Eval(&s);
}

Node *LambdaFunc::Call(std::vector<Node *>& params) {
	// this callable takes arguments to create a lambda,
	// then returns another callable that executes the lambda.
	if (params.size() == 2) {
		if (dynamic_cast<IdentNode *>(params[0])) {
			return new Instance(state->symbol_table(), static_cast<IdentNode *>(params[0]), params[1]);
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
