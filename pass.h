
#ifndef CRISP_PASS_H_
#define CRISP_PASS_H_

#include "tree.h"

namespace crisp {

// (def name thing)
class Define : public CallableNode {
public:
	Define(SymbolTable& t);
	virtual std::string PPrint() const;
	virtual NodeInterface *Call(std::vector<NodeInterface *>& params);
private:
	SymbolTable& table;
};

class LambdaInstance : public CallableNode {
public:
	LambdaInstance(SymbolTable& t, ListNode *f, NodeInterface *body);
	virtual std::string PPrint() const;
	virtual NodeInterface *Call(std::vector<NodeInterface *>& params);
private:
	SymbolTable& table;
	ListNode *func;
	NodeInterface *func_body;
};

class Lambda : public CallableNode {
public:
	Lambda(SymbolTable& t) : table(t) {}

	virtual std::string PPrint() const {
		return "Builtin<Lambda>";
	}

	virtual NodeInterface *Call(std::vector<NodeInterface *>& params) {
		// this callable takes arguments to create a lambda,
		// then returns another callable that executes the lambda.
		if (params.size() == 2) {
			if (params[0]->category() == NodeInterface::kList) {
				return new LambdaInstance(table, static_cast<ListNode *>(params[0]), params[1]);
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
		symbol_table_.Put("def", new Define(symbol_table()));
		symbol_table_.Put("lambda", new Lambda(symbol_table()));
	}

	ExecutionState(SymbolTable s) : symbol_table_(s) {}

	SymbolTable& symbol_table() {
		return symbol_table_;
	}

private:
	SymbolTable symbol_table_;
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

		auto cat = node->category();

		if (
			(cat == NodeInterface::kIdent && static_cast<IdentNode *>(node)->constant())
			|| cat == NodeInterface::kNum
			|| cat == NodeInterface::kString
			|| cat == NodeInterface::kCallable
		) {
			// constants and literals evaluate to themselves.
			return node;
		} else if (cat == NodeInterface::kIdent) {

			// idents evaluate to their definitions
			std::string id_name = static_cast<IdentNode *>(node)->str();
			NodeInterface *def = state_.symbol_table().Get(id_name);

			if (def == nullptr) {
				// no definition found!
				std::stringstream s;
				s << "Identifier '" << id_name << "' has not been defined";
				return new ErrorNode(s.str());
			} else {
				// definition is evaluated lazily.
				return EvaluateAtom(def);
			}
		} else if (cat == NodeInterface::kRoot || (cat == NodeInterface::kList && static_cast<ListNode *>(node)->constant())) {
			auto root = static_cast<ParentNode *>(node);
			for (auto i = root->Begin(); i != root->End(); i++) {
				*i = EvaluateAtom(*i);
			}
			return root;
		} else if (cat == NodeInterface::kList) {
			// active list
			NodeInterface *callNode = *(static_cast<ParentNode *>(node)->Begin());
			if (callNode != nullptr) {
				// evaluate atom to call
				// assure it is a CallableNode.
				callNode = EvaluateAtom(callNode);
				if (callNode->category() == NodeInterface::kCallable) {
					CallableNode *call = static_cast<CallableNode *>(callNode);

					// now create a vector of parameters.
					std::vector<NodeInterface *> params;
					auto i = static_cast<ParentNode *>(node)->Begin();
					if (i != static_cast<ParentNode *>(node)->End()) {
						i++; // dismiss the first atom.
						for (; i != static_cast<ParentNode *>(node)->End(); i++) {

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
