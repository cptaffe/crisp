
#include "scan.h"

using namespace Crisp;

InputScanner::InputScanner(std::istream *stream) : is(stream) {}

bool InputScanner::empty() const {
	if (backs.empty()) {
		return is->eof();
	} else {
		return false;
	}
}

char InputScanner::next() {
	if (backs.empty()) {
		return is->get();
	} else {
		char c = backs.top();
		backs.pop();
		return c;
	}
}

void InputScanner::back(char c) {
	backs.push(c);
}

char InputScanner::peek() {
	char c = next();
	back(c);
	return c;
}
