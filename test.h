
#ifndef CRISP_TEST_H_
#define CRISP_TEST_H_

#include <utility> // std::function
#include <map>

// test results structure.
struct TestResult {
public:
	// mapping of test name to test result.
	std::map<const std::string, bool> results;
};

class Test {
public:
	virtual TestResult test();
protected:
	// vector of test functions
	std::map<std::string, std::function<bool()>> tests;
};

TestResult Test::test() {
	TestResult results{};

	// iterate through test mapping and store result in result mapping.
	for (auto i = tests.begin(); i != tests.end(); i++) {
		results.results[i->first] = i->second();
	}

	return results;
}

#endif // CRISP_TEST_H_
