/*
 * signal.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: jorge
 */

#ifndef DC_SIGNALS_SIGNAL_H_
#define DC_SIGNALS_SIGNAL_H_

#import <forward_list>

#import "connection.h"

namespace dc
{
	//template<typename ReturnType>
	//class CSignal;

	template<typename ReturnType, typename... Args>
	class CSignal<ReturnType(Args...)>
	{
	
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
		Connect(T* ptr, ReturnType (T::* function) (Args...))
		{
			CConnection<ReturnType(Args...)> connection(this, ptr, function);
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
		
		void operator() (Args&&... args) const
		{
			for(const CConnection<ReturnType(Args...)>& connection : m_connections)
			{
				connection(std::forward<Args>(args)...);
			}
		}
		
	private:
		std::forward_list<CConnection<ReturnType(Args...)>> m_connections;

	};
}

#endif /* DC_SIGNALS_SIGNAL_H_ */
