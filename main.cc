
#include "lexer.h"

#include <sstream>
#include <cstdio>

int main() {
	crisp::Token *tok;
	std::stringstream str("(x 1. 2)");
	crisp::InputScanner scanner(&str);
	crisp::Lexer lex(&scanner);

	while ((tok = lex.Next()) != nullptr) {
		printf("lexeme@%d:%d-> '%s'\n", tok->pos().linenum + 1, tok->pos().chnum + 1, tok->lexeme().c_str());
	}
}
