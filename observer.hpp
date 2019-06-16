#pragma once


typedef int EVENT_ID;


//! Observer interface.
/*!
	\author Giuliano Pasqualotto
	\date 12/18/2013
	\todo Open issues: a vector of boost::any objects has to be created every time an event is generate. The goal is however to have a "perfect forwarding".
		This is not currently possible at the subclass level (e.g. muxer) because it requires an overload of the observer function for each type in the parameter pack.
		The proposed solution is thus sub-obtimal.
*/
template<typename MsgType1, typename... OtherMsgs>
class Observer
{
public:
	virtual ~Observer() { }

	//! Variadic observer function. Observe at least one event.
	virtual void observe(EVENT_ID id, const MsgType1&& msg, const OtherMsgs&&... otherMsgs) = 0;
};
