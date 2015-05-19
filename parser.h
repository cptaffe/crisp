
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

		// begginning of list.
		if (tok->GetCategory() == Token::kTick) {
			const_count++;
		} else {
			if (const_count > 0) {
				Node *n;
				for (int i = 0; i < const_count; i++) {
					n = new ConstNode();
					static_cast<ParentNode *>(path.back())->Put(n);
				}
				path.push_back(n);
			}

			if (tok->GetCategory() == Token::kBeginParen) {
				paren_count++;
				auto list = new ListNode(tok);
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
				Node *node = path.front();
				path.clear();
				path.push_back(node);
			} else if (tok->GetCategory() == Token::kComment) {
				// throw away comment.
			} else if (tok->GetCategory() == Token::kError) {
				// print error, attempt recovery (via ignoring).
				std::cout << ErrorNode(tok->GetLexeme()).PPrint();
			} else {
				Node *node;
				if (tok->GetCategory() == Token::kIdent) {
					node = new IdentNode(tok);
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

			if (const_count > 0) {
				// remove const from path.
				if (path.back()->category() != Node::kConst) {
					Node *n = path.back();
					path.pop_back();
					path.push_back(n);
				} else {
					path.pop_back();
				}
				const_count = 0;
			}
		}
	}

	Node *GetTree() const {
		if (!path.empty()) {
			return path[0];
		} else {
			return nullptr;
		}
	}
private:
	// path to current node.
	int const_count = 0;
	int paren_count = 0;
	std::vector<Node *> path;
};

} // namespace crisp

#endif // CRISP_PARSER_H_
