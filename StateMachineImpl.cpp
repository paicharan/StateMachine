#include<StateMachine.cpp>
#include<chrono>

class StateMachineImpl 
{
    std::mutex mutex;
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
            std::lock_guard<std::mutex> guard(mutex);
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }
        int handleEvent2()
        {
            std::lock_guard<std::mutex> guard(mutex);
            std::cout<<__FUNCTION__<<std::endl;
            return 0;
        }
        int handleEvent3()
        {
            std::lock_guard<std::mutex> guard(mutex);
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
            machine.sendEventAsyncBlocked(E_EVENT_2);
            machine.sendEventAsyncUnblocked(E_EVENT_3);
            machine.sendEventAsyncBlocked(E_EVENT_1);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            machine.sendEvent(E_EVENT_2);
            machine.sendEventAsyncUnblocked(E_EVENT_3);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

};

