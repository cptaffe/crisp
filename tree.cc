
#include "tree.h"

namespace crisp {

Node *ListNode::Eval(State *state) const {
	// list nodes attempt to call the first atom
	// with the following atoms as parameters.
	if (children_.begin() != children_.end()) {
		// assure first atom is callable
		Node *callNode = (*children_.begin())->Eval(state);
		if (callNode->category() == Node::kCallable) {
			// now create a vector of parameters.
			std::vector<Node *> params;
			for (auto i = children_.begin(); i != children_.end(); i++) {
				// evaluate parameters once
				if (i != children_.begin()) {
					params.push_back((*i)->Eval(state));
				}
			}

			// execute call
			return static_cast<CallableNode *>(callNode)->Call(params);
		} else {
			return new ErrorNode(std::string("Active List: first atom must be Callable not '") + callNode->PPrint() + "'");
		}
	} else {
		// empty list evaluates to null.
		return new NullNode();
	}
}

Node::State::State() {
	symbol_table_.Put("def", new Define(symbol_table()));
	symbol_table_.Put("lambda", new Lambda(symbol_table()));
}

Define::Define(Node::State::SymbolTable& t) : table(t) {}

std::string Define::PPrint() const {
	return "Builtin<Define>";
}

Node *Define::Call(std::vector<Node *>& params) {
	// add an entry to the symbol table.
	std::string str = static_cast<IdentNode *>(params[0])->str();
	table.Put(str, params[1]);
	return params[0];
}

LambdaInstance::LambdaInstance(Node::State::SymbolTable& t, ListNode *f, Node *body) : table(t), func(f), func_body(body) {}

std::string LambdaInstance::PPrint() const {
	return "Builtin<Lambda-Instance>";
}

Node *LambdaInstance::Call(std::vector<Node *>& params) {
	// localize data by creating a new symbol table.
	Node::State::SymbolTable symb = table;

	// Define these parameters against the ids in the
	// call to Lambda's 1st parameter (the function definition list).
	// ...
	std::vector<Node *> children = func->children();

	{
		auto i = children.begin();
		auto j = params.begin();
		while (i != children.end() && j != params.end()) {
			if ((*i)->category() == Node::kIdent) {
				std::string str = static_cast<IdentNode *>(*i)->str();
				symb.Put(str, *j); // add definition to local symbol table
			} else {
				return new ErrorNode("Function misdefinition, non-ident in parameters list");
			}
			i++; j++;
		}

		if (i == children.end() && j == params.end()) {
			// execute func_body with new symbol table.
			Node::State s(symb);
			return func_body->Eval(&s);
		} else {
			return new ErrorNode(std::string((i == children.end()) ? "Too many" : "Too few") + " arguments for function call");
		}
	}
}

} // namespace
