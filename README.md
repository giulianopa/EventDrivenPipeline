#EventDrivenPipeline

This code was written long time ago in my spare time, and never had the chance to use. I am putting it on Github just to have a backup. Maybe one day I will add better documentation and I will finish it.
I wanted to design a new framework for a processing pipeline (i.e., a graph of connected modules, each one with its own input and output), and I wanted to avoid the constraint of having to explicitly declare the topology. I wanted the (optimal) topology to be determined only by the modules input, at run-time. I wanted also to reuse the modules output, without using a "cache".


This work was somehow inspired by a beautiful (to me) article I read, that now seems completely unrelated: [Deprecating the Observer Pattern](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.186.8309&rep=rep1&type=pdf)


Here are the main concepts (each one with its own template class):
* *Event*: just an asynchronous message emitted by a source. A message is a combination of (at least) one or more objects or integral data types.
* *Event Source*: an entity that can emit an event (i.e., message). An observer can subscribe to an event source, to be notified when a new event is thrown, getting the message. E.g. a camera, a module, a clock source, etc.
* *Observer*: An entity that subscribes to an Event Source.
* *BaseModule*: It's an observer of one of multiple event sources (inputs), *and* an event source of a well defined message type (output). Every time input events are generated, a given function (i.e., process) is called, getting in input the event messages. The output is then "emitted".

Considerations:
* Pros:
  * There's no need to define a pipeline object. The only information needed is about connectivity, i.e. who's the event source of who.
  * Module logic is defined at compile time.
  * No "callback hell": each event simply triggers a sequence of calls (determined at compile-time).
  * No mutex: only one mutex is needed, the one to prevent multiple inputs to be notified at the same time (see EventMuxer class)
  * (almost) perfect forwarding: event messages are never copied, but a reference is simply forwarded (moved) to the observers.
  * Less code: less code needed to implement a module.
  * Readable: module code is easier to read (other than the "core" parts)
  * Scalable: the framework is intentionally super generic.
  * Easier to control/test performance: each process call is intended to be submitted to a thread pool, for an easy profiling (e.g. implementing a sort of CPU sampling) and to handle the overhead.
  * No need to implement a module to save intermediate output: some ideas can be tested simply implementing an EventSource or an Observer.
* Cons
  * More constraints when designing modules (e.g. only one output type allowed, although this constraint could be avoided)
  * "Core" issues could be harder to debug. E.g., if there's an invalid call to Module.setInput the dev gets a compiler error, referring to a template class.

This code was tested with VS2013 (VC12), and uses variadic templates (VS2013 and above) + boost MPL (v1.55.0) (because of the lack of support of C++11 features such as constexpr in VC12).
