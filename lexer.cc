
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lexer.h"

#include <utility>
#include <sstream>
#include <cctype>

using namespace crisp;

Lexer::Lexer(ScannerInterface *s) : mach(s) {}

namespace {

inline bool iswhitespace(char c) {
	// whitespace character or comment delimitor
	return c == ' ' || c == '\t' || c == '\n';
}

void *whitespace_start_state(State *s);
void *whitespace_sexp_state(State *s);
void *sexp_delim_state(State *s);
void *sexp_state(State *s);
void *id_state(State *s);
void *num_state(State *s);
void *tick_state(State *s);

// lex whitespace
void whitespace(State *s) {
	char c = s->scanner->Next();
	while (iswhitespace((c = s->scanner->Next()))) {
		// dump whitespace
	}
	s->scanner->Back(c);
}

void comment(State *s) {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	// lex #stuffstuff\n comment,
	// don't save delimiter '#' character
	while ((c = s->scanner->Next()) != '\n') {
		s->buf.push_back(c);
	}
	s->toks.push(new Token(Token::kComment, s->pos, s->buf));
	s->buf.clear();
}

void *whitespace_start_state(State *s) {
	whitespace(s);
	return (void *) start_state;
}

void *comment_start_state(State *s) {
	comment(s);
	return (void *) start_state;
}

void *whitespace_sexp_state(State *s) {
	whitespace(s);
	return (void *) sexp_state;
}

void *comment_sexp_state(State *s) {
	comment(s);
	return (void *) sexp_state;
}

void *sexp_delim_state(State *s) {
	char c = s->scanner->Next();
	if (c == '(') {
		s->toks.push(new Token(Token::kBeginParen, s->scanner->pos(), std::string(1, c)));
		s->paren_depth++;
		return (void *) sexp_state;
	} else if (c == ')') {
		s->toks.push(new Token(Token::kEndParen, s->scanner->pos(), std::string(1, c)));
		s->paren_depth--;
		if (s->paren_depth > 0) {
			return (void *) sexp_state;
		} else {
			return (void *) start_state;
		}
		return NULL;
	} else {
		// should never reach
		return NULL;
	}
}

void *sexp_state(State *s) {
	char c = s->scanner->Peek();
	if (c == '(' || c == ')') {
		return (void *) sexp_delim_state;
	} else if (iswhitespace(c)) {
		return (void *) whitespace_sexp_state;
	} else if (c == '#') {
		return (void *) comment_sexp_state;
	} else if (isalpha(c)) {
		return (void *) id_state;
	} else if (c >= '0' && c <= '9') {
		return (void *) num_state;
	} else if (c == '\'') {
		return (void *) tick_state;
	} else if (c == EOF) {
		std::stringstream str;
		str << "unexpected EOF";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return NULL;
	} else {
		std::stringstream str;
		str << "unexpected character '" << c << "'";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return NULL;
	}
}

void *id_state(State *s) {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	s->buf.push_back(c);
	while ((isalnum((c = s->scanner->Next())) || c == '_') && c != EOF) {
		s->buf.push_back(c);
	}
	s->scanner->Back(c);
	s->toks.push(new Token(Token::kIdent, s->pos, s->buf));
	s->buf.clear();
	return (void *) sexp_state;
}

void *num_state(State *s) {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	s->buf.push_back(c);
	while ((((c = s->scanner->Next()) >= '0' && c <= '9') || c == '.') && c != EOF) {
		s->buf.push_back(c);
	}
	s->scanner->Back(c);
	s->toks.push(new Token(Token::kNum, s->pos, s->buf));
	s->buf.clear();
	return (void *) sexp_state;
}

void *tick_state(State *s) {
	char c = s->scanner->Next();
	s->toks.push(new Token(Token::kTick, s->scanner->pos(), std::string(1, c)));
	return (void *) sexp_state;
}

} // namespace

// start state is exported
void *crisp::start_state(State *s) {
	char c = s->scanner->Peek();
	if (iswhitespace(c)) {
		return (void *) whitespace_start_state;
	} else if (c == '#') {
		return (void *) comment_sexp_state;
	} else if (c == '(') {
		return (void *) sexp_delim_state;
	} else if (c == EOF) {
		return NULL;
	} else {
		s->scanner->Next();
		std::stringstream str;
		str << "unexpected character '" << c << "'";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return NULL;
	}
}

Token *StateMachine::Next() {
	while (func != NULL && state.toks.empty()) {
		func = StateFunc(func(&state));
	}
	if (!state.toks.empty()) {
		Token *tok = state.toks.top(); // has tokens.
		state.toks.pop();
		return tok;
	} else {
		return nullptr;
	}
}

Token *Lexer::Next() {
	return mach.Next();
}
