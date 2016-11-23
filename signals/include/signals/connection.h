/*
 * connection.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: jorge
 */

#ifndef DC_SIGNALS_CONNECTION_H_
#define DC_SIGNALS_CONNECTION_H_

#include <functional>
#include <memory>

namespace dc
{
	// Forward declaration for the signals
	template<typename ReturnType, typename... Args>
	class CSignal;
	
	// GenericClass is a fake class, ONLY used to provide a type.
	class GenericClass;

	template<typename ReturnType>
	class CConnection;
	
	/**
	 * Base class for connections. Its purpose is to act as an wrapper for all the callbacks
	 */
	template <typename ReturnType, typename... Args>
	class CConnection<ReturnType(Args...)>
	{
		friend class CSignal<ReturnType(Args...)>;
	protected:
		// Generic types for functions
		using TMemberFunctionPtr = ReturnType (GenericClass::*)(Args...);
		using TFunctionPtr = ReturnType (*)(Args...);
		
	public:
		const bool IsConnected() const
		{
			if(m_pBindedSignal && m_pCaller && m_pMemberFunction && m_pFunction)
				return true;
			
			// m_pFunction is not zero only when you connect a free or static function
			return m_pBindedSignal && m_pCaller && m_pMemberFunction;
		}
	
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
			m_pBindedSignal(signal)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(std::addressof(ref));
			Bind(this, &CConnection::InvokeStaticFunctionPtr<T>);
		}
		
		template <typename T>
		CConnection(CSignal<ReturnType(Args...)>* signal, T* ptr):
			m_pBindedSignal(signal)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(ptr);
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
		
		template <typename T, typename TMemberFunction>
		inline
		const bool Equals(T* caller, TMemberFunction function) const
		{
			return m_pCaller == reinterpret_cast<GenericClass *>(caller) && m_pMemberFunction == reinterpret_cast<TMemberFunctionPtr>(function) && m_pFunction == 0;
		}
		
		template <typename T, typename TMemberFunction>
		inline
		const bool Equals(const T* caller, TMemberFunction function) const
		{
			return m_pCaller == reinterpret_cast<GenericClass *>(const_cast<T*>(caller))
				&& m_pMemberFunction == reinterpret_cast<TMemberFunctionPtr>(function)
				&& m_pFunction == 0;
		}
		
		template <typename T>
		inline
		const bool Equals(T& reference) const
		{
			return Equals(std::addressof(reference));
		}
		
		template <typename T>
		inline
		const bool Equals(T* pointer) const
		{
			return m_pFunction == reinterpret_cast<TFunctionPtr>(pointer);
		}
		
		template< typename T>
		inline
		ReturnType InvokeStaticFunctionPtr(Args... args) const
		{
			return (reinterpret_cast<T*>(m_pFunction)->operator())(args...);
		}

		inline
		ReturnType InvokeFunctionPtr(Args... args) const
		{
			// NOTE!! Very important to have the * when calling the m_pFunction
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
