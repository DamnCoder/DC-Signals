/*
 The MIT License (MIT)

 Copyright (c) 2018 Jorge López González

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * silly_tests.h
 *
 *  Created on: Mar 21, 2018
 *      Author: jorge
 */

#pragma once

#include <cassert>
#include <utility>

/*
 * UNIVERSAL CASTING
 * from pbhogan code
 */
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
	static_assert(sizeof(InputClass) == sizeof(OutputClass), "Sizes are not the same");
	u.in = input;
	return u.out;
}

/*
 * DECLARED CLASSES SIZES
 */

class DeclaredClass;
using TMFPDeclaredClass = void(DeclaredClass::*)();

class DefinedClass {};
using TMFPDefinedClass = void(DefinedClass::*)();

#if defined(_MSC_VER)
class __single_inheritance SingleInheritanceClass;
using TMFPSingleInheritanceClass = void(SingleInheritanceClass::*)();
#endif

void TestTypeSizes()
{
	printf("Size of TMFPDefinedClass %lu\n", sizeof(TMFPDeclaredClass));

	printf("Size of DefinedClass %lu\n", sizeof(DefinedClass));
	printf("Size of TMFPDEfinedClass %lu\n", sizeof(TMFPDefinedClass));

#if defined(_MSC_VER)
	printf("Size of SingleInheritanceClass %d\n", sizeof(SingleInheritanceClass));
	printf("Size of TMFPSingleInheritanceClass %d\n", sizeof(TMFPSingleInheritanceClass));
#endif
}

/*
 * MFP Basic connection
 */

class CClassToTest
{
public:
	CClassToTest(): number(0)
	{
	}

public:
	void StupidMethod()
	{
		printf("Hola %d!\n", number);
	}
public:
	int number;
};

#if defined(_MSC_VER)
	// Compilers using Microsoft's structure need to be treated as a special case.

	// From: https://msdn.microsoft.com/en-us/library/ck561bfk.aspx
	//
	// C++ allows you to declare a pointer to a class member prior to the definition of the class, example:
	//
	// class S;
	// int S::*p;
	//
	// In the code above, p is declared to be a pointer to integer member of class S.
	// However, class S has not yet been defined in this code; it has only been declared.
	// When the compiler encounters such a pointer, it must make a generalized representation of the pointer.
	// The size of the representation is dependent on the inheritance model specified.
	class __single_inheritance GenericClass;
#else
	class GenericClass;
#endif

template<typename ReturnType>
class Con;

template<typename ReturnType, typename... Args>
class Con<ReturnType(Args...)>
{
public:
	using TMemberFunctionPtr = ReturnType(GenericClass::*)(Args...);
public:

	Con():memberFunction(0), caller(0) {}

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

	ReturnType operator() (Args&&... args) const
	{
		// Here is the reason why we need 'm_pCaller' to be of a generic class type, so it is compatible with the right hand operand '->*'
		return (caller->*memberFunction)(std::forward<Args>(args)...);
	}

private:
	TMemberFunctionPtr memberFunction;
	GenericClass* caller;
};

void TestSimpleCon()
{
	// Test void* storage
	CClassToTest stackCTT;
	void* storage = static_cast<void*>(&stackCTT);
	CClassToTest* conversionTest = static_cast<CClassToTest*>(storage);
	conversionTest->number = 2;
	conversionTest->StupidMethod();
	stackCTT.StupidMethod();

	// Test connection
	CClassToTest* ctt = new CClassToTest();
	Con<void(void)> con;
	con.Bind(ctt, &CClassToTest::StupidMethod);

	printf("%lu == %lu\n", sizeof(Con<void(void)>::TMemberFunctionPtr), sizeof(&CClassToTest::StupidMethod));
	con();
}
