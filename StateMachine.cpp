#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include<iostream>
#include<map>
#include<vector>
#include<memory>
#include<mutex>
#include<thread>
#include<ThreadPool.cpp>


template<typename T> class StateMachineBuilder;// fwd declaration

/**
 * State machine class to store and process the events and states
 */
template<typename T>
class StateMachine
{
    public:

        /*
         * Get builder class to build the state machine.
         * addTransition() can be used to add to the state machine.
         */
        static std::unique_ptr<StateMachineBuilder<T> > build(T* t, int initialState)
        {
            return std::make_unique<StateMachineBuilder<T> >(t, initialState);
        }

        /*
         * Send event to the stateMachine to handle.
         */
        int sendEvent(int event) {
            return processEvent(event);
        }
        
        void sendEventAsyncUnblocked(int event) {
            processEventAsync(event);
        }
        
        void sendEventAsyncBlocked(int event) {
            std::lock_guard<std::mutex> guard(mutexAsync);
            if( mAsyncThread != nullptr )
                mAsyncThread->join();
            mAsyncThread = new std::thread(&StateMachine<T>::processEvent,this,event);
        }
    private:
        /**
         * A Handler function should return a int and accepts nothing.
         */
        typedef int (T::*Handler)(void);
       
        /**
         * State class represents a state.
         * This will have info of eligible events to be handled
         * Their coresponding handler function and nextState after the
         * handler exec.
         */
        class Transition {
            private:
                //Event that can be handled
                int event;
                //Next state afte handler exec
                int finalState;
                //Functionpointer to handler function
                Handler handle;
                //Private defaukt constructor
                Transition() = delete;
                Transition& operator=(const Transition& other) = delete;
            public:
                /*
                 * Constructor to init a state
                 */
                Transition(int event, int state, Handler handle):event(event), finalState(state), handle(handle) {
                }

                ~Transition()
                {
                }

                /**
                 * Copy constructor
                 */
                Transition(const Transition& state)
                {
                    this->event = state.event;
                    this->handle = state.handle;
                    this->finalState = state.finalState;
                }

                /**
                 * Overloaded overloaded operator ==
                 */
                bool operator==(const int event )
                {
                    return this->event == event;
                }

                /**
                 * Overloaded overloaded operator ==
                 */
                bool operator==(const Transition& state )
                {
                    return this->event == state.event;
                }

                /**
                 * Returns handler function coresponding to transition
                 */
                Handler getHandler()
                {
                    return handle;
                }

                /*
                 * Return next state coresponding to the transition
                 */
                int getNextState()
                {
                    return finalState;
                }
        };
       
        /*
         * Current state of state machine
         */
        int currentState;

        /*
         * Class pointer who implements handler
         */
        T* handlerClass;

        /**
         * Avaoid simultanious send events
         */
        std::mutex mutex;
        std::mutex mutexAsync;
        ThreadPool<T>* mThreadPool;
        /**
         * Multimap to store the transitions
         */
        std::multimap<int, Transition> stateMachine;

        std::thread *mAsyncThread = nullptr;

        /**
         * Constructor to init state machine
         * InitialState - The initial state of the machine
         * clazz - Class pointer which implements handler function
         */
        StateMachine(int initialState, T* clazz)
        {
            currentState = initialState;
            handlerClass = clazz;
            mThreadPool = new ThreadPool<T>(std::thread::hardware_concurrency());
        }

        StateMachine() = delete;
        StateMachine operator=(const StateMachine& rhs) = delete;
        StateMachine(const StateMachine& other) = delete;
        /*
         * Event processor, finds the coresponding Transtition structur
         * and the calles handler function and moves the currentState to 
         * the next state based on the event.
         *
         * TODO:: add process event async
         */
        int processEvent(int event)
        {
            std::lock_guard<std::mutex> guard(mutex);
            int retVal = -1;
            bool isHandled = false;

            for( auto itr = stateMachine.find(currentState); itr != stateMachine.end() ; itr++ )
            {
                if(itr->second == event )
                {
                    Handler handle = itr->second.getHandler();
                    retVal = (handlerClass->*handle)();
                    currentState = itr->second.getNextState();
                    isHandled = true;
                    break;
                }
            }
            if( !isHandled )
                std::cout<<"Event dropped Event: "<< event << " Current state : "<<currentState << std::endl;
            return retVal;
        }

        void processEventAsync(int event)
        {
            std::lock_guard<std::mutex> guard(mutex);
            bool isHandled = false;

            for( auto itr = stateMachine.find(currentState); itr != stateMachine.end() ; itr++ )
            {
                if(itr->second == event )
                {
                    Handler handle = itr->second.getHandler();
                    Job<T> job(handlerClass, handle);
                    mThreadPool->addToPool(job);
                    //std::thread* fireThread = new std::thread(handle, handlerClass);
                    currentState = itr->second.getNextState();
                    isHandled = true;
                    break;
                }
            }
            if( !isHandled )
                std::cout<<"Event dropped Event: "<< event << " Current state : "<<currentState << std::endl;
        }

        /*
         * Adds a transition to the state machine.
         */
        void addTransition(int from, int to, int event, Handler handler)
        {
            Transition transition(event, to, handler);
            stateMachine.insert(std::make_pair(from, transition));
        }

        //THis guy access the private members
        friend class StateMachineBuilder<T>;
};

/**
 * State machine bulder class.
 * addTransition to build the state machine.
 * get StateMAchine claass object to send the event to the state machine.
 */
template <typename T>
class StateMachineBuilder {
    //Private state machine object. 
    StateMachine<T>* mMachine;
    StateMachineBuilder() = delete;
    StateMachineBuilder& operator= (const StateMachineBuilder& rhs) = delete;
    StateMachineBuilder(const StateMachineBuilder& other) = delete;
    public:

    /**
     * Bulder constructio. TODO:: make it private
     */
    StateMachineBuilder(T* t, int initialState)
    {
        mMachine = new StateMachine<T>(initialState, t);
    }

    /*
     * Handler defination, it shall return a int and accept nothing.
     */
    typedef int (T::*Handler)(void);

    /**
     * Add transition to the state machine.
     * from - From state
     * to - next state
     * event - On which event
     * handler - Handler function to be called while transition.
     */
    StateMachineBuilder& addTransition(int from, int to, int event, Handler handler)
    {
        mMachine->addTransition(from,to, event, handler);
        return *this;
    }

    /*
     * Get the state machine object to send events.
     */
    StateMachine<T>& getStateMachine()
    {
        return *mMachine;
    }
};

#endif
