
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
	virtual Token *Get() = 0;
};

struct SharedStateData {
public:
	SharedStateData(ScannerInterface *s) : scanner(s) {}

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

// Patterns:
// IsDelim returns if a character could be the first
// character in that state.
// Is returns if a character could be a non-first character
// and is generally used by the class only.

class Ident : public StateInterface {
public:
	Ident(SharedStateData *state): s(state) {}
	// is initial character of ident
	static bool IsDelim(char c) {
		return Is(c);
	}
	virtual StateInterface *Next();
private:
	// is ident character
	static bool Is(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || legal.find(c) != std::string::npos;
	}

	static const std::string legal;
	SharedStateData *s;
};

class Num : public StateInterface {
public:
	Num(SharedStateData *state): s(state) {}

	// initial character for a number
	static bool IsDelim(char c) {
		return (c >= '0' && c <= '9');
	}

	virtual StateInterface *Next();
private:
	// is a numeric character
	static bool Is(char c) {
		return (c >= '0' && c <= '9') || c == '_';
	}

	SharedStateData *s;
};

class Tick : public StateInterface {
public:
	Tick(SharedStateData *state): s(state) {}

	static bool IsDelim(char c) {
		return c == '\'';
	}

	virtual StateInterface *Next();
private:
	SharedStateData *s;
};

class SExpressionDelim : public StateInterface {
public:
	SExpressionDelim(SharedStateData *state): s(state) {}

	static bool IsDelim(char c) {
		return c == '(' || c == ')' || c == ']';
	}

	virtual StateInterface *Next();
private:
	SharedStateData *s;
};

class SExpression : public StateInterface {
public:
	SExpression(SharedStateData *state): s(state) {}
	virtual StateInterface *Next();
private:
	SharedStateData *s;
};

class Whitespace : public StateInterface {
public:
	Whitespace(SharedStateData *state, StateInterface *nextstate) :
		s(state), next(nextstate) {}

	static bool IsDelim(char c) {
		return Is(c);
	}

	virtual StateInterface *Next();
private:
	// is a whitespae character
	static bool Is(char c) {
		// whitespace character
		return c == ' ' || c == '\t' || c == '\n';
	}

	SharedStateData *s;
	StateInterface *next;
};

class Comment : public StateInterface {
public:
	Comment(SharedStateData *state, StateInterface *nextstate) :
		s(state), next(nextstate) {}

	// initial comment character
	static bool IsDelim(char c) {
		return c == '#';
	}

	virtual StateInterface *Next();
private:
	SharedStateData *s;
	StateInterface *next;
};

class String : public StateInterface {
public:
	String(SharedStateData *state, StateInterface *nextstate) :
		s(state), next(nextstate) {}

	// initial string character
	static bool IsDelim(char c) {
		return c == '"';
	}

	virtual StateInterface *Next();
private:
	SharedStateData *s;
	StateInterface *next;
};

class StateMachine {
public:
	StateMachine(ScannerInterface *scanner) : s(scanner), state(new SExpression(&s)) {}
	~StateMachine() {
		delete state;
	}
	Token *Next();
private:
	SharedStateData s;
	StateInterface *state;
};

class Lexer : public LexerInterface {
public:
	Lexer(ScannerInterface *s);

	// returns next token.
	virtual Token *Get();
private:
	StateMachine mach;
};

} // namespace crisp

#endif // CRISP_LEXER_H_
