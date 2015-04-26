
#include "scan.h"
#include "test.h"

#include <cstdio>
#include <sstream>

class InputScannerTest : public Test {
public:
	InputScannerTest() {
		// create mapping of test description to test function.
		tests = {
			{"Next", []{
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
			}},

			{"Back", []{
				return true;
			}},

			{"Empty on Stream EOF", []{
				std::stringstream ss;
				Crisp::InputScanner scanner(&ss);

				if (scanner.empty()) { return false; }
				ss.get(); // set eof bit.
				if (!scanner.empty()) { return false; }
				return true;
			}},

			{"Empty on Back", []{
				std::stringstream ss;
				Crisp::InputScanner scanner(&ss);

				ss.get(); // set eof bit.
				// back a character, should not eof.
				scanner.back('c');
				if (scanner.empty()) { return false; }
				return true;
			}}
		};
	}
};

int main() {
	InputScannerTest test;
	auto res = test.test();
	int success = 0, failure = 0;
	for (auto i = res.begin(); i != res.end(); i++) {

		if (i->second) {
			success++;
		} else { failure++; }

		printf("%-20.20s %s\n", i->first.c_str(), i->second ? "\033[1;32mSuccess\033[0m" : "\033[1;31mFailure\033[0m");
	}
	printf("%.2f%% success\n", ((float) success/(success + failure)) * 100);
}
