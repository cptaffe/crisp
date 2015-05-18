
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
			delete tok; // not used
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
		} else if (tok->GetCategory() == Token::kError) {
			// print error, attempt recovery (via ignoring).
			std::cout << "Error: " << tok->GetLexeme() << std::endl;
		} else {
			static_cast<ParentNode *>(path.back())->Put(NodeFactory::GetInstance()->CreateNode(tok, constant));
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
