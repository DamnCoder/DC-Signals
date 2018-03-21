/*
 The MIT License (MIT)
 
 Copyright (c) 2016 Jorge López González
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * connection.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: Jorge López González
 *
 * Inspired by the solutions from:
 * - pbhogan (https://github.com/pbhogan/Signals)
 * - NoAvailableAlias (https://github.com/NoAvailableAlias/nano-signal-slot)
 */

#pragma once

#include <utility>

namespace dc
{
	// Forward declaration for the signals
	template<typename ReturnType, typename... Args>
	class CSignal;
	
	// The use of GenericClass comes as the solution to avoid specifying the class type
	// when defining a signal when we want to store MFP (member function pointer)
	// From pbhogan (https://github.com/pbhogan/Signals/blob/master/Delegate.h)
	// GenericClass is a fake class, ONLY used to provide a type.
	// It is vitally important that it is never defined, so that the compiler doesn't
	// think it can optimize the invocation. 

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
	class CConnection;
	
	/*
	 CCONNECTION
	 
	 Base class for connections. Its purpose is to act as an wrapper for the callbacks
	 */
	template <typename ReturnType, typename... Args>
	class CConnection<ReturnType(Args...)>
	{
		friend class CSignal<ReturnType(Args...)>;
		
		//------------------------------------------------------------------------------------------------------------------------
		// Type definitions
		//------------------------------------------------------------------------------------------------------------------------
	protected:
		// Generic types for functions
		using TMemberFunctionPtr	= ReturnType (GenericClass::*)(Args...);
		using TFunctionPtr			= ReturnType (*)(Args...);
		using TConnection			= CConnection<ReturnType(Args...)>;
		using TSignal				= CSignal<ReturnType(Args...)>;
		
		//------------------------------------------------------------------------------------------------------------------------
		// Getters / Setters
		//------------------------------------------------------------------------------------------------------------------------
	public:
		const bool IsConnected() const;

		//------------------------------------------------------------------------------------------------------------------------
		// Constructors / Destructors
		//------------------------------------------------------------------------------------------------------------------------
	public:
		~CConnection();

		CConnection(const TConnection& copy);
		CConnection(TConnection&& copy);

		TConnection& operator= (const TConnection& moved);

	private:
		template <typename T>
		CConnection(TSignal* signal, T& ref);
		
		template <typename T>
		CConnection(TSignal* signal, T* functionPtr);

		template <typename TInstance, typename TMemberFunction>
		CConnection(TSignal* signal, TInstance* instance, TMemberFunction function);
		
		//------------------------------------------------------------------------------------------------------------------------
		// Functions
		//------------------------------------------------------------------------------------------------------------------------
	public:
		void Disconnect();

		const bool operator== (const TConnection& connection) const;
		
	private:
		ReturnType operator() (Args... args) const;

		template <typename TInstance, typename TMemberFunction>
		void Bind(TInstance* instance, TMemberFunction function);

		/*
		We use this method on copy constructor and copy assignment operator because we want only to transfer the pointers,
		no allocations are involved
		*/
		void PassParameters(const TConnection& copy);
		
		//------------------------------------------------------------------------------------------------------------------------
		// Equals to check wether the function passed is the same as the one keeped in the connection
		//------------------------------------------------------------------------------------------------------------------------

		template <typename T, typename TMemberFunction>
		const bool Equals(T* caller, TMemberFunction function) const;
		
		template <typename T>
		const bool Equals(T& reference) const;
		
		template <typename T>
		const bool Equals(T* pointer) const;
		
		//------------------------------------------------------------------------------------------------------------------------
		// Helper functions to transform into member function binded functions, lambdas, functors, free and static functions
		//------------------------------------------------------------------------------------------------------------------------
		
		// Used when we want to call back binded functions, lambdas and functors
		template< typename T>
		ReturnType InvokeTemplatizedFunctionPtr(Args... args) const;

		// Used when we want to call back free or static functions
		ReturnType InvokeFunctionPtr(Args... args) const;
		
		void Clear();

	private:
		TSignal*			mp_bindedSignal;
		
		GenericClass*		mp_caller;
		TMemberFunctionPtr	mp_memberFunction;
		
		TFunctionPtr		mp_function;
	};

	template <typename ReturnType, typename... Args>
	const bool CConnection<ReturnType(Args...)>::IsConnected() const
	{
		const bool hasMFP = mp_bindedSignal && mp_caller && mp_memberFunction;
		const bool hasFP = mp_function && hasMFP;

		// mp_function is not zero only when you connect a free or static function
		return hasFP || hasMFP;
	}

	template <typename ReturnType, typename... Args>
	CConnection<ReturnType(Args...)>::~CConnection()
	{
		Clear();
	}

	template <typename ReturnType, typename... Args>
	CConnection<ReturnType(Args...)>::CConnection(const TConnection& copy)
	{
		PassParameters(copy);
	}

	template <typename ReturnType, typename... Args>
	CConnection<ReturnType(Args...)>::CConnection(TConnection&& moved)
	{
		mp_bindedSignal = std::move(moved.mp_bindedSignal);
		if (moved.mp_function)
		{
			mp_function = std::move(moved.mp_function);
			Bind(this, std::move(moved.mp_memberFunction));
		}
		else
		{
			mp_function = 0;
			Bind(std::move(moved.mp_caller), std::move(moved.mp_memberFunction));
		}
	}

	template <typename ReturnType, typename... Args>
	CConnection<ReturnType(Args...)>& CConnection<ReturnType(Args...)>::operator= (const TConnection& copy)
	{
		PassParameters(copy);
		return *this;
	}

	template <typename ReturnType, typename... Args>
	template <typename T>
	CConnection<ReturnType(Args...)>::CConnection(TSignal* signal, T& ref) :
		mp_bindedSignal(signal),
		mp_caller(0),
		mp_memberFunction(0),
		mp_function(0)

	{
		mp_function = reinterpret_cast<TFunctionPtr>(&ref);
		Bind(this, &CConnection::InvokeTemplatizedFunctionPtr<T>);
	}

	template <typename ReturnType, typename... Args>
	template <typename T>
	CConnection<ReturnType(Args...)>::CConnection(TSignal* signal, T* functionPtr) :
		mp_bindedSignal(signal),
		mp_caller(0),
		mp_memberFunction(0),
		mp_function(0)

	{
		mp_function = functionPtr;
		Bind(this, &CConnection::InvokeFunctionPtr);
	}

	template <typename ReturnType, typename... Args>
	template <typename TInstance, typename TMemberFunction>
	CConnection<ReturnType(Args...)>::CConnection(TSignal* signal, TInstance* instance, TMemberFunction function) :
		mp_bindedSignal(signal),
		mp_caller(0),
		mp_memberFunction(0),
		mp_function(0)
	{
		Bind(instance, function);
	}

	template <typename ReturnType, typename... Args>
	void CConnection<ReturnType(Args...)>::Disconnect()
	{
		// We need to dereference ourselves to properly use the disconnection
		mp_bindedSignal->Disconnect(*this);
		Clear();
	}

	template <typename ReturnType, typename... Args>
	const bool CConnection<ReturnType(Args...)>::operator== (const TConnection& connection) const
	{
		if (this == &connection) return true;
		if (mp_caller == connection.mp_caller) return true;

		return mp_bindedSignal == connection.mp_bindedSignal
			&& mp_function == connection.mp_function
			&& mp_memberFunction == connection.mp_memberFunction;
	}

	template <typename ReturnType, typename... Args>
	ReturnType CConnection<ReturnType(Args...)>::operator() (Args... args) const
	{
		// Here is the reason why we need 'm_pCaller' to be of a generic class type, so it is compatible with the right hand operand '->*'
		return (mp_caller->*mp_memberFunction)(args...);
	}

	template <typename ReturnType, typename... Args>
	template <typename TInstance, typename TMemberFunction>
	void CConnection<ReturnType(Args...)>::Bind(TInstance* instance, TMemberFunction function)
	{
		assert(sizeof(TMemberFunction) == sizeof(TMemberFunctionPtr));
		// We are casting the type of 'function' to the generic type TMemberFunctionPtr
		// so we don't need to keep the original type TInstance
		mp_caller = reinterpret_cast<GenericClass *>(instance);
		mp_memberFunction = reinterpret_cast<TMemberFunctionPtr>(function);
	}

	/*
	We use this method on copy constructor and copy assignment operator because we want only to transfer the pointers,
	no allocations are involved
	*/
	template <typename ReturnType, typename... Args>
	void CConnection<ReturnType(Args...)>::PassParameters(const TConnection& copy)
	{
		mp_bindedSignal = copy.mp_bindedSignal;
		if (copy.mp_function)
		{
			mp_function = copy.mp_function;
			Bind(this, copy.mp_memberFunction);
		}
		else
		{
			mp_function = 0;
			Bind(copy.mp_caller, copy.mp_memberFunction);
		}
	}

	template <typename ReturnType, typename... Args>
	template <typename T, typename TMemberFunction>
	const bool CConnection<ReturnType(Args...)>::Equals(T* caller, TMemberFunction function) const
	{
		return mp_caller == reinterpret_cast<GenericClass *>(caller)
			&& mp_memberFunction == reinterpret_cast<TMemberFunctionPtr>(function)
			&& mp_function == 0;
	}

	template <typename ReturnType, typename... Args>
	template <typename T>
	const bool CConnection<ReturnType(Args...)>::Equals(T& reference) const
	{
		return Equals(reinterpret_cast<TFunctionPtr>(&reference));
	}

	template <typename ReturnType, typename... Args>
	template <typename T>
	const bool CConnection<ReturnType(Args...)>::Equals(T* pointer) const
	{
		return mp_function == pointer;
	}

	template <typename ReturnType, typename... Args>
	template< typename T>
	ReturnType CConnection<ReturnType(Args...)>::InvokeTemplatizedFunctionPtr(Args... args) const
	{
		return (reinterpret_cast<T*>(mp_function)->operator())(args...);
	}

	template <typename ReturnType, typename... Args>
	ReturnType CConnection<ReturnType(Args...)>::InvokeFunctionPtr(Args... args) const
	{
		return (*mp_function)(args...);
	}

	template <typename ReturnType, typename... Args>
	void CConnection<ReturnType(Args...)>::Clear()
	{
		mp_bindedSignal = 0;
		mp_caller = 0;
		mp_memberFunction = 0;
		mp_function = 0;
	}
} /* namespace dc */
