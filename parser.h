
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
	~Parser() {
		if (!path.empty()) {
			std::cout << ">> ";
			path.front()->PPrint(std::cout);
			std::cout << std::endl;
		}
	}

	void Put(Token *tok) {

		bool constant = nextNodeIsConstant;
		nextNodeIsConstant = false;

		// begginning of list.
		if (tok->GetCategory() == Token::kBeginParen) {
			paren_count++;
			auto list = new ListNode(tok, constant);
			if (!path.empty()) {
				static_cast<ParentNodeInterface *>(path.back())->Put(list);
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
			delete tok; // not used.
		} else if (tok->GetCategory() == Token::kComment) {
			// throw away comment.
			std::cout << "tossing comment: '" << tok->GetLexeme() << "'" << std::endl;
		} else {
			if (!path.empty()) {
				static_cast<ParentNodeInterface *>(path.back())->Put(NodeFactory::GetInstance()->CreateNode(tok, constant));
			} else {
				// unexpected token error.
				std::cout << "Unexpected Token '" << tok->String() << "' containing \"" << tok->GetLexeme() << "\"" << std::endl;
			}
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
