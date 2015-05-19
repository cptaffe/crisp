
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lexer.h"

#include <utility>
#include <sstream>
#include <cctype>

using namespace crisp;

const std::string crisp::Ident::legal = "+-*/`~!@$%^&*_=|?\\;:<>,.";

Lexer::Lexer(ScannerInterface *s) : mach(s) {}

StateInterface *Ident::Next() {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	do {
		s->buf.push_back(c);
	} while (Is((c = s->scanner->Next())) && c != EOF);
	s->scanner->Back(c);
	s->toks.push(new Token(Token::kIdent, s->pos, s->buf));
	s->buf.clear();
	return new SExpression(s);
}

StateInterface *Num::Next() {
	char c = s->scanner->Next();
	s->pos = s->scanner->pos();
	do {
		s->buf.push_back(c);
	} while (Is((c = s->scanner->Next())) && c != EOF);
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
		return new SExpression(s);
	} else if (c == ']') {
		s->toks.push(new Token(Token::kEndAllParen, s->scanner->pos(), std::string(1, c)));
		s->paren_depth = 0;
		return new SExpression(s);
	} else {
		// should never reach
		return nullptr;
	}
}

StateInterface *SExpression::Next() {
	char c = s->scanner->Peek();
	if (SExpressionDelim::IsDelim(c)) {
		return new SExpressionDelim(s);
	} else if (Whitespace::IsDelim(c)) {
		return new Whitespace(s, new SExpression(s));
	} else if (Comment::IsDelim(c)) {
		return new Comment(s, new SExpression(s));
	} else if (Ident::IsDelim(c)) {
		return new Ident(s);
	} else if (Num::IsDelim(c)) {
		return new Num(s);
	} else if (Tick::IsDelim(c)) {
		return new Tick(s);
	} else if (String::IsDelim(c)) {
		return new String(s, new SExpression(s));
	} else if (c == EOF) {
		if (s->paren_depth > 0) {
			std::stringstream str;
			str << "unexpected EOF";
			s->toks.push(new Token(Token::kError, s->scanner->pos(), str.str()));
		}
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

StateInterface *String::Next() {
	char c = s->scanner->Next();
	char delim = c; // " or '
	s->pos = s->scanner->pos();
	// lex "stuff" sans ""
	while ((c = s->scanner->Next()) != delim) {
		s->buf.push_back(c);
	}
	s->toks.push(new Token(Token::kString, s->pos, s->buf));
	s->buf.clear();
	return next;
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

Token *Lexer::Get() {
	return mach.Next();
}
