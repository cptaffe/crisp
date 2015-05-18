
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

class ExecutionState {
public:
	ExecutionState() {
		// add preliminary definitions to the symbol table.
		table.Put("def", new Define(GetSymbolTable()));
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
			node->IsConstant()
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
			// definition is evaluated lazily.
			return EvaluateAtom(def);
		} else if (node->GetCategory() == NodeInterface::kRoot) {
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
					i++; // dismiss the first atom.
					for (; i != static_cast<ParentNode *>(node)->ChildEnd(); i++) {
						params.push_back(*i);
					}

					// execute call
					return call->Call(params);
				} else {
					std::stringstream s;
					s << "Active List: first atom must be Callable not " << Node::CategoryString(callNode->GetCategory()) << "(" << callNode->PPrint() << ")";
					return new ErrorNode(s.str());
				}
			} else {
				// NOTE: empty list evalutes to itself?
				// TODO: make this a NullNode or something.
				return node;
			}
		} else {
			return new ErrorNode("You have confused the interpreter");
		}
	}

	ExecutionState& state_;
};

} // namespace crisp

#endif // CRISP_PASS_H_
