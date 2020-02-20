/*
 The MIT License (MIT)

 Copyright (c) 2016 Jorge López González

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * test_signals.cpp
 *
 *  Created on: 9 de oct. de 2016
 *      Author: jorge
 */

#include "test_signals.h"

#include <signals/signal.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <functional>
#include <utility>

void Signals::SetUpTestCase()
{
    auto console = spdlog::stdout_logger_mt("console");
    spdlog::set_default_logger(console);
}

void Signals::TearDownTestCase()
{
}

void Signals::SetUp()
{
}

void Signals::TearDown()
{
}

TEST_F(Signals, AutoreleaseConnections)
{
}

TEST_F(Signals, ConnectingFunctions)
{
    spdlog::info("+ INIT HOW TO USE SIGNALS");

    Foo foo;

    dc::CSignal<void(const char*)> signal;

    // Connecting binded functions
    // - with no parameter binded
    auto functionBindedWithoutBindedParams = std::bind(&Foo::PrintString, &foo, std::placeholders::_1);
    signal.Connect(functionBindedWithoutBindedParams);

    // - with binded parameter
    auto functionBindedWithParams = std::bind(&Foo::PrintString, &foo, "Binded string");
    signal.Connect(functionBindedWithParams);

    // Connecting lambda
    std::function<void(const char*)> stdFunctionParameter = [](const char* message) {
        spdlog::info("The std::function message: {}", message);
    };
    signal.Connect(stdFunctionParameter);

    // Connecting Functor
    TActionFunctor functor;
    signal.Connect(functor);

    // Connecting static function
    signal.Connect(&Foo::StaticPrintString);

    // Connecting free function
    signal.Connect(&FreeFunctionPrintString);

    // Connecting member function
    signal.Connect(&foo, &Foo::PrintString);

    // Connecting const member function
    signal.Connect(&foo, &Foo::PrintStringConst);

    EXPECT_EQ(signal.Count(), 8);

    // Emission of signal
    EXPECT_TRUE(signal("Hello world!"));

    // Disconnecting binded functions
    // - with no parameter binded
    signal.Disconnect(functionBindedWithoutBindedParams);
    EXPECT_EQ(signal.Count(), 7);

    // - with binded parameter
    signal.Disconnect(functionBindedWithParams);
    EXPECT_EQ(signal.Count(), 6);

    // Disconnecting member function
    signal.Disconnect(&foo, &Foo::PrintString);
    EXPECT_EQ(signal.Count(), 5);

    // Disconnecting static function
    signal.Disconnect(&Foo::StaticPrintString);
    EXPECT_EQ(signal.Count(), 4);

    // Disconnecting free function
    signal.Disconnect(&FreeFunctionPrintString);
    EXPECT_EQ(signal.Count(), 3);

    // Disconnecting lambda
    signal.Disconnect(stdFunctionParameter);
    EXPECT_EQ(signal.Count(), 2);

    // Disconnecting Functor
    signal.Disconnect(functor);
    EXPECT_EQ(signal.Count(), 1);

    // Disconnecting const member function
    signal.Disconnect(&foo, &Foo::PrintStringConst);
    EXPECT_TRUE(signal.Empty());

    // Emission of signal
    spdlog::info("+ EMITTING THE SIGNAL AFTER DISCONNECTING (nothing should happen)\n");

    EXPECT_TRUE(signal("Hello"));
}

TEST_F(Signals, Copy)
{
    Foo foo;
    dc::CSignal<void(Foo&)> signal;

    signal.Connect(&FreeFunctionRefObject);
    signal.Connect(&foo, &Foo::PassRef);

    ASSERT_EQ(signal.Count(), 2u);

    dc::CSignal<void(Foo&)> initiallyEmptySignal = signal;
    ASSERT_EQ(initiallyEmptySignal.Count(), 2u);

    dc::CSignal<void(Foo&)> initiallyWithConnectionsSignal;
    initiallyWithConnectionsSignal.Connect(&FreeFunctionRefObject);
    initiallyWithConnectionsSignal.Connect(&foo, &Foo::PassRef2);
    ASSERT_EQ(initiallyWithConnectionsSignal.Count(), 2u);
    initiallyWithConnectionsSignal = signal;

    ASSERT_TRUE(signal(foo));
    ASSERT_TRUE(initiallyEmptySignal(foo));
    ASSERT_TRUE(initiallyWithConnectionsSignal(foo));
}

TEST_F(Signals, Move)
{
    Foo foo;
    dc::CSignal<void(Foo&)> signal;

    signal.Connect(&FreeFunctionRefObject);
    signal.Connect(&foo, &Foo::PassRef);

    ASSERT_EQ(signal.Count(), 2u);

    dc::CSignal<void(Foo&)> initiallyEmptySignal;
    ASSERT_TRUE(initiallyEmptySignal.Empty());

    initiallyEmptySignal = std::move(signal);
    ASSERT_EQ(initiallyEmptySignal.Count(), 2u);
    ASSERT_TRUE(signal.Empty());

    dc::CSignal<void(Foo&)> initiallyWithConnectionsSignal;
    initiallyWithConnectionsSignal.Connect(&FreeFunctionRefObject);
    initiallyWithConnectionsSignal.Connect(&foo, &Foo::PassRef2);
    ASSERT_EQ(initiallyWithConnectionsSignal.Count(), 2u);
    initiallyWithConnectionsSignal = std::move(initiallyEmptySignal);
    ASSERT_EQ(initiallyWithConnectionsSignal.Count(), 2u);
    ASSERT_TRUE(initiallyEmptySignal.Empty());

    ASSERT_TRUE(signal(foo));
    ASSERT_TRUE(initiallyEmptySignal(foo));
    ASSERT_TRUE(initiallyWithConnectionsSignal(foo));
}

TEST_F(Signals, PassingRefParameter)
{
    Foo foo;
    dc::CSignal<void(const std::string&)> signal;

    signal.Connect(&FreeFunctionPrintStringRef);
    EXPECT_EQ(signal.Count(), 1);

    // Execution
    const std::string paramByRef = "Text by reference";
    EXPECT_TRUE(signal(paramByRef));

    signal.Disconnect(&FreeFunctionPrintStringRef);
    EXPECT_TRUE(signal.Empty());

    signal.Connect(&foo, &Foo::PrintStringRefConst);
    EXPECT_EQ(signal.Count(), 1);

    // Execution
    const std::string paramByRefConstMethod = "Text by reference in a const method";
    EXPECT_TRUE(signal(paramByRefConstMethod));

    signal.Disconnect(&foo, &Foo::PrintStringRefConst);
    EXPECT_TRUE(signal.Empty());
}

TEST_F(Signals, PassingCopyParameter)
{
    Foo foo;
    dc::CSignal<void(Foo)> signal;

    signal.Connect(&FreeFunctionCopyObject);
    EXPECT_EQ(signal.Count(), 1);

    EXPECT_TRUE(signal(foo));

    signal.Disconnect(&FreeFunctionCopyObject);
    EXPECT_TRUE(signal.Empty());

    signal.Connect(&foo, &Foo::PassCopy);
    EXPECT_EQ(signal.Count(), 1);

    EXPECT_TRUE(signal(foo));

    signal.Disconnect(&foo, &Foo::PassCopy);
    EXPECT_TRUE(signal.Empty());
}

template <typename ListType>
void checkArrayValidity(const ListType& valueList, const bool* expectedValuesArray, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        const bool returnedValue = valueList[i];
        const bool expectedValue = expectedValuesArray[i];

        EXPECT_EQ(returnedValue, expectedValue);

        spdlog::info("Returning value was {}", returnedValue);
    }
}

TEST_F(Signals, RecoverValues)
{
    const bool expectedValuesArray[] = {false, false, true};
    Foo foo;

    dc::CSignal<bool(void)> signal;

    signal.Connect(&FreeFunctionReturnBool);
    signal.Connect(&Foo::StaticFunctionReturnBool);
    signal.Connect(&foo, &Foo::MemberFunctionReturnBool);

    // Using an std::vector
    std::vector<bool> valuesVector(signal.Count(), false);

    EXPECT_TRUE(signal(valuesVector));

    const auto valuesCount = signal.Count();
    checkArrayValidity(valuesVector, expectedValuesArray, valuesCount);

    // Using an array
    bool* valuesArray = new bool[valuesCount];

    EXPECT_TRUE(signal(valuesArray));
    checkArrayValidity(valuesArray, expectedValuesArray, valuesCount);

    delete[] valuesArray;
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
