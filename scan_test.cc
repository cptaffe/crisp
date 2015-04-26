
#include "scan.h"
#include "test.h"

#include <cstdio>
#include <sstream>

class InputScannerTest : public Test {
public:
	InputScannerTest();
private:
	// test functions
	static bool next();
	static bool back();
	static bool empty_with_stream_eof();
	static bool empty_on_back();
};

InputScannerTest::InputScannerTest() {
	// create mapping of test description to test function.
	tests = {
		{"Next", next},
		{"Back", back},
		{"Empty on Stream EOF", empty_with_stream_eof},
		{"Empty on Back", empty_on_back}
	};
}

bool InputScannerTest::next() {
	std::string test{"hello"};
	std::stringstream ss{test};
	Crisp::InputScanner scanner(&ss);

	for (auto i = test.begin(); i != test.end(); i++) {
		if (scanner.next() != *i) {
			return false;
		}
	}

	if (scanner.next() != EOF) {
		return false;
	}

	return true;
}

bool InputScannerTest::back() {
	return true;
}

bool InputScannerTest::empty_with_stream_eof() {
	std::stringstream ss;
	Crisp::InputScanner scanner(&ss);

	if (scanner.empty()) { return false; }
	ss.get(); // set eof bit.
	if (!scanner.empty()) { return false; }
	return true;
}

bool InputScannerTest::empty_on_back() {
	std::stringstream ss;
	Crisp::InputScanner scanner(&ss);

	ss.get(); // set eof bit.
	// back a character, should not eof.
	scanner.back('c');
	if (scanner.empty()) { return false; }
	return true;
}

int main() {
	InputScannerTest test;
	auto res = test.test();
	for (auto i = res.results.begin(); i != res.results.end(); i++) {
		printf("%s: %s\n", i->first.c_str(), i->second ? "Success" : "Failure");
	}
}
