#pragma once

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <string>

class Signals : public ::testing::Test
{
protected:
    // Code here will be called before any test is run
    static void SetUpTestCase();

    // Code here will be called after all tests have been run
    static void TearDownTestCase();

protected:
    // Code here will be called immediately after the constructor (right before each test).
    void SetUp() override;

    // Code here will be called immediately after each test (right before the destructor).
    void TearDown() override;
};

////////////////////////////////////////////////////////////////////////
/// Test types to be used as parameters
////////////////////////////////////////////////////////////////////////

struct TActionFunctor
{
public:
    TActionFunctor() {}
    virtual ~TActionFunctor() {}
    TActionFunctor(const TActionFunctor&) = default;

public:
    void operator()(const char* message) { spdlog::info("The functor message is: {}", message); }
};

class Foo
{
public:
    Foo() {}
    ~Foo() {}

    Foo(const Foo&) { spdlog::info("Foo copy"); }

public:
    void PrintSomething() { spdlog::info("Member method no arguments no return value!"); }

    void PassCopy(Foo) { spdlog::info("Member method copy passed"); }

    void PassRef(Foo&) { spdlog::info("Member method ref passed"); }

    void PassRef2(Foo&) { spdlog::info("Member method ref passed2"); }

    void PrintStringBinded(const char* message) { spdlog::info("The member binded function message is: {}", message); }

    void PrintString(const char* message) { spdlog::info("The member message is: {}", message); }

    void PrintStringRef(const std::string& message) { spdlog::info("The member message is: {}", message.c_str()); }

    void PrintStringRefConst(const std::string& message) const
    {
        spdlog::info("The member message is: {}", message.c_str());
    }

    void PrintStringConst(const char* message) const { spdlog::info("The member message is: {}", message); }

    void PrintStringConst(const char* message, const bool isTrue) const
    {
        spdlog::info("The member message is: {0}? {1}", message, isTrue ? "true" : "false");
    }

    bool MemberFunctionReturnBool()
    {
        spdlog::info("Member function returning true");
        return true;
    }

    static bool StaticFunctionReturnBool()
    {
        spdlog::info("Static function returning false");
        return false;
    }

    static void StaticPrintString(const char* message) { spdlog::info("The static message is: {}", message); }
};

////////////////////////////////////////////////////////////////////////
/// Test methods to be used as parameters
////////////////////////////////////////////////////////////////////////

void TestFunction()
{
    spdlog::info("Yeah! It's me! A function!");
}

bool FreeFunctionReturnBool()
{
    spdlog::info("Free function returning false");
    return false;
}

bool TestFunctionReturnAndPassing(const char* message)
{
    spdlog::info("{}", message);
    return true;
}

void FreeFunctionPrintStringRef(const std::string& message)
{
    spdlog::info("The free function with const message passed by ref prints: {}", message);
}

void FreeFunctionPrintString(const char* message)
{
    spdlog::info("The free function receiving a const char array prints: {}", message);
}

void FreeFunctionCopyObject(Foo)
{
    spdlog::info("Foo copy passed correctly");
}

void FreeFunctionRefObject(Foo&)
{
    spdlog::info("Foo ref passed correctly");
}

void FreeFunctionRefObject(Foo const&)
{
    spdlog::info("Foo const ref passed correctly");
}

void FreeFunctionPtrObject(Foo*)
{
    spdlog::info("Foo ptr passed correctly");
}

void FreeFunctionPtrObject(Foo const*)
{
    spdlog::info("Foo const ptr passed correctly");
}

const Foo& FreeFunctionReturnConstRef(Foo& foo)
{
    return foo;
}

const Foo* FreeFunctionReturnConstPtr(Foo& foo)
{
    return &foo;
}
