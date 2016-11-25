# DC-Signals

## Overview

This [Signals - Slot][signals_slots_wikipedia] implementation is based on the following implementations:

* [Signals by pbhogan] [signals_repo]
* [nano-signal-slot by NoAvailableAlias] [nano_signal_slot_repo]

The purpose was to have a simplified version, using [variadic templates][variadic_templates], more easily understandable and readable. 
The number of classes and types have been reduced to only two (CSignal and CConnection).

I liked the way *pbhogan* implemented it, but it didn't use variadic templates so it has implemented the solution up till 8 times, each one for a different number of arguments. His solution for the problem is really amazing and I highly recommend to spend some time going through the code analizing all the tricks and tips it provide.

On the other hand even though *NoAvailableAlias* uses variadic templates and implements the solution in a very clever and elegant way. I didn't like the way it worked for member functions because it makes you indicate the type of the class, the signature of the function and pass the instance, making the connection a bit messy.

Example from nano-signal-slot:

``` cpp
Nano::Signal<bool(const char*)> signal_one;
signal_one.disconnect<Foo, &Foo::handler_a>(foo);
```

So I have mixed the best things of both. It has also being built imitating the way [signals2][boost_signals2] is used within the boost library (everytime you make a connection it returns a CConnection and you can use that connection to disconect it from the signal).

## Requires

The implementation relies on C++11 features like `std::addressof` or `std::forward`.

## Working with it

If you just want to use this code straight away in your project you just need to add the folder [*signals*][dc_signals_repo_folder] into your project.

In case you would like to work with it to make changes or improve it, there are included some scripts to easily create (with **CMake**) either an **Xcode** or **Eclipse CDT** project. The projects are done so they create an executable which tests that everything works fine after any change.

As a warning, the code has been only tested in OSX and Ubuntu and is distributed under the [MIT license][mit_license].

## Examples of use

In the repository is included a header [`signal_tests.h`][dc_signals_repo_test_file] with examples of its use.

### Connection and emitting without returning values

Declaration of functions.

``` c++
void FreeFunctionPrintString(const char* message)

static void Foo::StaticPrintString(const char* message)

void Foo::PrintString(const char* message)

void Foo::PrintStringConst(const char* message) const
```

Use and emission

``` c++
// Class of wich we will use its methods as callbacks
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

// Emission of signal
signal("Hello world!");
```

### Connecting and emitting returning values.

Declaration of the test functions.

``` c++
const bool FreeFunctionReturnBool(void);
static const bool Foo::StaticFunctionReturnBool(void);
const bool Foo::MemberFunctionReturnBool(void);
```

Use and emission.

``` c++
Foo foo;
	
dc::CSignal<const bool(void)> signal;
signal.Connect(&FreeFunctionReturn);
signal.Connect(&Foo::StaticFunctionReturnBool);
signal.Connect(&foo, &Foo::MemberFunctionReturnBool);

// Using an std::vector
std::vector<bool> valuesVector(signal.ConnectionsCount());

signal(valuesVector);

for(bool value : valuesVector)
{
	printf("Returning value was %s\n", value ? "true" : "false");
}

// Using an array
const long valuesCount = signal.ConnectionsCount();
bool* valuesArray = new bool[valuesCount];

signal(valuesArray);

for(int i=0; i<valuesCount; ++i)
{
	const bool value = valuesArray[i];
	printf("Returning value was %s\n", value ? "true" : "false");
}
```

### Disconnection

There are two ways to disconnect functions from the signal. 

Everytime you connect a function the method returns a `CConnection`, and you can use that connection to disconnect the function from the signal.

``` cpp
dc::CSignal<void(const char*)> signal;
dc::CConnection<void(const char*)> connection = signal.Connect(&foo, &Foo::PrintString);

signal("Hello world!");

connection.Disconnect();
```

Or you can just ask directly to the signal passing it the functions you want to disconnect.

``` cpp
dc::CSignal<void(const char*)> signal;
signal.Connect(&foo, &Foo::PrintString);

signal("Hello world!");

signal.Disconnect(&foo, &Foo::PrintString);
```

## How it works internally

### The idea

The problems mainly come because we don't want to indicate the type of the class when we declare a signal. We want to write something like this:

``` cpp
dc:CSignal<void(const bool, const char*)> signal;
```

So the idea is to cast the callers to a common `GenericClass*` and the functions to a `GenericClass::*` member function. The tricky part comes in the cases of *binded functions, lambdas, functors, and with free and static functions*, where we also need to keep the function apart.

### Member functions

Member functions are the easiest to keep. We use `reinterpret_cast` to adapt their types so we are able to assign the caller instance and the member function without indicating the type of the class.

You can't cast a member function and transform it to a function pointer, but you can cast from a member function into a member function of another class. 

``` cpp
template <typename ReturnType, typename... Args>
class CConnection<ReturnType(Args...)>
{
	// Definition of the type
	using TMemberFunctionPtr = ReturnType (GenericClass::*)(Args...);
	
	...
	
	template <typename TInstance, typename TMemberFunction>
	inline
	void Bind(TInstance* instance, TMemberFunction function)
	{
		// We are casting the type of 'function' to the generic type TMemberFunctionPtr
		// so we don't need to keep the original type TInstance
		m_pCaller = reinterpret_cast<GenericClass *>(instance);
		m_pMemberFunction = reinterpret_cast<TMemberFunctionPtr>(function);
	}
};
```

### Free and static functions

To keep this kind of functions what I do is to keep the function pointer apart in `TFunctionPtr m_pFunction` and bind the connection itself with `CConnection::InvokeFunctionPtr`, which is the function that effectively calls `m_pFunction`.

``` cpp

// Connection
template <typename T>
CConnection<ReturnType(Args...)> CSignal::Connect(T* slotPtr)

...

template <typename ReturnType, typename... Args>
class CConnection<ReturnType(Args...)>
{
	// Definition of the type
	using TFunctionPtr = ReturnType (*)(Args...);
	
	...
	
	CConnection(CSignal<ReturnType(Args...)>* signal, T* functionPtr):
		m_pBindedSignal(signal)
	{
		m_pFunction = functionPtr;
		Bind(this, &CConnection::InvokeFunctionPtr);
	}
	
	...
	
	ReturnType InvokeFunctionPtr(Args... args) const
	{
		return (*m_pFunction)(args...);
	}
};
```

### Binded functions, lambdas, functors

For this ones we need to keep track of the type, luckily we can pass a function template as parameter, so we just need to pass it and cast the element that arrives into a function pointer.

``` cpp
// Connection
template <typename T>
CConnection<ReturnType(Args...)> CSignal::Connect(T& slotRef)

...

template <typename ReturnType, typename... Args>
class CConnection<ReturnType(Args...)>
{
	
	using TFunctionPtr = ReturnType (*)(Args...);
	
	...
	
	template <typename T>
	CConnection(CSignal<ReturnType(Args...)>* signal, T& ref)
	{
			m_pFunction = reinterpret_cast<TFunctionPtr>(std::addressof(ref));
			Bind(this, &CConnection::InvokeTemplatizedFunctionPtr<T>);
	}
	
	...
	
	template< typename T>
	inline
	ReturnType InvokeTemplatizedFunctionPtr(Args... args) const
	{
		return (reinterpret_cast<T*>(m_pFunction)->operator())(args...);
	}
};
```



[signals_slots_wikipedia]: https://en.wikipedia.org/wiki/Signals_and_slots "Wikipedia - Signals and slots"
[signals_repo]: https://github.com/pbhogan/Signals "Signals github repository"
[nano_signal_slot_repo]: https://github.com/NoAvailableAlias/nano-signal-slot "Nano-Signal-Slot github repository"
[variadic_templates]: https://en.wikipedia.org/wiki/Variadic_template "Wikipedia - Variadic Templates"
[boost_signals2]: http://www.boost.org/doc/libs/1_61_0/doc/html/signals2.html "Boost Signals2 page"
[mit_license]: https://opensource.org/licenses/mit-license.php "MIT License page"
[dc_signals_repo_folder]: https://github.com/DamnCoder/DC-Signals/tree/master/signals/include/signals "DC-Signal signals folder" 
[dc_signals_repo_test_file]: https://github.com/DamnCoder/DC-Signals/blob/master/signals/include/test/signal_tests.h "DC-Signal tests file"