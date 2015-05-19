
#ifndef CRISP_PARSER_H_
#define CRISP_PARSER_H_

#include "tree.h"

#include <vector>

#include <cstdio>
#include <sstream>
#include <string>
#include <iostream>

namespace crisp {

class Parser {
public:
	Parser() {
		path.push_back(new RootNode());
	}

	void Put(Token *tok) {

		bool constant = nextNodeIsConstant;
		nextNodeIsConstant = false;

		// begginning of list.
		if (tok->GetCategory() == Token::kBeginParen) {
			paren_count++;
			auto list = new ListNode(tok, constant);
			if (!path.empty()) {
				static_cast<ParentNode *>(path.back())->Put(list);
			}
			path.push_back(list); // descend tree
		} else if (tok->GetCategory() == Token::kEndParen) {
			paren_count--;
			if (paren_count < 0 || path.size() == 1) {
				if (paren_count < 0) {
					std::cout << "unmatched paren!" << std::endl;
				}
			} else {
				path.pop_back(); // ascend tree
			}
		} else if (tok->GetCategory() == Token::kEndAllParen) {
			paren_count = 0;
			NodeInterface *node = path.front();
			path.clear();
			path.push_back(node);
		} else if (tok->GetCategory() == Token::kTick) {
			nextNodeIsConstant = true;
		} else if (tok->GetCategory() == Token::kComment) {
			// throw away comment.
		} else if (tok->GetCategory() == Token::kError) {
			// print error, attempt recovery (via ignoring).
			std::cout << "Error: " << tok->GetLexeme() << std::endl;
		} else {
			NodeInterface *node;
			if (tok->GetCategory() == Token::kIdent) {
				node = new IdentNode(tok, constant);
			} else if (tok->GetCategory() == Token::kNum) {
				node = new NumNode(tok);
			} else if (tok->GetCategory() == Token::kString) {
				node = new StringNode(tok);
			} else {
				std::stringstream s;
				s << "Unknown token type '" << tok->String() << "'";
				node = new ErrorNode(s.str());
			}
			static_cast<ParentNode *>(path.back())->Put(node);
		}
	}

	NodeInterface *GetTree() const {
		if (!path.empty()) {
			return path[0];
		} else {
			return nullptr;
		}
	}
private:
	// path to current node.
	bool nextNodeIsConstant = false;
	int paren_count = 0;
	std::vector<NodeInterface *> path;
};

} // namespace crisp

#endif // CRISP_PARSER_H_
