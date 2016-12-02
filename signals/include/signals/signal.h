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
		
	public:
		const bool IsEmpty() const
		{
			return ConnectionsCount() == 0;
		}
		
		const long ConnectionsCount() const
		{
			return std::distance(m_connections.begin(), m_connections.end());
		}
	 
	public:
		CSignal() {}
		
		~CSignal()
		{
			Clear();
		}

	public:
		
		template <typename T>
		CConnection<ReturnType(Args...)>
		Connect(T& slotRef)
		{
			CConnection<ReturnType(Args...)> connection(this, slotRef);
			m_connections.push_front(connection);
			return connection;
		}

		template <typename T>
		CConnection<ReturnType(Args...)>
		Connect(T* slotPtr)
		{
			CConnection<ReturnType(Args...)> connection(this, slotPtr);
			m_connections.push_front(connection);
			return connection;
		}

		template<typename T>
		CConnection<ReturnType(Args...)>
		Connect(T* caller, ReturnType (T::* function) (Args...))
		{
			CConnection<ReturnType(Args...)> connection(this, caller, function);
			m_connections.push_front(connection);
			return connection;
		}

		template<typename T>
		CConnection<ReturnType(Args...)>
		Connect(T* caller, ReturnType (T::* function) (Args...) const)
		{
			CConnection<ReturnType(Args...)> connection(this, caller, function);
			m_connections.push_front(connection);
			return connection;
		}
		
		template<typename T>
		CConnection<ReturnType(Args...)>
		Connect(const T* caller, ReturnType (T::* function) (Args...) const)
		{
			// Since we know that the member function is const, it's safe to
			// remove the const qualifier from the 'caller' pointer with a const_cast.
			CConnection<ReturnType(Args...)> connection(this, const_cast<T*>(caller), function);
			m_connections.push_front(connection);
			return connection;
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

		void Disconnect(CConnection<ReturnType(Args...)>& connection)
		{
			m_connections.remove(connection);
		}
		
		void Disconnect(const CConnection<ReturnType(Args...)>& connection)
		{
			m_connections.remove(connection);
		}
		
		void Clear()
		{
			m_connections.clear();
		}
		
		const bool operator() (Args&&... args) const
		{
			for(const CConnection<ReturnType(Args...)>& connection : m_connections)
			{
				connection(std::forward<Args>(args)...);
			}
			return true;
		}
		
		template<typename TArray>
		const bool operator() (TArray& valuesArray, Args&&... args) const
		{
			int index = 0;
			for(const CConnection<ReturnType(Args...)>& connection : m_connections)
			{
				valuesArray[index++] = connection(std::forward<Args>(args)...);
			}
			return true;
		}
		
	private:
		std::forward_list<CConnection<ReturnType(Args...)>> m_connections;

	};
}

#endif /* DC_SIGNALS_SIGNAL_H_ */
