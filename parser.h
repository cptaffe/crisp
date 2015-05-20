
#ifndef CRISP_PARSER_H_
#define CRISP_PARSER_H_

#include "tree.h"

#include <vector>

#include <cstdio>
#include <sstream>
#include <string>
#include <iostream>

namespace crisp {
namespace parser {

class Parser {
public:
	Parser();
	void Put(Token *tok);
	Node *GetTree() const {
		return path.empty() ? nullptr : path[0];
	}
private:
	// path to current node.
	int const_count = 0;
	int paren_count = 0;
	std::vector<Node *> path;
};

} // namespace parser
} // namespace crisp

#endif // CRISP_PARSER_H_
