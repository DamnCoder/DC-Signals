/*
 The MIT License (MIT)

 Copyright (c) 2016 Jorge López González

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * perfect_forwarding_tests.h
 *
 *  Created on: Mar 21, 2018
 *      Author: jorge
 */

#pragma once

#include <utility>

/*
template<typename... Args>
void Function(Args... args)
{
	printf("Expanded to copy args\n");
}

template<typename... Args>
void Function(Args&... args)
{
	printf("Expanded to ref args\n");
}

// PERFECT FORWARDING
template<typename... Args>
void Function(Args&&... args)
{
	printf("Expanded to move args\n");
}

template<typename... Args>
void Function(Args*... args)
{
	printf("Expanded to ptr args\n");
}
*/

template<typename... Args>
class CPFTest
{
public:
	//*
	void PrintUR(const char* msg, Args&&... args)
	{
		printf("Args declared on class template -> ");
		printf(msg, std::forward<Args>(args)...);
	}
	//*/

	/*
	void Print(const char* msg, Args... args)
	{
		printf("No universal expansion -> ");
		printf(msg, std::forward<Args>(args)...);
	}
	//*/

	//*
	template<typename... FuncArgs>
	void Print(const char* msg, FuncArgs&&... args)
	{
		printf("Are the same? %d ->", std::is_same<std::tuple<FuncArgs&&...>, std::tuple<Args...> >::value);
		//static_assert(, "Passed types should be the same");
		printf("Args declared on function template -> ");
		printf(msg, std::forward<Args>(args)...);
	}

	void FooFunc(Args... args)
	{
		printf("FooFunc called\n");
	}
	//*/
};

void TestSimplePerfectForwarding()
{
	//Function(&CClassToTest());
	//Function("Hola");

	CPFTest<const char*> test;
	test.Print("Variadic template parameters test %s\n", "MESSAGE");
	test.PrintUR("Variadic template parameters test %s\n", "MESSAGE");

	Foo foo;
	CPFTest<Foo&> fooTest;
	fooTest.Print("Variadic template parameters test %d\n", foo);
	fooTest.PrintUR("Variadic template parameters test %d\n", foo);
	fooTest.FooFunc(foo);
}
