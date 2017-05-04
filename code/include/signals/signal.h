/*
 The MIT License (MIT)
 
 Copyright (c) 2016 Jorge López González
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * signal.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: Jorge López González
 * 
 * Inspired by the solutions from:
 * - pbhogan (https://github.com/pbhogan/Signals)
 * - NoAvailableAlias (https://github.com/NoAvailableAlias/nano-signal-slot)
 */

#ifndef DC_SIGNALS_SIGNAL_H_
#define DC_SIGNALS_SIGNAL_H_

#include <forward_list>
#include <vector>
#include <iterator>
#include <utility>

#include "connection.h"

namespace dc
{
	/*
	 CSIGNAL
	 
	 This class is a factory of CConnection and also a manager for them.
	 */
	 
	template<typename ReturnType, typename... Args>
	class CSignal<ReturnType(Args...)>
	{
		//------------------------------------------------------------------------------------------------------------------------
		// Type definitions
		//------------------------------------------------------------------------------------------------------------------------
	public:
		using TConnection = CConnection<ReturnType(Args...)>;
		
		//------------------------------------------------------------------------------------------------------------------------
		// Getters / Setters
		//------------------------------------------------------------------------------------------------------------------------
	public:
		const bool IsEmpty() const
		{
			return ConnectionsCount() == 0;
		}
		
		const long ConnectionsCount() const
		{
			return std::distance(m_connections.begin(), m_connections.end());
		}
	 
		//------------------------------------------------------------------------------------------------------------------------
		// Constructors / Destructors
		//------------------------------------------------------------------------------------------------------------------------
	public:
		CSignal() {}
		
		CSignal(const CSignal<ReturnType(Args...)>& copy) : m_connections(copy.m_connections)
		{}
		
		~CSignal()
		{
			Clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		// Functions
		//------------------------------------------------------------------------------------------------------------------------
	public:
		template <typename T>
		TConnection
		Connect(const T& slotRef)
		{
			CConnection<ReturnType(Args...)> connection(this, const_cast<T&>(slotRef));
			m_connections.push_front(connection);
			return connection;
		}
		
		template <typename T>
		TConnection
		Connect(T& slotRef)
		{
			CConnection<ReturnType(Args...)> connection(this, slotRef);
			m_connections.push_front(connection);
			return connection;
		}

		template <typename T>
		TConnection
		Connect(T* slotPtr)
		{
			CConnection<ReturnType(Args...)> connection(this, slotPtr);
			m_connections.push_front(connection);
			return connection;
		}

		template<typename T>
		TConnection
		Connect(T* caller, ReturnType (T::* function) (Args...))
		{
			CConnection<ReturnType(Args...)> connection(this, caller, function);
			m_connections.push_front(connection);
			return connection;
		}

		template<typename T>
		TConnection
		Connect(T* caller, ReturnType (T::* function) (Args...) const)
		{
			CConnection<ReturnType(Args...)> connection(this, caller, function);
			m_connections.push_front(connection);
			return connection;
		}
		
		template<typename T>
		TConnection
		Connect(const T* caller, ReturnType (T::* function) (Args...) const)
		{
			// Since we know that the member function is const, it's safe to
			// remove the const qualifier from the 'caller' pointer with a const_cast.
			CConnection<ReturnType(Args...)> connection(this, const_cast<T*>(caller), function);
			m_connections.push_front(connection);
			return connection;
		}
		
		template<typename T>
		void Disconnect(const T& ref)
		{
			Disconnect(const_cast<T&>(ref));
		}

		template<typename T>
		void Disconnect(T& ref)
		{
			for (const auto& connection : m_connections)
			{
				if(connection.Equals(ref))
				{
					Disconnect(connection);
					return;
				}
			}
		}
		
		template<typename T>
		void Disconnect(T* ptr)
		{
			for (const auto& connection : m_connections)
			{
				if(connection.Equals(ptr))
				{
					Disconnect(connection);
					return;
				}
			}
		}
		
		template<typename T>
		void Disconnect(T* ptr, ReturnType (T::* function) (Args...))
		{
			for (const auto& connection : m_connections)
			{
				if(connection.Equals(ptr, function))
				{
					Disconnect(connection);
					return;
				}
			}
		}
		
		template<typename T>
		void Disconnect(T* ptr, ReturnType (T::* function) (Args...) const)
		{
			for (const auto& connection : m_connections)
			{
				if(connection.Equals(ptr, function))
				{
					Disconnect(connection);
					return;
				}
			}
		}
		
		template<typename T>
		void Disconnect(const T* ptr, ReturnType (T::* function) (Args...) const)
		{
			// Since we know that the member function is const, it's safe to
			// remove the const qualifier from the 'caller' pointer with a const_cast.
			Disconnect(const_cast<T*>(ptr), function);
		}

		void Disconnect(TConnection& connection)
		{
			m_connections.remove(connection);
		}
		
		void Disconnect(const TConnection& connection)
		{
			m_connections.remove(connection);
		}
		
		void Clear()
		{
			m_connections.clear();
		}
		
	private:
		inline const bool Execute(Args&&... args)
		{
			for(const auto& connection : m_connections)
			{
				connection(std::forward<Args>(args)...);
			}
			return true;
		}

		inline const bool Execute(Args&&... args) const
		{
			for(const auto& connection : m_connections)
			{
				connection(std::forward<Args>(args)...);
			}
			return true;
		}
		//------------------------------------------------------------------------------------------------------------------------
		// Overrided operators
		//------------------------------------------------------------------------------------------------------------------------
	public:
		inline const bool operator() (Args&... args)
		{
			return Execute(std::forward<Args>(args)...);
		}
		
		inline const bool operator() (Args&... args) const
		{
			return Execute(std::forward<Args>(args)...);
		}
		
		template<typename TArray>
		const bool operator() (TArray& valuesArray, Args&&... args) const
		{
			int index = 0;
			for(const auto& connection : m_connections)
			{
				valuesArray[index++] = connection(std::forward<Args>(args)...);
			}
			return true;
		}
		
		inline
		void operator= (const CSignal<ReturnType(Args...)>& copy)
		{
			this(copy);
		}
		
	private:
		std::forward_list<TConnection> m_connections;

	};
}

#endif /* DC_SIGNALS_SIGNAL_H_ */
