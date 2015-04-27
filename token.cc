
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

Position Token::pos() const {
	return pos_;
}
