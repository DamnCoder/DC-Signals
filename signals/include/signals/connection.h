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

#ifndef DC_SIGNALS_CONNECTION_H_
#define DC_SIGNALS_CONNECTION_H_

namespace dc
{
	// Forward declaration for the signals
	template<typename ReturnType, typename... Args>
	class CSignal;
	
	// GenericClass is a fake class, ONLY used to provide a type.
	class GenericClass;

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
		using TMemberFunctionPtr = ReturnType (GenericClass::*)(Args...);
		using TFunctionPtr = ReturnType (*)(Args...);
		
		//------------------------------------------------------------------------------------------------------------------------
		// Getters / Setters
		//------------------------------------------------------------------------------------------------------------------------
	public:
		const bool IsConnected() const
		{
			if(m_pBindedSignal && m_pCaller && m_pMemberFunction && m_pFunction)
				return true;
			
			// m_pFunction is not zero only when you connect a free or static function
			return m_pBindedSignal && m_pCaller && m_pMemberFunction;
		}
	
		//------------------------------------------------------------------------------------------------------------------------
		// Constructors / Destructors
		//------------------------------------------------------------------------------------------------------------------------
	public:
		CConnection(const CConnection<ReturnType(Args...)>& copy)
		{
			m_pBindedSignal = copy.m_pBindedSignal;
			if(copy.m_pFunction)
			{
				m_pFunction = copy.m_pFunction;
				Bind(this, copy.m_pMemberFunction);
			}
			else
			{
				m_pCaller = copy.m_pCaller;
				m_pMemberFunction = copy.m_pMemberFunction;
				m_pFunction = 0;
			}
		}
		
		~CConnection()
		{
			Clear();
		}
		
	private:
		template <typename T>
		CConnection(CSignal<ReturnType(Args...)>* signal, T& ref):
			m_pBindedSignal(signal),
			m_pCaller(0),
			m_pMemberFunction(0),
			m_pFunction(0)

		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(&ref);
			Bind(this, &CConnection::InvokeTemplatizedFunctionPtr<T>);
		}
		
		template <typename T>
		CConnection(CSignal<ReturnType(Args...)>* signal, T* functionPtr):
			m_pBindedSignal(signal),
			m_pCaller(0),
			m_pMemberFunction(0),
			m_pFunction(0)

		{
			m_pFunction = functionPtr;
			Bind(this, &CConnection::InvokeFunctionPtr);
		}
		
		template <typename TInstance, typename TMemberFunction>
		CConnection(CSignal<ReturnType(Args...)>* signal, TInstance* instance, TMemberFunction function):
			m_pBindedSignal(signal),
			m_pCaller(0),
			m_pMemberFunction(0),
			m_pFunction(0)
		{
			Bind(instance, function);
		}
		
		//------------------------------------------------------------------------------------------------------------------------
		// Functions
		//------------------------------------------------------------------------------------------------------------------------
	public:
		inline
		void Disconnect()
		{
			// We need to dereference ourselves to properly use the disconnection
			m_pBindedSignal->Disconnect(*this);
			Clear();
		}

		inline
		const bool operator== (const CConnection<ReturnType(Args...)>& connection) const
		{
			if(this == &connection) return true;
			if(m_pCaller == connection.m_pCaller) return true;
			
			return m_pBindedSignal == connection.m_pBindedSignal
				&& m_pFunction == connection.m_pFunction
				&& m_pMemberFunction == connection.m_pMemberFunction;
		}
		
		// For efficiency, prevent creation of a temporary
		inline
		void operator= (const CConnection<ReturnType(Args...)>& connection)
		{
			this(connection);
		}
		
	private:
		inline
		ReturnType operator() (Args... args) const
		{
			// Here is the reason why we need 'm_pCaller' to be of a generic class type, so it is compatible with the right hand operand '->*'
			return (m_pCaller->*m_pMemberFunction)(args...);
		}
		
		template <typename TInstance, typename TMemberFunction>
		inline
		void Bind(TInstance* instance, TMemberFunction function)
		{
			// We are casting the type of 'function' to the generic type TMemberFunctionPtr
			// so we don't need to keep the original type TInstance
			m_pCaller = reinterpret_cast<GenericClass *>(instance);
			m_pMemberFunction = reinterpret_cast<TMemberFunctionPtr>(function);
		}
		
		//------------------------------------------------------------------------------------------------------------------------
		// Equals to check wether the function passed is the same as the one keeped in the connection
		//------------------------------------------------------------------------------------------------------------------------
		
		template <typename T, typename TMemberFunction>
		inline
		const bool Equals(T* caller, TMemberFunction function) const
		{
			return m_pCaller == reinterpret_cast<GenericClass *>(caller)
				&& m_pMemberFunction == reinterpret_cast<TMemberFunctionPtr>(function)
				&& m_pFunction == 0;
		}
		
		template <typename T, typename TMemberFunction>
		inline
		const bool Equals(const T* caller, TMemberFunction function) const
		{
			return Equals (const_cast<T*>(caller), function);
		}
		
		template <typename T>
		inline
		const bool Equals(T& reference) const
		{
			return Equals(reinterpret_cast<TFunctionPtr>(&reference));
		}
		
		template <typename T>
		inline
		const bool Equals(T* pointer) const
		{
			return m_pFunction == pointer;
		}
		
		//------------------------------------------------------------------------------------------------------------------------
		// Helper functions to transform into member function binded functions, lambdas, functors, free and static functions
		//------------------------------------------------------------------------------------------------------------------------
		
		// Used when we want to call back binded functions, lambdas and functors
		template< typename T>
		inline
		ReturnType InvokeTemplatizedFunctionPtr(Args... args) const
		{
			return (reinterpret_cast<T*>(m_pFunction)->operator())(args...);
		}

		// Used when we want to call back free or static functions
		inline
		ReturnType InvokeFunctionPtr(Args... args) const
		{
			return (*m_pFunction)(args...);
		}
		
		inline
		void Clear()
		{
			m_pBindedSignal = 0;
			m_pCaller = 0;
			m_pMemberFunction = 0;
			m_pFunction = 0;
		}

	private:
		CSignal<ReturnType(Args...)>*	m_pBindedSignal;
		
		GenericClass*					m_pCaller;
		TMemberFunctionPtr				m_pMemberFunction;
		
		TFunctionPtr					m_pFunction;
	};
} /* namespace dc */

#endif /* DC_SIGNALS_CONNECTION_H_ */
