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
	template<typename T>
	class CBackwardsList
	{
		
	};
	
	template<typename ReturnType> class CSignal;

	template<typename ReturnType, typename... Args>
	class CSignal<ReturnType(Args...)>
	{
	private:
		
		template<typename T>
		static CConnection<ReturnType(Args...)>
		MakeConnection(T& slot)
		{
			CConnection<ReturnType(Args...)> connection;
			connection.Bind(slot);
			return connection;
		}
		
		template<typename T>
		static CConnection<ReturnType(Args...)>
		MakeConnection(T* slot)
		{
			CConnection<ReturnType(Args...)> connection;
			connection.Bind(slot);
			return connection;
		}
		
		template<typename T>
		static CConnection<ReturnType(Args...)>
		MakeConnection(T* ptr, ReturnType (T::* function) (Args...))
		{
			CConnection<ReturnType(Args...)> connection;
			connection.Bind(ptr, function);
			return connection;
		}
		
	public:
		CSignal() {}
		~CSignal() {}

	public:
		
		template <typename T>
		CConnection<ReturnType(Args...)>
		Connect(T& slotRef)
		{
			CConnection<ReturnType(Args...)> connection = MakeConnection(slotRef);
			m_connections.push_front(connection);
			return connection;
		}

		template <typename T>
		CConnection<ReturnType(Args...)>
		Connect(T* slotPtr)
		{
			CConnection<ReturnType(Args...)> connection = MakeConnection(slotPtr);
			m_connections.push_front(connection);
			return connection;
		}

		template<typename T>
		CConnection<ReturnType(Args...)>
		Connect(T* ptr, ReturnType (T::* function) (Args...))
		{
			CConnection<ReturnType(Args...)> connection = MakeConnection(ptr, function);
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
		
		void Disconnect()
		{
			m_connections.clear();
		}
		
		void operator() (Args&&... args) const
		{
			unsigned int index = 0;
			for(const CConnection<ReturnType(Args...)>& connection : m_connections)
			{
				printf("Emit %d\n", ++index);
				connection(std::forward<Args>(args)...);
			}
		}
		
	private:
		std::forward_list<CConnection<ReturnType(Args...)>> m_connections;

	};
}

#endif /* DC_SIGNALS_SIGNAL_H_ */
