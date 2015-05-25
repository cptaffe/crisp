
#include "parser.h"

namespace crisp {
namespace parser {

Parser::Parser() {
	path.push_back(new RootNode());
}

void Parser::Put(Token *tok) {
	// begginning of list.
	if (tok->category() == Token::kBeginParen) {
		paren_count++;
		auto list = new ListNode();
		if (!path.empty()) {
			static_cast<ParentNode *>(path.back())->Put(list);
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
		Node *node = path.front();
		path.clear();
		path.push_back(node);
	} else if (tok->category() == Token::kComment) {
		// throw away comment.
	} else if (tok->category() == Token::kError) {
		// print error, attempt recovery (via ignoring).
		std::cout << ErrorNode(tok->lexeme()).PPrint();
	} else {
		static_cast<ParentNode *>(path.back())->Put([&]()->Node* {
			if (tok->category() == Token::kIdent) {
				return new IdentNode(tok->lexeme());
			} else if (tok->category() == Token::kNum) {
				return new NumNode(std::stoi(tok->lexeme()));
			} else if (tok->category() == Token::kString) {
				return new StringNode(tok->lexeme());
			} else {
				std::stringstream s;
				s << "Unknown token type '" << tok->str() << "'";
				return new ErrorNode(s.str());
			}
		}());
	}
}

} // namespace parser
} // namespace crisp
