// TestSuite::Test.h
#ifndef _TESTSUITE_TEST_H
#define _TESTSUITE_TEST_H
#include <iostream>
#include <string>
#include <cassert>

using std::string;
using std::ostream;
using std::cout;


#define test_(cond)\
	do_test(cond, #cond, __FILE__, __LINE__)
#define fail_(str)\
	do_fail(str, __FILE__, __LINE__)
	
namespace TestSuite {

class Test
{
	ostream* osptr;
	long nPass;
	long nFail;
	//Disallowed:
	Test(const Test&);
	Test& operator=(Test&);
protected:
	void do_test(bool cond, const string& lbl, const char* fname, long lineno);
	void do_fail(const string& lbl, const char* fname, long lineno);
public:
	Test(ostream* osptr = &cout)
	{
		this->osptr = osptr;
		nPass = 0;
		nFail = 0;
	}
	virtual ~Test() {}
    virtual void run() = 0;
    virtual void reset() { nPass = nFail = 0;}
	long getNumPassed() const { return nPass; }
	long getNumFailed() const { return nFail; }
	const ostream* getStream() const { return osptr; }
	void setStream(ostream* osptr) { this->osptr = osptr; }
    void succeed_() { ++nPass; }
	long report() const;

};	
	
}// namespace TestSuite
#endif
