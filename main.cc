
#include "lexer.h"
#include "parser.h"
#include "channel.h"

#include <sstream>
#include <future>

int main() {
	crisp::InputScanner scanner(&std::cout);
	crisp::Lexer lex(&scanner);
	crisp::Parser p;
	crisp::Channel<crisp::Token *> chan(5);

	auto lexf = std::async([](crisp::Lexer *lex, crisp::Channel<crisp::Token *> *chan){
		crisp::Token *tok;
		while ((tok = lex->Get()) != nullptr) {
			chan->Put(tok);
		}
		chan->Kill();
	}, &lex, &chan);

	auto parsef = std::async([](crisp::Parser *p, crisp::Channel<crisp::Token *> *chan){
		crisp::Token *tok;
		while (chan->Get(&tok)) {
			p->Put(tok);
		}
	}, &p, &chan);
}
