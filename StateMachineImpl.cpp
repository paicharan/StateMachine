#include<StateMachine.cpp>

class StateMachineImpl 
{
    public:
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
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }
        int handleEvent2()
        {
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }
        int handleEvent3()
        {
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }

        StateMachineImpl()
        {
            std::cout<<__FUNCTION__<<std::endl;
            StateMachine<StateMachineImpl>& machine =
                StateMachine<StateMachineImpl>::build(this,E_STATE_1)->
            addTransition( E_STATE_1, E_STATE_2, E_EVENT_1, &StateMachineImpl::handleEvent1).
            addTransition( E_STATE_2, E_STATE_3, E_EVENT_2, &StateMachineImpl::handleEvent2).
            addTransition( E_STATE_3, E_STATE_1, E_EVENT_3, &StateMachineImpl::handleEvent3).
            getStateMachine();
            machine.sendEvent(E_EVENT_1);
            machine.sendEvent(E_EVENT_2);
            machine.sendEvent(E_EVENT_3);
            machine.sendEvent(E_EVENT_1);
            machine.sendEvent(E_EVENT_2);
            machine.sendEvent(E_EVENT_3);
        }

};

