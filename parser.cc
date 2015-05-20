
#include "parser.h"

namespace crisp {
namespace parser {

Parser::Parser() {
	path.push_back(new RootNode());
}

void Parser::Put(Token *tok) {
	// begginning of list.
	if (tok->GetCategory() == Token::kBeginParen) {
		paren_count++;
		auto list = new ListNode();
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
		static_cast<ParentNode *>(path.back())->Put([&]()->Node* {
			if (tok->GetCategory() == Token::kIdent) {
				return new IdentNode(tok->GetLexeme());
			} else if (tok->GetCategory() == Token::kNum) {
				return new NumNode(tok);
			} else if (tok->GetCategory() == Token::kString) {
				return new StringNode(tok);
			} else {
				std::stringstream s;
				s << "Unknown token type '" << tok->String() << "'";
				return new ErrorNode(s.str());
			}
		}());
	}
}

} // namespace parser
} // namespace crisp
