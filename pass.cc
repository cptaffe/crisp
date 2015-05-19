
#include "pass.h"

namespace crisp {

Define::Define(SymbolTable& t) : table(t) {}

std::string Define::PPrint() const {
	return "Builtin<Define>";
}

NodeInterface *Define::Call(std::vector<NodeInterface *>& params) {
	// add an entry to the symbol table.
	std::string str = static_cast<IdentNode *>(params[0])->str();
	table.Put(str, params[1]);
	return params[0];
}

LambdaInstance::LambdaInstance(SymbolTable& t, ListNode *f, NodeInterface *body) : table(t), func(f), func_body(body) {}

std::string LambdaInstance::PPrint() const {
	return "Builtin<Lambda-Instance>";
}

NodeInterface *LambdaInstance::Call(std::vector<NodeInterface *>& params) {
	// localize data by creating a new symbol table.
	SymbolTable symb = table;

	// Define these parameters against the ids in the
	// call to Lambda's 1st parameter (the function definition list).
	// ...
	auto fi = func->Begin();
	auto pi = params.begin();
	for (; fi != func->End() && pi != params.end(); fi++, pi++) {
		// iterate through entries.
		if ((*fi)->category() == NodeInterface::kIdent) {
			std::string str = static_cast<IdentNode *>(*fi)->str();
			symb.Put(str, *pi); // add definition to local symbol table
		} else {
			return new ErrorNode("Function misdefinition, non-ident in parameters list");
		}
	}

	if (!(pi == params.end() && fi == func->End())) {
		// the lists are not the same length.
		std::stringstream s;
		s << ((pi == params.end()) ? "Too many" : "Too few") << " arguments for function call";
		return new ErrorNode(s.str());
	} else {
		// execute func_body with new symbol table.
		ExecutionState s(symb);
		ExecutePass exec(s);
		return exec.Apply(func_body);
	}

	// TODO: define a LocalSymbolTable that uses a list instead
	// of a map and points back to a previous SymbolTableInterface
	// (if not null) to continue searching.

	return new NullNode();
}


} // namespace crisp
