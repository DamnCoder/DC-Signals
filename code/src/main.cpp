//
//  main.cpp
//  SignalsTest
//
//  Created by Jorge López on 14/10/16.
//  Copyright © 2016 Jorge López. All rights reserved.
//

//#include "signal_tests.h"

#include <functional>
#include <iostream>

using FunctionPtr = void (*)(void);
using ConstFunctionPtr = void (*const*)(void);

using TTestFuncPtr = int (*) (int, int);
using TTestConstFuncPtr = int (*const*) (int, int);


void foo( std::function< void (void) > func )
{
	if( func ) // if the call-wrapper has wrapped a callable object
	{
		//typedef void function_t(void) ;
		using function_t = void (void);
		function_t** ptr_ptr_fun = func.target<function_t*>() ;
		if( ptr_ptr_fun != nullptr )
		{
			function_t* ptr_fun = *ptr_ptr_fun ;
			ptr_fun() ;
		}
	}
}

void foo2( std::function< void (void) >& func )
{
	if( func ) // if the call-wrapper has wrapped a callable object
	{
		using function_ptr = void (*)(void);
		//typedef void function_t(void) ;
		function_ptr* ptr_ptr_fun = func.target<function_ptr>() ;
		if( ptr_ptr_fun != nullptr )
		{
			function_ptr ptr_fun = *ptr_ptr_fun ;
			ptr_fun() ;
		}
	}
}

void test_it( void)
{
	printf("This works\n");
}


int f(int, int) { return 1; }
int g(int, int) { return 2; }

void test(std::function<int(int, int)> const& arg)
{
	std::cout << "test function: ";
	if (arg.target<std::plus<int>>())
		std::cout << "it is plus\n";
	
	if (arg.target<std::minus<int>>())
		std::cout << "it is minus\n";
 
	int (*const* ptr)(int, int) = arg.target<int(*)(int, int)>();
	if (ptr && *ptr == f)
		std::cout << "it is the function f\n";
	
	if (ptr && *ptr == g)
		std::cout << "it is the function g\n";
}

void test2(const std::function<int(int, int)>& arg)
{
	TTestConstFuncPtr ptr = arg.target<TTestFuncPtr>();
	if(ptr)
	{
		printf("Something got stored\n");
	}
}

ConstFunctionPtr functionStorage;

void Adapt(const std::function<void(void)>& function)
{
	functionStorage = reinterpret_cast<ConstFunctionPtr>(&function);
	if(functionStorage)
	{
		printf("Function saved!");
		const std::function<void(void)>* retransformedAction = reinterpret_cast<const std::function<void(void)>*>(functionStorage);
		retransformedAction->operator()();
	}
}

int main()
{
	//test(std::function<int(int, int)>(std::plus<int>()));
	//test(std::function<int(int, int)>(std::minus<int>()));
	//test(std::function<int(int, int)>(f));
	//test(std::function<int(int, int)>(g));
	
	std::function<int(int, int)> ffunc(f);
	//test2(ffunc);
	//test2(std::function<int(int, int)>(f));
	
	std::function<int(int, int)> lambdaFunc = [](int, int) { return 2; };
	//test2(lambdaFunc);
	
	//foo(test_it) ;
	
	//foo2(*test_it);
	
	const std::function<void(void)> constAction = []() { printf("Const action\n"); };
	std::function<void(void)> nonConstAction = []() { printf("Non const action\n"); };
	
	//FunctionPtr funcPtr = reinterpret_cast<FunctionPtr>(&nonConstAction);
	//std::function<void(void)>* retransformedAction = reinterpret_cast<std::function<void(void)>*>(funcPtr);
	//retransformedAction->operator()();
	
	Adapt(nonConstAction);
	Adapt(constAction);
	
	//foo2(nonConstAction);
	
	
	//auto funcPtr2Ptr = nonConstAction.target<FunctionPtr>();
	//(*funcPtr2Ptr)();
	
	
	
	ConstFunctionPtr funcPtr2Ptr = constAction.target<FunctionPtr>();
	//FunctionPtr func = (*funcPtr2Ptr);
	//func();
	//const ConstFunctionPtr constFuncPtr = reinterpret_cast<ConstFunctionPtr>(&constAction);
	
    //TestDCSignal();

    return 0;
}
