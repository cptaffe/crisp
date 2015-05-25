
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_TOK_H_
#define CRISP_TOK_H_

#include "position.h"

#include <string>

namespace crisp {

struct Token {
public:
	// possible Token categories.
	enum TokenCategory {
		kBeginParen,
		kEndParen,
		kEndAllParen,
		kIdent,
		kNum,
		kString,
		kTick,
		kComment,
		kError,
		kPossibleBreak,
	};

	Token(const enum TokenCategory c, const Position p, const std::string l);

	// returns token as string
	std::string str() const;
	std::string lexeme() const;
	enum TokenCategory category() const;
	Position pos() const;
private:
	std::string lexeme_;
	enum TokenCategory category_;
	Position pos_;
};

} // namespace crisp

#endif // CRISP_TOK_H_
