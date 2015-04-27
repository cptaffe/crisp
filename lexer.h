
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_LEXER_H_
#define CRISP_LEXER_H_

#include "token.h"
#include "scanner.h"

#include <stack>

namespace crisp {

class LexerInterface {
public:
	// returns a new'd Token pointer or nullptr (on end).
	virtual Token *Next() = 0;
};

struct State {
public:
	State(ScannerInterface *s) : scanner(s) {}

	int paren_depth = 0;
	Position pos; // current token origin position.
	std::string buf;
	std::stack<Token *> toks;
	ScannerInterface *scanner;
};

typedef void *(* StateFunc)(State *);
void *start_state(State *s);

class StateMachine {
public:
	StateMachine(ScannerInterface *s) : state(s) {}
	Token *Next();
private:
	StateFunc func = &start_state;
	State state;
};

class Lexer : public LexerInterface {
public:
	Lexer(ScannerInterface *s);

	// returns next token.
	virtual Token *Next();
private:
	StateMachine mach;
};

} // namespace crisp

#endif // CRISP_LEXER_H_
