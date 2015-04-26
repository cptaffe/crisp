
#ifndef CRISP_SCAN_H_
#define CRISP_SCAN_H_

#include <string>
#include <istream>
#include <stack>

namespace Crisp {

class ScannerInterface {
public:
	// returns true if the scanner is at the end of its input.
	virtual bool empty() const = 0;

	// returns the next character or EOF.
	virtual char next() = 0;

	// Backs up one character of input.
	// This method may be called an arbitrary number of times.
	// After a call to back that is successful:
	// empty will return false.
	// next will return the last back'd character until there are no
	// back'd characters.
	virtual void back(char c) = 0;

	// peeks ahead one character.
	// this method is equivalent to calling next and then calling back.
	virtual char peek() = 0;
};

class InputScanner : public ScannerInterface {
public:
	InputScanner(std::istream *stream);

	// delted copy and move constructor.
	InputScanner(const InputScanner&) = delete;
	InputScanner(InputScanner&&) = delete;

	virtual bool empty() const;
	virtual char next();
	virtual void back(char c);
	virtual char peek();
private:
	std::stack<char> backs;
	std::istream *is;
};

} // namesapce Crisp

#endif // CRISP_SCAN_H_
