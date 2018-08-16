// TestSuite::Suite.cpp
#include <iostream>
#include <cassert>
#include <cstddef>
#include "suite.h"

using namespace std;
using namespace TestSuite;

void Suite::addTest(Test* t) throw(TestSuiteError)
{
	if (t == 0)
	{
		throw TestSuiteError("Null test in Suite::addTest()");
	}
	else if (osptr && !t->getStream())
	{
		t->setStream(osptr);
	}
	tests.push_back(t);
	t->reset();
}

void Suite::addSuite(const Suite& s)
{
	for (size_t i = 0; i < s.tests.size(); ++i)
	{
		assert(s.tests[i]);
		addTest(s.tests[i]);
	}
}

void Suite::free()
{
	for (size_t i = 0; i < tests.size(); ++i)
	{
		delete tests[i];
		tests[i] = 0;
	}
}

void Suite::run()
{
	reset();
	for (size_t i = 0; i < tests.size(); ++i)
	{
		assert(tests[i]);
		tests[i]->run();
	}
}

long Suite::report() const
{
	if (osptr)
	{
		long totalFail = 0;
		*osptr << "Suite \"" << name << "\"\n======================" << endl;
		for (size_t i = 0; i < tests.size(); ++i)
		{
			assert(tests[i]);
			totalFail += tests[i]->report();
		}
		return totalFail;
	}
	else
	{
		return getNumFailed();
	}
}

long Suite::getNumPassed() const
{
	long totalPass = 0;
	for (size_t i = 0; i < tests.size(); ++i)
	{
		assert(tests[i]);
		totalPass += tests[i]->getNumPassed();
	}
	return totalPass;
}

long Suite::getNumFailed() const
{
	long totalFail = 0;
	for (size_t i = 0; i < tests.size(); ++i)
	{
		assert(tests[i]);
		totalFail += tests[i]->getNumFailed();
	}
	return totalFail;
}


void Suite::reset()
{
	for (size_t i = 0; i < tests.size(); ++i)
	{
		assert(tests[i]);
		tests[i]->reset();
	}
}





