#pragma once
#include <tuple>
#include "mpl.hpp"
#include "observer.hpp"
#include "boost/mpl/copy.hpp"
#include "boost/mpl/joint_view.hpp"


//---------------------------
// Observer for Event Muxers
//---------------------------


//! Observer wrapper for single messages.
/*!
	An event muxer inherits a single message observer for each single-message event.
	\author Giuliano Pasqualotto
*/
template <class EventMuxer, size_t Id, typename MsgType>
class ConsciousObserverBase : public Observer<MsgType>
{
	EventMuxer *m_pMuxer;
public:
	const size_t ID = Id;	//!< Observer ID.
	ConsciousObserverBase() : m_pMuxer(nullptr) { }
	void setMuxer(EventMuxer &muxer) { m_pMuxer = &muxer; }
	void observe(EVENT_ID id, const MsgType&& msg)
	{
		// Message forwarding.
		m_pMuxer->observe<Id, MsgType>(id, std::forward<const MsgType>(msg));
	}
};


//! Inherit observers incrementally. Each subclass has it's own Id, allowind to inherit multiple observers handling the same data type.
/*!
	Hey, my first metafunction works!!!
	\author Giuliano Pasqualotto
*/
template <class EventMuxer, typename... Args>
struct inherit_observers
{
	//! Insertion loop: recursion.
	template <size_t Id, class Type, class... A>
	struct insertion_loop : boost::mpl::joint_view<insertion_loop<Id + 1, A...>, boost::mpl::vector<ConsciousObserverBase<EventMuxer, Id, Type>>>
	{ };

	//! Insertion loop: base.
	template <size_t Id, class Type>
	struct insertion_loop<Id, Type> : boost::mpl::vector<ConsciousObserverBase<EventMuxer, Id, Type>>
	{ };

	//! Vector of Observer to inherit from.
	typedef typename insertion_loop<0, Args...> observers_list;

	//! Base type
	typedef typename boost::mpl::inherit_linearly<
		observers_list,
		boost::mpl::inherit<boost::mpl::_1, boost::mpl::_2>
	>::type type;
};


//! ConsciousObserver: wrapper for generic single message Observers.
/*!
	\author Giuliano Pasqualotto
*/
template<class EventMuxer, typename MsgType1, typename... OtherMsgType>
class ConsciousObserver :
	public inherit_observers<EventMuxer, MsgType1, OtherMsgType...>::type
{
private:

	//! List of inherited single-message observers.
	typedef typename inherit_observers<EventMuxer, MsgType1, OtherMsgType...>::observers_list ObserversList;

public:

	//----------
	// Functors
	//----------

	//! Initialization functor.
	/*!
		To be used with boost::mpl::for_each
	*/
	struct ObserverInit
	{
	private:
		ConsciousObserver &m_instance;	//!< Caller.
		EventMuxer &m_muxerRef;			//!!< Reference to the muxer.
	public:
		ObserverInit(ConsciousObserver &instance, EventMuxer &muxerRef) : m_instance(instance), m_muxerRef(muxerRef) { }
		template<size_t Id, typename Msg> void operator()(ConsciousObserverBase<EventMuxer, Id, Msg> &x)
		{
			static_cast<ConsciousObserverBase<EventMuxer, Id, Msg>*>(&m_instance)->setMuxer(m_muxerRef);
		}
	};

	//! Event subscription functor.
	/*!
		To be used with boost::mpl::for_each
	*/
	struct SubscribeFunc
	{
	private:
		ConsciousObserver &m_instance;												//!< Caller.
		std::tuple<const Event<MsgType1>&, const Event<OtherMsgType>&...> m_events;	//!< Events to subscribe to, stored in a tuple of references.
	public:
		SubscribeFunc(
			ConsciousObserver &instance, const Event<MsgType1> &&evt1, const Event<OtherMsgType>&&... otherEvts)
			: m_instance(instance), m_events(std::forward_as_tuple(evt1, otherEvts...)) { }
		template<size_t Id, typename Msg> void operator()(ConsciousObserverBase<EventMuxer, Id, Msg> &x)
		{
			// Subscribe the corresponding (inherited) observer to the event.
			std::get<Id>(m_events).subscribe(*static_cast<ConsciousObserverBase<EventMuxer, Id, Msg>*>(&m_instance));
		}
	};

	//! Constructor.
	/*!
		\param Caller muxer.
	*/
	ConsciousObserver(EventMuxer &ref)
	{
		// Set the event muxer for each single-message observer
		boost::mpl::for_each<ObserversList>(ObserverInit(*this, ref));
	}

	//! Subscribe the observer to the input events. 
	void subscribe(const Event<MsgType1> &evt1, const Event<OtherMsgType>&... otherEvts)
	{
		boost::mpl::for_each<ObserversList>(SubscribeFunc(*this, std::move(evt1), std::move(otherEvts)...));
	}
};