/*
 * signal_tests.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: jorge
 */

#ifndef SIGNAL_TESTS_H_
#define SIGNAL_TESTS_H_

#include <utility>
#include <functional>
#include <assert.h>
#include <stdio.h>
#include <string>

#include <signals/signal.h>

////////////////////////////////////////////////////////////////////////
// Test types to be used as parameters
////////////////////////////////////////////////////////////////////////
struct TActionFunctor
{
public:
	TActionFunctor(){}
	virtual ~TActionFunctor(){}

private:
	TActionFunctor(const TActionFunctor& copy) {}

public:

	void operator()(const char* message)
	{
		printf("The functor message is: %s\n", message);
	}
};

class Foo
{
public:
	Foo(){}
	~Foo(){}

public:
	void PrintSomething()
	{
		printf("Member method no arguments no return value!\n");
	}

	void PrintString(const char* message)
	{
		printf("The member message is: %s\n", message);
	}
	
	void PrintStringRef(const std::string& message)
	{
		printf("The member message is: %s\n", message.c_str());
	}

	void PrintStringRefConst(const std::string& message) const
	{
		printf("The member message is: %s\n", message.c_str());
	}
	
	void PrintStringConst(const char* message) const
	{
		printf("The member message is: %s\n", message);
	}
	
	void PrintStringConst(const char* message, const bool isTrue) const
	{
		printf("The member message is: %s? %s \n", message, isTrue ? "true" : "false");
	}
	
	const bool MemberFunctionReturnBool()
	{
		printf("Member function returning true\n");
		return true;
	}
	
	static const bool StaticFunctionReturnBool()
	{
		printf("Static function returning false\n");
		return false;
	}
	
	static void StaticPrintString(const char* message)
	{
		printf("The static message is: %s\n", message);
	}
};

////////////////////////////////////////////////////////////////////////
// Test methods to be used as parameters
////////////////////////////////////////////////////////////////////////

void TestFunction()
{
	printf("Yeah! It's me! A function!\n");
}

const bool FreeFunctionReturnBool()
{
	printf("Free function returning false\n");
	return false;
}

const bool TestFunctionReturnAndPassing(const char* message)
{
	printf("%s\n", message);
	return true;
}

void FreeFunctionPrintStringRef(const std::string& message)
{
	printf("The free message is: %s\n", message.c_str());
}

void FreeFunctionPrintString(const char* message)
{
	printf("The free message is: %s\n", message);
}

void FreeFunctionCopyObject(Foo foo)
{
	printf("Foo passed correctly\n");
}
////////////////////////////////////////////////////////////////////////
// Test examples of use
////////////////////////////////////////////////////////////////////////

template<typename ReturnType>
class CSignalTests;

template<typename ReturnType, typename... Args>
class CSignalTests<ReturnType(Args...)>
{
public:
	CSignalTests(){}
	
public:
	template <typename T>
	const bool ConnectingFunction(T& slotRef, Args&&... args) const
	{
		dc::CSignal<ReturnType(Args...)> signal;
		auto connection = signal.Connect(slotRef);
		
		// We check if it is connected and if it executes correctly the signal
		const bool succeed = connection.IsConnected() && signal(std::forward<Args>(args)...);
		
		assert("[ERROR] Connecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool DisconnectingFunction(T& slotRef) const
	{
		bool succeed = true;
		
		dc::CSignal<ReturnType(Args...)> signal;
		
		// Using disconnect method from connection (it uses internally the signal's disconnect)
		auto connection = signal.Connect(slotRef);
		connection.Disconnect();
		
		succeed &= signal.IsEmpty() && !connection.IsConnected();
		
		// Using disconnect method with the connection parameters
		signal.Connect(slotRef);
		signal.Disconnect(slotRef);
		
		succeed &= signal.IsEmpty();
		
		assert("[ERROR] Disconnecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool ConnectingFunction(T* slotPtr, Args&&... args) const
	{
		dc::CSignal<ReturnType(Args...)> signal;
		auto connection = signal.Connect(slotPtr);
		
		// We check if it is connected and if it executes correctly the signal
		const bool succeed = connection.IsConnected() && signal(std::forward<Args>(args)...);
		
		assert("[ERROR] Connecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool DisconnectingFunction(T* slotPtr) const
	{
		bool succeed = true;
		
		dc::CSignal<ReturnType(Args...)> signal;
		
		// Using disconnect method from connection (it uses internally the signal's disconnect)
		auto connection1 = signal.Connect(slotPtr);
		connection1.Disconnect();
		
		succeed &= signal.IsEmpty() && !connection1.IsConnected();
		
		// Using disconnect method with the connection parameters
		signal.Connect(slotPtr);
		signal.Disconnect(slotPtr);
		
		succeed &= signal.IsEmpty();
		
		assert("[ERROR] Disconnecting test failed" && succeed);
		return succeed;
	}

	template<typename T>
	const bool ConnectingMemberFunction(T* caller, ReturnType(T::* function)(Args...), Args&&... args) const
	{
		dc::CSignal<ReturnType(Args...)> signal;
		auto connection = signal.Connect(caller, function);
		
		// We check if it is connected and if it executes correctly the signal
		const bool succeed = connection.IsConnected() && signal(std::forward<Args>(args)...);
		
		assert("[ERROR] Connecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool DisconnectingMemberFunction(T* caller, ReturnType(T::* function)(Args...)) const
	{
		bool succeed = true;
		
		dc::CSignal<ReturnType(Args...)> signal;
		
		// Using disconnect method from connection (it uses internally the signal's disconnect)
		auto connection = signal.Connect(caller, function);
		connection.Disconnect();
		
		succeed &= signal.IsEmpty() && !connection.IsConnected();
		
		// Using disconnect method with the connection parameters
		signal.Connect(caller, function);
		signal.Disconnect(caller, function);
		
		succeed &= signal.IsEmpty();
		
		assert("[ERROR] Disconnecting test failed" && succeed);
		return succeed;
	}
	
	template<typename T>
	const bool ConnectingMemberFunction(T* caller, ReturnType(T::* function)(Args...) const, Args&&... args) const
	{
		dc::CSignal<ReturnType(Args...)> signal;
		auto connection = signal.Connect(caller, function);
		
		// We check if it is connected and if it executes correctly the signal
		const bool succeed = connection.IsConnected() && signal(std::forward<Args>(args)...);
		
		assert("[ERROR] Connecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool DisconnectingMemberFunction(T* caller, ReturnType(T::* function)(Args...) const) const
	{
		bool succeed = true;
		
		dc::CSignal<ReturnType(Args...)> signal;
		
		// Using disconnect method from connection (it uses internally the signal's disconnect)
		auto connection = signal.Connect(caller, function);
		connection.Disconnect();
		
		succeed &= signal.IsEmpty() && !connection.IsConnected();
		
		// Using disconnect method with the connection parameters
		signal.Connect(caller, function);
		signal.Disconnect(caller, function);
		
		succeed &= signal.IsEmpty();
		
		assert("[ERROR] Disconnecting test failed" && succeed);
		return succeed;
	}
	
	template<typename T>
	const bool ConnectingMemberFunction(const T* caller, ReturnType(T::* function)(Args...) const, Args&&... args) const
	{
		dc::CSignal<ReturnType(Args...)> signal;
		auto connection = signal.Connect(caller, function);
		
		// We check if it is connected and if it executes correctly the signal
		const bool succeed = connection.IsConnected() && signal(std::forward<Args>(args)...);
		
		assert("[ERROR] Connecting test failed" && succeed);
		return succeed;
	}
	
	template <typename T>
	const bool DisconnectingMemberFunction(const T* caller, ReturnType(T::* function)(Args...) const) const
	{
		bool succeed = true;
		
		dc::CSignal<ReturnType(Args...)> signal;
		
		// Using disconnect method from connection (it uses internally the signal's disconnect)
		auto connection = signal.Connect(caller, function);
		connection.Disconnect();
		
		succeed &= signal.IsEmpty() && !connection.IsConnected();
		
		// Using disconnect method with the connection parameters
		signal.Connect(caller, function);
		signal.Disconnect(caller, function);
		
		succeed &= signal.IsEmpty();
		
		assert("[ERROR] Disconnecting test failed" && succeed);
		return succeed;
	}
};

const bool SimpleTestHowToUseIt()
{
	printf("+ INIT HOW TO USE IT TEST\n");
	bool succeed = true;
	
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
	std::function<void(const char*)> stdFunctionParameter = [](const char* message){
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
	
	succeed &= signal.Count() == 8;
	
	assert("[ERROR] Connecting failed" && succeed);
	
	printf("+ EMITTING THE SIGNAL AFTER CONNECTING\n");
	
	// Emission of signal
	succeed &= signal("Hello world!");
	
	assert("[ERROR] Emitting failed" && succeed);
	
	// Disconnecting binded functions
	// - with no parameter binded
	signal.Disconnect(functionBindedWithoutBindedParams);
	
	// - with binded parameter
	signal.Disconnect(functionBindedWithParams);
	
	// Disconnecting member function
	signal.Disconnect(&foo, &Foo::PrintString);
	
	// Disconnecting static function
	signal.Disconnect(&Foo::StaticPrintString);
	
	// Disconnecting free function
	signal.Disconnect(&FreeFunctionPrintString);
	
	// Disconnecting lambda
	signal.Disconnect(stdFunctionParameter);
	
	// Disconnecting Functor
	signal.Disconnect(functor);
	
	// Disconnecting const member function
	signal.Disconnect(&foo , &Foo::PrintStringConst);
	
	succeed &= signal.Count() == 0;
	
	// Emission of signal
	printf("+ EMITTING THE SIGNAL AFTER DISCONNECTING\n");
	
	succeed &= signal("Hello");
	
	printf("- END HOW TO USE IT TEST\n");

	assert("[ERROR] Disconnecting failed" && succeed);
	
	return succeed;
}

const bool SimpleTestHowToRecoverValues()
{
	printf("+ INIT HOW TO RECOVER VALUES TEST\n");
	
	bool succeed = true;
	
	const bool expectedValuesArray[] = { false, false, true};
	
	Foo foo;
	
	dc::CSignal<const bool(void)> signal;
	
	signal.Connect(&FreeFunctionReturnBool);
	signal.Connect(&Foo::StaticFunctionReturnBool);
	signal.Connect(&foo, &Foo::MemberFunctionReturnBool);
	
	// Using an std::vector
	std::vector<bool> valuesVector(signal.Count());
	
	succeed &= signal(valuesVector);
	
	const long valuesCount = signal.Count();
	
	for(int i=0; i<valuesCount; ++i)
	{
		const bool returnedValue = valuesVector[i];
		const bool expectedValue = expectedValuesArray[i];
		
		succeed &= (returnedValue == expectedValue);
		
		printf("Returning value was %s\n", returnedValue ? "true" : "false");
	}
	
	// Using an array
	bool* valuesArray = new bool[valuesCount];
	
	succeed &= signal(valuesArray);
	
	for(int i=0; i<valuesCount; ++i)
	{
		const bool returnedValue = valuesArray[i];
		const bool expectedValue = expectedValuesArray[i];
		
		succeed &= (returnedValue == expectedValue);
		
		printf("Returning value was %s\n", returnedValue ? "true" : "false");
	}
	
	delete [] valuesArray;
	
	assert("[ERROR] SimpleTestHowToUseIt failed" && succeed);
	
	printf("- END HOW TO RECOVER VALUES TEST\n");
	
	return succeed;
}

void TestDCSignal()
{
	printf("+++ TEST DC SIGNAL\n\n");
	
	Foo foo;
	bool succeed = true;
	
	dc::CSignal<void(Foo)> signalCopy;
	
	signalCopy.Connect(&FreeFunctionCopyObject);
	succeed &= signalCopy.Count() == 1;
	succeed &= signalCopy(foo);
	
	signalCopy.Disconnect(&FreeFunctionCopyObject);
	succeed &= signalCopy.Count() == 0;
	
	
	dc::CSignal<void(const std::string&)> signalRef;
	
	signalRef.Connect(&FreeFunctionPrintStringRef);
	succeed &= signalRef.Count() == 1;
	succeed &= signalRef("Text by reference");
	
	signalRef.Disconnect(&FreeFunctionPrintStringRef);
	succeed &= signalRef.Count() == 0;
	
	signalRef.Connect(&foo, &Foo::PrintStringRefConst);
	succeed &= signalRef.Count() == 1;
	succeed &= signalRef("Text by reference in a const method");
	
	signalRef.Disconnect(&foo, &Foo::PrintStringRefConst);
	succeed &= signalRef.Count() == 0;

	
	const char* message = "Hello World!";
	
	
	
	CSignalTests<void(const char*)> signalTests;
	
	
	// Testing free function
	succeed &= signalTests.ConnectingFunction(&FreeFunctionPrintString, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(&FreeFunctionPrintString);
	
	// Testing static function
	succeed &= signalTests.ConnectingFunction(&Foo::StaticPrintString, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(&Foo::StaticPrintString);
	
	// Testing binded function with no parameters attached
	auto functionBindedWithoutBindedParams = std::bind(&Foo::PrintString, &foo, std::placeholders::_1);
	succeed &= signalTests.ConnectingFunction(functionBindedWithoutBindedParams, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(functionBindedWithoutBindedParams);
	
	// Testing binded function with parameters attached
	auto functionBindedWithBindedParams = std::bind(&Foo::PrintString, &foo, "Binded parameter");
	succeed &= signalTests.ConnectingFunction(functionBindedWithBindedParams, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(functionBindedWithBindedParams);
	
	// Testing std::function
	std::function<void(const char*)> stdFunction = [](const char* message)
	{
		printf("The std::function message: %s\n", message);
	};
	succeed &= signalTests.ConnectingFunction(stdFunction, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(stdFunction);
	
	// Testing functor
	TActionFunctor functor;
	succeed &= signalTests.ConnectingFunction(functor, std::forward<const char*>(message));
	succeed &= signalTests.DisconnectingFunction(functor);
	
	// Testing member function
	succeed &= signalTests.ConnectingMemberFunction(&foo, &Foo::PrintString, "Message for member function");
	succeed &= signalTests.DisconnectingMemberFunction(&foo, &Foo::PrintString);
	
	// Testing const member function
	succeed &= signalTests.ConnectingMemberFunction(&foo, &Foo::PrintStringConst, "Message for const member function");
	succeed &= signalTests.DisconnectingMemberFunction(&foo, &Foo::PrintStringConst);
	
	// Testing const caller with const member function
	const Foo fooConst;
	succeed &= signalTests.ConnectingMemberFunction(&fooConst, &Foo::PrintStringConst, "Message for const caller and const member function");
	succeed &= signalTests.DisconnectingMemberFunction(&fooConst, &Foo::PrintStringConst);
	
	succeed &= SimpleTestHowToUseIt();
	succeed &= SimpleTestHowToRecoverValues();
	
	if(succeed)
	{
		printf("\n+ ALL TESTS WERE SUCCESSFUL\n\n");
	}
	else
	{
		printf("\n- TESTS FAILED\n\n");
	}
	
	printf("---END TEST DC SIGNAL\n");
}

#endif /* SIGNAL_TESTS_H_ */
