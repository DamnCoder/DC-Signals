# DC-Signals

## Overview

This Signals-Slot implementation is based on the following implementations:

* [Signals by pbhogan] (https://github.com/pbhogan/Signals)
* [nano-signal-slot by NoAvailableAlias] (https://github.com/NoAvailableAlias/nano-signal-slot)

The purpose was to have a simplified version, more easily understandable and readable. 
The number of classes and types have been reduced to only two (CSignal and CConnection). 
It has also being built imitating the way signals2 is used within the boost library (everytime you make a connection it returns a type CConnection and you can use that connection to disconect it from the signal).

## Requires

The implementation relies on C++11 features like `std::addressof` or `std::forward`.

## Working with it

If you just want to use this code straight away in your project you just need to add the folder [*signals*](https://github.com/DamnCoder/DC-Signals/tree/master/signals/include/signals) in your project.

In case you would like to work with it to make changes or improve it, there are included some scripts to easily create (with **CMake**) either an **Xcode** or **Eclipse CDT** project. The projects are done so they create an executable which tests that everything works fine after any change.

As a warning, the code has been only tested in OSX and Ubuntu and is distributed under the MIT license.

## Examples of use

In the repository is included a header [`signal_tests.h`] (https://github.com/DamnCoder/DC-Signals/blob/master/signals/include/test/signal_tests.h) with examples of its use.

### Connection

Declaration of functions.

``` c++
void FreeFunctionPrintString(const char* message)

static void Foo::StaticPrintString(const char* message)

void Foo::PrintString(const char* message)

void Foo::PrintStringConst(const char* message) const
```
Connecting them to the signal

``` c++
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
```

Emission of the signal

``` c++
// Emission of signal
signal("Hello world!");
```

### Disconnection

There are two ways to disconnect functions from the signal. 

Everytime you connect a function the method returns a CConnection, and you can use that connection to disconnect the function from the signal.

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
