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

#pragma once

#include <vector>
#include <iterator>
#include <utility>
#include <algorithm>

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
		//---------------------------------------------------------------------
		// Type definitions
		//---------------------------------------------------------------------
	public:
		using TConnection = CConnection<ReturnType(Args...)>;
		using TSignal = CSignal<ReturnType(Args...)>;
		
		//---------------------------------------------------------------------
		// Getters / Setters
		//---------------------------------------------------------------------
	public:
		const bool		IsEmpty() const	{ return Count() == 0; }

		const unsigned	Count() const	{ return m_connections.size(); }
		
		//---------------------------------------------------------------------
		// Constructors / Destructors
		//---------------------------------------------------------------------
	public:
		CSignal() {}

		~CSignal() { Clear(); }
		
		CSignal(const TSignal& copy) : m_connections(copy.m_connections) { }

		CSignal(CSignal&& other) : TSignal() { swap(*this, other); }
		
		TSignal& operator= (TSignal signal);

	private:
		friend void swap(TSignal& first, TSignal& second)
		{
			using std::swap;
			swap(first.m_connections, second.m_connections);
		}

		//---------------------------------------------------------------------
		// Functions
		//---------------------------------------------------------------------
	public:

		template <typename T>
		TConnection& Connect(const T& slotRef);

		template <typename T>
		TConnection& Connect(const T* slotPtr);

		template<typename T>
		TConnection& Connect(T* caller, ReturnType(T::* function) (Args...));
		
		template<typename T>
		TConnection& Connect(const T* caller, ReturnType(T::* function) (Args...) const);
		
		template<typename T>
		void Disconnect(const T& slotRef);
		
		template<typename T>
		void Disconnect(const T* slotPtr);
		
		template<typename T>
		void Disconnect(T* caller, ReturnType(T::* function) (Args...));
		
		template<typename T>
		void Disconnect(const T* caller, ReturnType(T::* function) (Args...) const);

		void Disconnect(const TConnection& connection);
		
		void Clear();
		
		const bool operator() (Args... args);

		const bool operator() (Args... args) const;
		
		template<typename TArray>
		const bool operator() (TArray& valuesArray, Args... args);
		
		template<typename TArray>
		const bool operator() (TArray& valuesArray, Args... args) const;
		
	private:
		std::vector<TConnection> m_connections;

	};

	template<typename ReturnType, typename... Args>
	CSignal<ReturnType(Args...)>& CSignal<ReturnType(Args...)>::operator= (TSignal signal)
	{
		// Using copy-and-swap idiom
		swap(*this, signal);

		return *this;
	}

	template<typename ReturnType, typename... Args>
	template <typename T>
	CConnection<ReturnType(Args...)>& CSignal<ReturnType(Args...)>::Connect(const T& slotRef)
	{
		m_connections.push_back(TConnection(this, const_cast<T&>(slotRef)));
		return m_connections.back();
	}

	template<typename ReturnType, typename... Args>
	template <typename T>
	CConnection<ReturnType(Args...)>& CSignal<ReturnType(Args...)>::Connect(const T* slotPtr)
	{
		m_connections.push_back(TConnection(this, const_cast<T*>(slotPtr)));
		return m_connections.back();
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	CConnection<ReturnType(Args...)>& CSignal<ReturnType(Args...)>::Connect(T* caller, ReturnType(T::* function) (Args...))
	{
		m_connections.push_back(TConnection(this, caller, function));
		return m_connections.back();
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	CConnection<ReturnType(Args...)>& CSignal<ReturnType(Args...)>::Connect(const T* caller, ReturnType(T::* function) (Args...) const)
	{
		// Since we know that the member function is const, it's safe to
		// remove the const qualifier from the 'caller' pointer with a const_cast.
		m_connections.push_back(TConnection(this, const_cast<T*>(caller), function));
		return m_connections.back();
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	void CSignal<ReturnType(Args...)>::Disconnect(const T& ref)
	{
		T& nonConstRef = const_cast<T&>(ref);
		for (const auto& connection : m_connections)
		{
			if (connection.Equals(nonConstRef))
			{
				Disconnect(connection);
				return;
			}
		}
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	void CSignal<ReturnType(Args...)>::Disconnect(const T* ptr)
	{
		T* nonConstPtr = const_cast<T*>(ptr);
		for (const auto& connection : m_connections)
		{
			if (connection.Equals(nonConstPtr))
			{
				Disconnect(connection);
				return;
			}
		}
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	void CSignal<ReturnType(Args...)>::Disconnect(T* ptr, ReturnType(T::* function) (Args...))
	{
		for (const auto& connection : m_connections)
		{
			if (connection.Equals(ptr, function))
			{
				Disconnect(connection);
				return;
			}
		}
	}

	template<typename ReturnType, typename... Args>
	template<typename T>
	void CSignal<ReturnType(Args...)>::Disconnect(const T* ptr, ReturnType(T::* function) (Args...) const)
	{
		// Since we know that the member function is const, it's safe to
		// remove the const qualifier from the 'caller' pointer with a const_cast.
		T* nonConstPtr = const_cast<T*>(ptr);
		for (const auto& connection : m_connections)
		{
			if (connection.Equals(nonConstPtr, function))
			{
				Disconnect(connection);
				return;
			}
		}
	}

	template<typename ReturnType, typename... Args>
	void CSignal<ReturnType(Args...)>::Disconnect(const TConnection& connection)
	{
		const auto& end = m_connections.end();
		const auto& it = std::find(m_connections.begin(), end, connection);

		m_connections.erase(it);
	}

	template<typename ReturnType, typename... Args>
	void CSignal<ReturnType(Args...)>::Clear()
	{
		m_connections.clear();
	}

	template<typename ReturnType, typename... Args>
	const bool CSignal<ReturnType(Args...)>::operator() (Args... args)
	{
		for (const TConnection& connection : m_connections)
		{
			connection(args...);
		}
		return true;
	}

	template<typename ReturnType, typename... Args>
	const bool CSignal<ReturnType(Args...)>::operator() (Args... args) const
	{
		for (const TConnection& connection : m_connections)
		{
			connection(args...);
		}
		return true;
	}

	template<typename ReturnType, typename... Args>
	template<typename TArray>
	const bool CSignal<ReturnType(Args...)>::operator() (TArray& valuesArray, Args... args)
	{
		int index = 0;
		for (const TConnection& connection : m_connections)
		{
			valuesArray[index++] = connection(args...);
		}
		return true;
	}

	template<typename ReturnType, typename... Args>
	template<typename TArray>
	const bool CSignal<ReturnType(Args...)>::operator() (TArray& valuesArray, Args... args) const
	{
		int index = 0;
		for (const TConnection& connection : m_connections)
		{
			valuesArray[index++] = connection(args...);
		}
		return true;
	}
}
