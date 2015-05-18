
#ifndef CRISP_PASS_H_
#define CRISP_PASS_H_

#include "tree.h"

#include <sstream>

namespace crisp {

class BuiltinInterface {
public:
	virtual ~BuiltinInterface() {}
	virtual NodeInterface *Apply(std::vector<NodeInterface *> *params) = 0;
	virtual std::vector<enum NodeInterface::Category> *Signature() = 0;
};

// (def name thing)
class Define : public BuiltinInterface {
public:
	Define(SymbolTable& t) : table(t) {}

	virtual NodeInterface *Apply(std::vector<NodeInterface *> *params) {
		// add an entry to the
		std::string str = static_cast<IdentNode *>((*params)[0])->GetIdentString();
		table.Put(str, (*params)[1]);
		return (*params)[0];
	}

	virtual std::vector<enum NodeInterface::Category> *Signature() {
		return &signature;
	}
private:
	SymbolTable& table;
	std::vector<enum NodeInterface::Category> signature = {NodeInterface::kIdent, NodeInterface::kOther};
};

class Builtins {
public:
	~Builtins() {
		for (auto i = builtins.begin(); i != builtins.end(); i++) {
			delete (*i).second;
		}
	}

	static bool CheckSignature(BuiltinInterface *b, std::vector<NodeInterface *> *params) {
		auto sig = b->Signature();
		if (sig->size() != params->size()) {
			return false; // signature may not mismatch in size.
		}

		auto j = sig->begin();
		for (auto i = params->begin(); i != params->end() && j != sig->end(); i++, j++) {
			if (*j == NodeInterface::kOther) {
				// anything will fit in an other
			} else if (*j != (*i)->GetCategory()) {
				return false;
			}
		}
		return true;
	}

	BuiltinInterface *Get(std::string str) {
		try {
			return builtins.at(str);
		} catch (...) {
			return nullptr;
		}
	}

	void Put(std::string str, BuiltinInterface *bf) {
		builtins[str] = bf;
	}

private:
	std::map<std::string, BuiltinInterface *> builtins;
};

class ExecutionState {
public:
	ExecutionState() {
		builtins.Put("def", new Define(GetSymbolTable()));
	}

	SymbolTable& GetSymbolTable() {
		return table;
	}

	Builtins& GetBuiltins() {
		return builtins;
	}

private:
	SymbolTable table;
	Builtins builtins;
};

class ExecutePass {
public:
	ExecutePass(ExecutionState& state) : state_(state) {}

	NodeInterface *Apply(NodeInterface *node) {
		if (node != nullptr) {
			if ((node->GetCategory() == NodeInterface::kList || node->GetCategory() == NodeInterface::kRoot) && !node->IsConstant()) {

				auto n = *static_cast<ParentNode *>(node)->ChildBegin();
				if (n != nullptr && n->GetCategory() == NodeInterface::kIdent && !n->IsConstant()) {
					// first child is identifier.
					// this means that this list is an active list.
					std::string id = static_cast<IdentNode *>(n)->GetIdentString();
					BuiltinInterface *builtin = state_.GetBuiltins().Get(id);
					if (builtin != nullptr) {
						std::vector<NodeInterface *> params;
						auto i = static_cast<ParentNode *>(node)->ChildBegin();
						i++;
						for (; i != static_cast<ParentNode *>(node)->ChildEnd(); i++) {
							params.push_back(*i);
						}
						if (Builtins::CheckSignature(builtin, &params)) {
							return builtin->Apply(&params);
						} else {
							std::stringstream str;
							str << "Improper call to " << id << ", wrong signature.";
							return new ErrorNode(str.str());
						}
					} else {
						std::stringstream str;
						str << "No such builtin function '" << id << "'";
						return new ErrorNode(str.str());
					}
				} else {
					// descend
					ParentNode *p = static_cast<ParentNode *>(node);
					for (auto i = p->ChildBegin(); i != p->ChildEnd(); i++) {
						*i = Apply(*i);
					}
				}
			}
		}
		return node;
	}
private:
	ExecutionState& state_;
};

} // namespace crisp

#endif // CRISP_PASS_H_
