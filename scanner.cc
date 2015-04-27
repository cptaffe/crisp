
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "scanner.h"

using namespace crisp;

InputScanner::InputScanner(std::istream *stream) : is(stream) {}

bool InputScanner::Empty() const {
	if (backstack.empty()) {
		return is->eof();
	} else {
		return false;
	}
}

Position InputScanner::pos() const {
	return pos_;
}

char InputScanner::Next() {
	char next_char;
	if (backstack.empty()) {
		next_char = is->get();
	} else {
		next_char = backstack.top();
		backstack.pop();
	}
	if (next_char == '\n') {
		// line number rollover
		pos_.chnum = 0;
		pos_.linenum++;
	}
	return next_char;
}

void InputScanner::Back(char c) {
	if (c == '\n') {
		// the last line length is unknown,
		// so chnum is reset to 0 and linenum is decremented.
		pos_.chnum = 0;
		pos_.linenum--;
	}
	backstack.push(c);
}

char InputScanner::Peek() {
	char c = Next();
	Back(c);
	return c;
}
