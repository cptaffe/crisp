
#include "lexer.h"
#include "parser.h"
#include "channel.h"

#include <sstream>
#include <future>

using namespace crisp;

int main() {
	InputScanner scanner(&std::cin);
	lexer::Lexer lex(&scanner);
	parser::Parser p;
	Channel<Token *> chan(5);

	auto lexf = std::async(std::launch::async, [](lexer::Lexer *lex, Channel<Token *> *chan){
		Token *tok;
		while ((tok = lex->Get()) != nullptr) {
			// put item in channel
			chan->Put(tok);
		}
		chan->Kill();
	}, &lex, &chan);

	auto parsef = std::async(std::launch::async, [](parser::Parser *p, Channel<Token *> *chan){
		Token *tok;
		while (chan->Get(&tok)) {
			// put item in channel
			p->Put(tok);
			delete tok;
		}
	}, &p, &chan);

	// TODO:
	// Third channel for trees of each statement.
	// Lexer passes kPossibleBreak (newline and/or paren-cout is 0)
	// Parser decides if it has a full statement (paren-count)

	lexf.wait();
	parsef.wait();

	Node::State e;
	Node *node = p.GetTree()->Eval(&e);

	if (node != nullptr) {
		std::cout << ">> " << node->PPrint() << std::endl;
	}

	std::cout << e.symbol_table()->PPrint();
}
