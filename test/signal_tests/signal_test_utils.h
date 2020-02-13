#pragma once

#include <stdio.h>
#include <string>

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
    void operator()(const char* message) { printf("The functor message is: %s\n", message); }
};

class Foo
{
public:
    Foo() {}
    ~Foo() {}

    Foo(const Foo&) { printf("Foo copy\n"); }

public:
    void PrintSomething() { printf("Member method no arguments no return value!\n"); }

    void PassCopy(Foo) { printf("Member method copy passed\n"); }

    void PassRef(Foo&) { printf("Member method ref passed\n"); }

    void PassRef2(Foo&) { printf("Member method ref passed2\n"); }

    void PrintStringBinded(const char* message) { printf("The member binded function message is: %s\n", message); }

    void PrintString(const char* message) { printf("The member message is: %s\n", message); }

    void PrintStringRef(const std::string& message) { printf("The member message is: %s\n", message.c_str()); }

    void PrintStringRefConst(const std::string& message) const
    {
        printf("The member message is: %s\n", message.c_str());
    }

    void PrintStringConst(const char* message) const { printf("The member message is: %s\n", message); }

    void PrintStringConst(const char* message, const bool isTrue) const
    {
        printf("The member message is: %s? %s \n", message, isTrue ? "true" : "false");
    }

    bool MemberFunctionReturnBool()
    {
        printf("Member function returning true\n");
        return true;
    }

    static bool StaticFunctionReturnBool()
    {
        printf("Static function returning false\n");
        return false;
    }

    static void StaticPrintString(const char* message) { printf("The static message is: %s\n", message); }
};

////////////////////////////////////////////////////////////////////////
/// Test methods to be used as parameters
////////////////////////////////////////////////////////////////////////

void TestFunction()
{
    printf("Yeah! It's me! A function!\n");
}

bool FreeFunctionReturnBool()
{
    printf("Free function returning false\n");
    return false;
}

bool TestFunctionReturnAndPassing(const char* message)
{
    printf("%s\n", message);
    return true;
}

void FreeFunctionPrintStringRef(const std::string& message)
{
    printf("The free function with const message passed by ref prints: %s\n", message.c_str());
}

void FreeFunctionPrintString(const char* message)
{
    printf("The free function receiving a const char array prints: %s\n", message);
}

void FreeFunctionCopyObject(Foo)
{
    printf("Foo copy passed correctly\n");
}

void FreeFunctionRefObject(Foo&)
{
    printf("Foo ref passed correctly\n");
}

void FreeFunctionRefObject(Foo const&)
{
    printf("Foo const ref passed correctly\n");
}

void FreeFunctionPtrObject(Foo*)
{
    printf("Foo ptr passed correctly\n");
}

void FreeFunctionPtrObject(Foo const*)
{
    printf("Foo const ptr passed correctly\n");
}

const Foo& FreeFunctionReturnConstRef(Foo& foo)
{
    return foo;
}

const Foo* FreeFunctionReturnConstPtr(Foo& foo)
{
    return &foo;
}
