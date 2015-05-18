
#ifndef CRISP_PASS_H_
#define CRISP_PASS_H_

#include "tree.h"

namespace crisp {

// (def name thing)
class Define : public CallableNode {
public:
	Define(SymbolTable& t) : table(t) {}

	virtual std::string PPrint() const {
		return "Define";
	}

	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) {
		// add an entry to the symbol table.
		std::string str = static_cast<IdentNode *>(params[0])->GetIdentString();
		table.Put(str, params[1]);
		return params[0];
	}
private:
	SymbolTable& table;
};

class LambdaInstance : public CallableNode {
public:
	LambdaInstance(SymbolTable *t, ListNode *f, NodeInterface *body) : table(t), func(f), func_body(body) {}

	virtual std::string PPrint() const {
		return "Lambda-Instance";
	}

	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) {
		// localize data by creating a new symbol table.
		SymbolTable *symb = table->Copy();

		// add definitions for the local scope by using
		// a localized Define
		Define def(*symb);

		// Define these parameters against the ids in the
		// call to Lambda's 1st parameter (the function definition list).
		// ...

		// TODO: define a LocalSymbolTable that uses a list instead
		// of a map and points back to a previous SymbolTableInterface
		// (if not null) to continue searching.

		return new NullNode();
	}
private:
	SymbolTable *table;
	ListNode *func;
	NodeInterface *func_body;
};

class Lambda : public CallableNode {
public:
	Lambda(SymbolTable& t) : table(t) {}

	virtual std::string PPrint() const {
		return "Lambda";
	}

	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) {
		// this callable takes arguments to create a lambda,
		// then returns another callable that executes the lambda.
		if (params.size() == 2) {
			if (params[0]->GetCategory() == NodeInterface::kList) {
				return new LambdaInstance(&table, static_cast<ListNode *>(params[0]), params[1]);
			} else {
				return new ErrorNode("Lambda: first atom must be list of function parameters");
			}
		} else {
			return new ErrorNode("Lambda: takes two atoms");
		}
	}
private:
	SymbolTable& table;
};

class ExecutionState {
public:
	ExecutionState() {
		// add preliminary definitions to the symbol table.
		table.Put("def", new Define(GetSymbolTable()));
		table.Put("lambda", new Lambda(GetSymbolTable()));
	}

	SymbolTable& GetSymbolTable() {
		return table;
	}

private:
	SymbolTable table;
};

class ExecutePass {
public:
	ExecutePass(ExecutionState& state) : state_(state) {}

	NodeInterface *Apply(NodeInterface *node) {
		return EvaluateAtom(node);
	}
private:
	NodeInterface *EvaluateAtom(NodeInterface *node) {

		// TODO: remove Builtins and remove them with CallableNodes.
		// these CallableNodes will be lambdas and builtins.
		// builtins will just be stored in the symbol table.

		// TODO: don't delete any nodes, all evaluations should just
		// return new trees.

		if (
			(node->GetCategory() == NodeInterface::kIdent && node->IsConstant())
			|| node->GetCategory() == NodeInterface::kNum
			|| node->GetCategory() == NodeInterface::kString
			|| node->GetCategory() == NodeInterface::kCallable
		) {
			// constants and literals evaluate to themselves.
			return node;
		} else if (node->GetCategory() == NodeInterface::kIdent) {
			// idents evaluate to their definitions
			std::string id_name = static_cast<IdentNode *>(node)->GetIdentString();
			NodeInterface *def = state_.GetSymbolTable().Get(id_name);
			if (def == nullptr) {
				// no definition found!
				std::stringstream s;
				s << "Identifier '" << id_name << "' has not been defined";
				return new ErrorNode(s.str());
			} else {
				// definition is evaluated lazily.
				return EvaluateAtom(def);
			}
		} else if (
			node->GetCategory() == NodeInterface::kRoot
			|| (node->GetCategory() == NodeInterface::kList && node->IsConstant())) {
			auto root = static_cast<ParentNode *>(node);
			for (auto i = root->ChildBegin(); i != root->ChildEnd(); i++) {
				*i = EvaluateAtom(*i);
			}
			return root;
		} else if (node->GetCategory() == NodeInterface::kList) {
			// active list
			NodeInterface *callNode = *(static_cast<ParentNode *>(node)->ChildBegin());
			if (callNode != nullptr) {
				// evaluate atom to call
				// assure it is a CallableNode.
				callNode = EvaluateAtom(callNode);
				if (callNode->GetCategory() == NodeInterface::kCallable) {
					CallableNode *call = static_cast<CallableNode *>(callNode);

					// now create a vector of parameters.
					std::vector<NodeInterface *> params;
					auto i = static_cast<ParentNode *>(node)->ChildBegin();
					if (i != static_cast<ParentNode *>(node)->ChildEnd()) {
						i++; // dismiss the first atom.
						for (; i != static_cast<ParentNode *>(node)->ChildEnd(); i++) {

							params.push_back(*i);
						}
					}

					// execute call
					return call->Call(params);
				} else {
					std::stringstream s;
					s << "Active List: first atom must be Callable not '" << callNode->PPrint() << "'";
					return new ErrorNode(s.str());
				}
			} else {
				// empty list is a NullNode.
				return new NullNode();
			}
		} else {
			return new ErrorNode("You have confused the interpreter");
		}
	}

	ExecutionState& state_;
};

} // namespace crisp

#endif // CRISP_PASS_H_
