
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_SCAN_H_
#define CRISP_SCAN_H_

#include "position.h"

#include <string>
#include <istream>
#include <stack>

namespace crisp {

class ScannerInterface {
public:
	// returns true if the scanner is at the end of its input.
	virtual bool Empty() const = 0;

	// returns the next character or EOF.
	virtual char Next() = 0;

	// Backs up one character of input.
	// This method may be called an arbitrary number of times.
	// After a call to back that is successful:
	// empty will return false.
	// next will return the last back'd character until there are no
	// back'd characters.
	virtual void Back(char c) = 0;

	// peeks ahead one character.
	// this method is equivalent to calling next and then calling back.
	virtual char Peek() = 0;

	// returns the current position in the text.
	virtual Position pos() const = 0;
};

class InputScanner : public ScannerInterface {
public:
	InputScanner(std::istream *stream);

	// delted copy and move constructor.
	InputScanner(const InputScanner&) = delete;
	InputScanner(InputScanner&&) = delete;

	virtual bool Empty() const;
	virtual Position pos() const;
	virtual char Next();
	virtual void Back(char c);
	virtual char Peek();
private:
	Position pos_;
	std::stack<char> backstack;
	std::istream *is;
};

} // namesapce crisp

#endif // CRISP_SCAN_H_
