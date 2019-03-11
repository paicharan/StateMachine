# StateMachine
This is a utility/framework class to create state machine based implementation. This provides the builder interface to construct
the state machine.
<br>
'src' folder contains the framework implemetation
'test' folder has example implementation of statemachine bulding.
</br>

Example :

        enum States{
            E_STATE_1 = 0,
            E_STATE_2,
            E_STATE_3,
            E_STATE_4
        };

        enum Events{
            E_EVENT_1,
            E_EVENT_2,
            E_EVENT_3,
            E_EVENT_4,
            E_EVENT_5
        };
        
        
        int handleEvent1()
        {
            std::lock_guard<std::mutex> guard(mutex);
            return 0;
        }
        int handleEvent2()
        {
            std::lock_guard<std::mutex> guard(mutex);
            return 0;
        }
        int handleEvent3()
        {
            std::lock_guard<std::mutex> guard(mutex);
            return 0;
        }

        //Building Statemachine usig fluent bulder method
        StateMachine<StateMachineImpl>& machine =
                StateMachine<StateMachineImpl>::build(this,E_STATE_1)->
            addTransition( E_STATE_1, E_STATE_2, E_EVENT_1, &StateMachineImpl::handleEvent1).
            addTransition( E_STATE_2, E_STATE_3, E_EVENT_2, &StateMachineImpl::handleEvent2).
            addTransition( E_STATE_3, E_STATE_1, E_EVENT_3, &StateMachineImpl::handleEvent3).
            getStateMachine();
       machine.sendEvent(E_EVENT_1);
       machine.sendEventAsyncBlocked(E_EVENT_2);
