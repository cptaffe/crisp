
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "token.h"

using namespace crisp;

Token::Token(const enum TokenCategory c, const Position p, const std::string l) : category_(c), pos_(p), lexeme_(l) {}

std::string Token::lexeme() const {
	return lexeme_;
}

enum Token::TokenCategory Token::category() const {
	return category_;
}

std::string Token::String() const {
	switch (category_) {
	case Token::kBeginParen:
		return "Begin Paren";
	case Token::kEndParen:
		return "End Paren";
	case Token::kEndAllParen:
		return "End All Paren";
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

Position Token::pos() const {
	return pos_;
}
