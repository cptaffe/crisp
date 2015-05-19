
#include "lexer.h"
#include "parser.h"
#include "channel.h"
#include "pass.h"

#include <sstream>
#include <future>

int main() {
	crisp::InputScanner scanner(&std::cin);
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
			delete tok;
		}
	}, &p, &chan);

	// TODO:
	// Third channel for trees of each statement.
	// Lexer passes kPossibleBreak (newline and/or paren-cout is 0)
	// Parser decides if it has a full statement (paren-count)

	lexf.wait();
	parsef.wait();

	crisp::NodeInterface *node = p.GetTree();

	crisp::ExecutionState exec;
	crisp::ExecutePass xpass(exec);

	node = xpass.Apply(node);

	if (node != nullptr) {
		std::cout << ">> " << node->PPrint() << std::endl;
	}

	std::cout << exec.symbol_table().PPrint();
}
