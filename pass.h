
#ifndef CRISP_PASS_H_
#define CRISP_PASS_H_

#include "tree.h"

namespace crisp {

class PassInterface {
public:
	virtual ~PassInterface() {}
	virtual void Apply(NodeInterface *root) = 0;
};

namespace {

} // namespace

// find all 'defun' calls and remove them,
// placing their values in the symboltable.
class ExecutePass : public PassInterface{
public:
	ExecutePass(SymbolTable& t) : table(t) {}

	virtual void Apply(NodeInterface *root) {
		if (root != nullptr) {
			if (root->GetCategory() == NodeInterface::kList) {
				Node Interface *node;
				for (int i = 0; (node = root->GetChild(i)) != nullptr; i++) {
					// branches may not execute in order.
					std::async([this](NodeInterface *node){
						Apply(node);
					}, node);
				}
			} else if (root->GetCategory() == NodeInterface::kIdent) {
				// found identifier, check if it's in the symbol table.
				table.Get(root->Get)
			}
		}
	}
private:
	SymbolTable& table;
};

} // namespace crisp

#endif // CRISP_PASS_H_
