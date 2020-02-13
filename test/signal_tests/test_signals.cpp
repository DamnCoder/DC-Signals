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

#include <gtest/gtest.h>
#include <signals/signal.h>
#include <functional>
#include <utility>
#include "signal_test_utils.h"

TEST(Signals, TestAutoreleaseConnections)
{
}

TEST(Signals, TestConnectingFunctions)
{
    printf("+ INIT HOW TO USE IT TEST\n");

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
        printf("The std::function message: %s\n", message);
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
    EXPECT_EQ(signal.Count(), 0);

    // Emission of signal
    printf("+ EMITTING THE SIGNAL AFTER DISCONNECTING (nothing should happen)\n");

    EXPECT_TRUE(signal("Hello"));
}

TEST(Signals, TestCopy)
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

TEST(Signals, TestMove)
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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
