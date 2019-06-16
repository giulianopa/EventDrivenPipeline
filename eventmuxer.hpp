#pragma once
#include "consciousobserver.hpp"
#include "event.hpp"
#include "mpl.hpp"
#include <type_traits>
#include <utility>
#include <bitset>
#include <mutex>
#include <tuple>
#include <cmath>
#include <map>


//! Event multiplexer.
/*!
	Observe mutiple events, and throw a single joint event when ALL occurred.
	Events are intended to be independent.
	\author Giuliano Pasqualotto
	\date 12/27/2013
*/
template<typename MsgType1, typename... OtherMsgType>
class EventMuxer :
	public EventSource<MsgType1, OtherMsgType...>,
	public ConsciousObserver<EventMuxer<MsgType1, OtherMsgType...>, MsgType1, OtherMsgType...>
{
private:

	typedef ConsciousObserver<EventMuxer<MsgType1, OtherMsgType...>, MsgType1, OtherMsgType...> InheritedObserver;

	//! Synchronization primitive: needed to prevent race conditions on evalAndEmit calls.
	std::mutex m_evaluationMutex;

	//! Messages data.
	/*!
		A container is defined for each event.
	*/
	std::tuple<MsgType1, OtherMsgType...> m_msgData;

	//! List of flags, set (to 1) when a given event occurs.
	std::bitset<sizeof...(OtherMsgType) + 1> m_notified;

	//! Convert the std::tuple back to a variadic list of args and emit the joint event.
	template<int ...Seq>
	void unpackAndEmit(const viper::mpl::seq<Seq...>&) {
		emit(std::move(std::get<Seq>(m_msgData))...);
	}

	//! For each event occurred, evaluate if to emit the joint event or not. 
	void evalAndEmit()
	{
		std::lock_guard<std::mutex> guard(m_evaluationMutex);
		if (m_notified.all())
		{
			// "Unpack" tuple
			const viper::mpl::gen_seq<sizeof...(OtherMsgType)+1> msgSeq;
			unpackAndEmit(msgSeq);
			m_notified.reset();
		}
	}

public:

	//! Constructor.
	EventMuxer() :
		ConsciousObserver<EventMuxer<MsgType1, OtherMsgType...>, MsgType1, OtherMsgType...>(*this)
	{
		m_notified.reset();
	}

	//! Destructor.
	~EventMuxer() { }

	//! Merge a set of statistically independent events.
	/*!
		This guy checks for the data types. The check is performed at compile-time.
		\param Variadic argument list: one event for each template type.
	*/
	void merge(const Event<MsgType1> &evt1, const Event<OtherMsgType>&... otherEvts)
	{
		InheritedObserver::subscribe(evt1, otherEvts...);
	}

	//! Subscribe to the joint event.
	void subscribe(Observer<MsgType1, OtherMsgType...> &ob)
	{
		Event<MsgType1, OtherMsgType...>::subscribe(ob);
	}

	//! ConsciuosObserver interface function.
	/*!
		\param ID of the event calling the observer.
		\param msg rvalue to the received message.
	*/
	template<size_t ObserverId, typename MsgType>
	void observe(EVENT_ID id, const MsgType&& msg)
	{
		// Store data
		std::get<ObserverId>(m_msgData) = std::move(msg);

		// Set notification flag
		m_notified.set(ObserverId);
		evalAndEmit();
	}
};
