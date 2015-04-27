
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

class StateInterface {
public:
	virtual ~StateInterface() {}
	// run state and get next state.
	virtual StateInterface *Next() = 0;
};

class Ident : public StateInterface {
public:
	Ident(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class Num : public StateInterface {
public:
	Num(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class Tick : public StateInterface {
public:
	Tick(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class SExpressionDelim : public StateInterface {
public:
	SExpressionDelim(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class SExpression : public StateInterface {
public:
	SExpression(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class Whitespace : public StateInterface {
public:
	Whitespace(State *state, StateInterface *nextstate) :
		s(state), next(nextstate) {}
	static bool Is(char c) {
		// whitespace character
		return c == ' ' || c == '\t' || c == '\n';
	}
	virtual StateInterface *Next();
private:
	State *s;
	StateInterface *next;
};

class Comment : public StateInterface {
public:
	Comment(State *state, StateInterface *nextstate) :
		s(state), next(nextstate) {}
	virtual StateInterface *Next();
private:
	State *s;
	StateInterface *next;
};

class Start : public StateInterface {
public:
	Start(State *state): s(state) {}
	virtual StateInterface *Next();
private:
	State *s;
};

class StateMachine {
public:
	StateMachine(ScannerInterface *scanner) : s(scanner), state(new Start(&s)) {}
	~StateMachine() {
		delete state;
	}
	Token *Next();
private:
	State s;
	StateInterface *state;
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
