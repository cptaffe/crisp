
#include "lexer.h"
#include "channel.h"
#include "tree.h"

#include <sstream>
#include <cstdio>
#include <string>

std::string tcstr(enum crisp::Token::TokenCategory t) {
	using namespace crisp;
	switch (t) {
	case Token::kBeginParen:
		return "Begin Paren";
	case Token::kEndParen:
		return "End Paren";
	case Token::kIdent:
		return "Ident";
	case Token::kNum:
		return "Num";
	case Token::kString:
		return "String";
	case Token::kTick:
		return "Tick";
	case Token::kComment:
		return "Comment";
	case Token::kError:
		return "Error";
	}
}

int main() {
	crisp::Token *tok;
	std::stringstream str("#comment\n(define#this is a comment\na '(x 2 3))");
	crisp::InputScanner scanner(&str);
	crisp::Lexer lex(&scanner);

	while ((tok = lex.Next()) != nullptr) {
		printf("lexeme@%d:%d-> %-10s as %s\n", tok->pos().linenum + 1, tok->pos().chnum, ("'" + tok->lexeme() + "'").c_str(), tcstr(tok->category()).c_str());
		delete tok;
	}
}
