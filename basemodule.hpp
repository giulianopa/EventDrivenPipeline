#pragma once
#include "event.hpp"
#include "observer.hpp"
#include "eventmuxer.hpp"


//! Generic definition of a base module.
/*!
	Variadic template class.
	Each module must have one output type and 1 ore more input types.
	\author Giuliano Pasqualotto
	\date 11/22/2013
*/
template<typename OutType, typename InType1, typename... OtherInTypes>
class BaseModule : public EventSource<OutType>,  public Observer<InType1, OtherInTypes...>
{
protected:

	//! Composed event, which triggers the processing.
	EventMuxer<InType1, OtherInTypes...> m_processEvts;

	//! Module output.
	OutType m_output;

	//! Observe one or more input events.
	/*!

	*/
	void observe(EVENT_ID id, const InType1 &&evt1, const OtherInTypes&&... otherEvents)
	{
		/*
			process() should be submitted to the ThreadPool.
			Main reasons:
			 - More accurate profiling
			 - Controlling overhead
			 - Improve parallelism
		*/
		process(m_output, evt1, otherEvents...);
		emit(m_output);
	}

public:

	BaseModule()
	{
		// The event muxer has to call the proper member function when all the events are triggered. 
		m_processEvts.subscribe(*static_cast<Observer<InType1, OtherInTypes...> *>(this));
	}

	void setInput(const Event<InType1> &evt1, const Event<OtherInTypes>&... otherIn)
	{
		// Each input event is managed by the event muxer object 
		m_processEvts.merge(evt1, otherIn...);

		/*
			Trigger initialization?
		*/
	}

	typedef int RESULT;
	
	//! Processing function.
	/*!
		Invoked every time all the input events are thrown. The arguments are forwarded by the event sources.
		DON'T CALL THIS FUNCTION.
	*/
	virtual RESULT process(OutType &out, const InType1 &in1, const OtherInTypes&... otherIn) = 0;
};
