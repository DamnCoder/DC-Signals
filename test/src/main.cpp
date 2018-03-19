//
//  main.cpp
//  SignalsTest
//
//  Created by Jorge López on 14/10/16.
//  Copyright © 2016 Jorge López. All rights reserved.
//

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "signal_tests.h"

//		horrible_cast< >
// This is truly evil. It completely subverts C++'s type system, allowing you
// to cast from any class to any other class. Technically, using a union
// to perform the cast is undefined behaviour (even in C). But we can see if
// it is OK by checking that the union is the same size as each of its members.
// horrible_cast<> should only be used for compiler-specific workarounds.
// Usage is identical to reinterpret_cast<>.

// This union is declared outside the horrible_cast because BCC 5.5.1
// can't inline a function with a nested class, and gives a warning.
template <typename OutputClass, typename InputClass>
union horrible_union 
{
	OutputClass out;
	InputClass in;
};

template <typename OutputClass, typename InputClass>
inline OutputClass horrible_cast(const InputClass input) 
{
	horrible_union<OutputClass, InputClass> u;
	// Cause a compile-time error if in, out and u are not the same size.
	// If the compile fails here, it means the compiler has peculiar
	// unions which would prevent the cast from working.
	//typedef int ERROR_CantUseHorrible_cast[sizeof(InputClass) == sizeof(u) && sizeof(InputClass) == sizeof(OutputClass) ? 1 : -1];
	assert(sizeof(InputClass) == sizeof(OutputClass));
	u.in = input;
	return u.out;
}

class GenericClass {};

template<typename ReturnType>
class Con;

template<typename ReturnType, typename... Args>
class Con<ReturnType(Args...)>
{
public:
	using TMemberFunctionPtr = ReturnType(GenericClass::*)(Args...);
public:

	Con() {}

	template <typename TInstance, typename TMemberFunction>
	inline
	void Bind(TInstance* instance, TMemberFunction function)
	{
		// We are casting the type of 'function' to the generic type TMemberFunctionPtr
		// so we don't need to keep the original type TInstance
		caller = reinterpret_cast<GenericClass*>(instance);
		memberFunction = horrible_cast<TMemberFunctionPtr>(function);
		//void* data = reinterpret_cast<void*>(&function);
	}

	inline
	ReturnType operator() (Args&&... args) const
	{
		// Here is the reason why we need 'm_pCaller' to be of a generic class type, so it is compatible with the right hand operand '->*'
		return (caller->*memberFunction)(std::forward<Args>(args)...);
	}

private:
	TMemberFunctionPtr memberFunction;
	GenericClass* caller;
};

// Using a functor to keep the member function and the instance 

class CClassToTest
{
public:
	CClassToTest(): number(0) 
	{
		//printf("%#x\n", &CClassToTest::StupidMethod);
	}

public:
	void StupidMethod() 
	{
		printf("Hola %d!\n", number);
	}
public:
	int number;
};


class DeclaredClass;
class DefinedClass {};
class __single_inheritance SingleInheritanceClass;

using TMFPDefinedClass = void(DeclaredClass::*)();
using TMFPEmptyClass = void(DefinedClass::*)();
using TMFPSingleInheritanceClass = void(SingleInheritanceClass::*)();

void StupidTests()
{
	printf("Size of TMFPDefinedClass %d\n", sizeof(TMFPDefinedClass));
	printf("Size of TMFPEmptyClass %d\n", sizeof(TMFPEmptyClass));
	printf("Size of TMFPSingleInheritanceClass %d\n", sizeof(TMFPSingleInheritanceClass));

	CClassToTest stackCTT;
	void* storage = static_cast<void*>(&stackCTT);

	CClassToTest* conversionTest = static_cast<CClassToTest*>(storage);
	conversionTest->number = 2;
	conversionTest->StupidMethod();
	stackCTT.StupidMethod();
	//
	CClassToTest* ctt = new CClassToTest();
	Con<void(void)> con;
	con.Bind(ctt, &CClassToTest::StupidMethod);

	printf("%d == %d\n", sizeof(Con<void(void)>::TMemberFunctionPtr), sizeof(&CClassToTest::StupidMethod));
	con();
}

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
	/*
	void Print(const char* msg, Args&&... args)
	{
		printf("Normal pack expansion -> ");
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
		printf("Are the same? %d ->", std::is_same<std::tuple<FuncArgs...>, std::tuple<Args...> >::value);
		//static_assert(, "Passed types should be the same");
		printf("Perfect forwarding -> ");
		printf(msg, std::forward<Args>(args)...);
	}

	void FooFunc(Args... args)
	{

	}
	//*/
};

int main()
{
	//Function(&CClassToTest());
	//Function("Hola");

	printf("Size of TMFPDefinedClass %d\n", sizeof(TMFPDefinedClass));
	printf("Size of TMFPEmptyClass %d\n", sizeof(TMFPEmptyClass));
	printf("Size of TMFPSingleInheritanceClass %d\n", sizeof(TMFPSingleInheritanceClass));

	for (int i = 0; i < 10; ++i)
	{
		CClassToTest();
	}
	//CPFTest<const char*> test;
	//test.Print("Variadic template parameters test %s\n", "MESSAGE");

	Foo foo;
	CPFTest<Foo&> fooTest;
	//fooTest.Print("Variadic template parameters test %d\n", foo);
	fooTest.FooFunc(foo);

	TestDCSignal();
	
	system("pause");
	return 0;
}
