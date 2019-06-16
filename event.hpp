#pragma once
#include <vector>
#include <memory>
#include <typeinfo>
#include <functional>
#include <tuple>
#include "observer.hpp"


//! Abstract event.
/*!
	Emit a message without holding any value.
	\date 11/21/2013
	\author Giuliano Pasqualotto
*/
template <typename MsgType1, typename... OtherMsgType>
class Event
{
protected:

	//! List of awaiting observers.
	mutable std::vector<Observer<MsgType1, OtherMsgType...> *> m_observers;

public:

	//! Unique ID of the event.
	const EVENT_ID id;

	//! Event constructor.
	Event() : id(EventIDGenerator::createID()) {}

	//! Subscribe to an event
	/*!
		\param An observer which needs to be notified when the event occurs.
	*/
	EVENT_ID subscribe(Observer<MsgType1, OtherMsgType...> &ob) const
	{
		m_observers.push_back(&ob);
		return id;
	}
};


//! Event ID generator class.
class EventIDGenerator
{
	static std::shared_ptr<EventIDGenerator> m_instance;
	EVENT_ID m_nextID;
	EventIDGenerator() : m_nextID(0) { }
	EVENT_ID next()
	{
		m_nextID++;
		return (m_nextID - 1);
	}

public:

	static EVENT_ID createID()
	{
		if (!m_instance)
			m_instance = std::shared_ptr<EventIDGenerator>(new EventIDGenerator);
		return m_instance->next();
	}
};


//! Event source.
/*!
	\date 11/21/2013
	\author Giuliano Pasqualotto
*/
template <typename MsgType1, typename... OtherMsgType>
class EventSource : public Event<MsgType1, OtherMsgType...>
{
public:

	EventSource() : Event<MsgType1, OtherMsgType...>() {}
	virtual ~EventSource() {}
	void emit(const MsgType1& msg, const OtherMsgType&... otherMsgs)
	{
		for (size_t observerId = 0; observerId < m_observers.size(); observerId++)
		{
			m_observers[observerId]->observe(id, std::forward<const MsgType1>(msg), std::forward<const OtherMsgType>(otherMsgs)...);
		}
	}
	void emit(const MsgType1&& msg, const OtherMsgType&&... otherMsgs)
	{
		for (size_t observerId = 0; observerId < m_observers.size(); observerId++)
		{
			m_observers[observerId]->observe(id, std::forward<const MsgType1>(msg), std::forward<const OtherMsgType>(otherMsgs)...);
		}
	}
};
