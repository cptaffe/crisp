
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lexer.h"

#include <utility>
#include <sstream>
#include <cctype>

using namespace crisp;

Lexer::Lexer(ScannerInterface *s) : mach(s) {}

StateInterface *Ident::Next() {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	s->buf.push_back(c);
	while ((isalnum((c = s->scanner->Next())) || c == '_') && c != EOF) {
		s->buf.push_back(c);
	}
	s->scanner->Back(c);
	s->toks.push(new Token(Token::kIdent, s->pos, s->buf));
	s->buf.clear();
	return new SExpression(s);
}

StateInterface *Num::Next() {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	s->buf.push_back(c);
	while ((((c = s->scanner->Next()) >= '0' && c <= '9') || c == '.') && c != EOF) {
		s->buf.push_back(c);
	}
	s->scanner->Back(c);
	s->toks.push(new Token(Token::kNum, s->pos, s->buf));
	s->buf.clear();
	return new SExpression(s);
}

StateInterface *Tick::Next() {
	char c = s->scanner->Next();
	s->toks.push(new Token(Token::kTick, s->scanner->pos(), std::string(1, c)));
	return new SExpression(s);
}

StateInterface *SExpressionDelim::Next() {
	char c = s->scanner->Next();
	if (c == '(') {
		s->toks.push(new Token(Token::kBeginParen, s->scanner->pos(), std::string(1, c)));
		s->paren_depth++;
		return new SExpression(s);
	} else if (c == ')') {
		s->toks.push(new Token(Token::kEndParen, s->scanner->pos(), std::string(1, c)));
		s->paren_depth--;
		if (s->paren_depth > 0) {
			return new SExpression(s);
		} else {
			return new Start(s);
		}
		return nullptr;
	} else {
		// should never reach
		return nullptr;
	}
}

StateInterface *SExpression::Next() {
	char c = s->scanner->Peek();
	if (c == '(' || c == ')') {
		return new SExpressionDelim(s);
	} else if (Whitespace::Is(c)) {
		return new Whitespace(s, new SExpression(s));
	} else if (c == '#') {
		return new Comment(s, new SExpression(s));;
	} else if (isalpha(c)) {
		return new Ident(s);
	} else if (c >= '0' && c <= '9') {
		return new Num(s);
	} else if (c == '\'') {
		return new Tick(s);
	} else if (c == EOF) {
		std::stringstream str;
		str << "unexpected EOF";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return nullptr;
	} else {
		std::stringstream str;
		str << "unexpected character '" << c << "'";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return nullptr;
	}
}

StateInterface *Whitespace::Next() {
	char c = s->scanner->Next();
	while (Is((c = s->scanner->Next()))) {
		// dump whitespace
	}
	s->scanner->Back(c);
	return next;
}

StateInterface *Comment::Next() {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	// lex #stuffstuff\n comment,
	// don't save delimiter '#' character
	while ((c = s->scanner->Next()) != '\n') {
		s->buf.push_back(c);
	}
	s->toks.push(new Token(Token::kComment, s->pos, s->buf));
	s->buf.clear();
	return next;
}

StateInterface *Start::Next() {
	char c = s->scanner->Peek();
	if (Whitespace::Is(c)) {
		return new Whitespace(s, new Start(s));
	} else if (c == '#') {
		return new Comment(s, new Start(s));
	} else if (c == '(') {
		return new SExpressionDelim(s);
	} else if (c == EOF) {
		return nullptr;
	} else {
		s->scanner->Next();
		std::stringstream str;
		str << "unexpected character '" << c << "'";
		s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		return nullptr;
	}
}

Token *StateMachine::Next() {
	while (state != nullptr && s.toks.empty()) {
		StateInterface *next_state = state->Next();
		delete state;
		state = next_state;
	}
	if (!s.toks.empty()) {
		Token *tok = s.toks.top(); // has tokens.
		s.toks.pop();
		return tok;
	} else {
		return nullptr;
	}
}

Token *Lexer::Next() {
	return mach.Next();
}
