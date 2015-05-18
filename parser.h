
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
	~Parser() {
		if (!path.empty()) {
			std::cout << ">> ";
			path.front()->PPrint(std::cout);
			std::cout << std::endl;
		}
	}

	void Put(Token *tok) {

		// begginning of list.
		if (tok->category() == Token::kBeginParen) {
			paren_count++;
			auto list = new ListNode(tok);
			if (!path.empty()) {
				path.back()->Put(list);
			}
			path.push_back(list); // descend tree
		} else if (tok->category() == Token::kEndParen) {
			paren_count--;
			if (paren_count < 0 || path.size() == 1) {
				if (paren_count < 0) {
					std::cout << "unmatched paren!" << std::endl;
				}
			} else {
				path.pop_back(); // ascend tree
			}
		} else if (tok->category() == Token::kEndAllParen) {
			paren_count = 0;
			NodeInterface *node = path.front();
			path.clear();
			path.push_back(node);
		} else if (tok->category() == Token::kComment) {
			// throw away comment.
			std::cout << "tossing comment: '" << tok->lexeme() << "'" << std::endl;
		} else {
			path.back()->Put(new Node(tok));
		}
	}
private:
	// path to current node.
	int paren_count = 0;
	std::vector<NodeInterface *> path;
};

} // namespace crisp

#endif // CRISP_PARSER_H_
