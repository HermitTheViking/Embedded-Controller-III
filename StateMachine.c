// System includes
#include <stdio.h>

// Local includes
#include "StateMachine.h"
#include "wifiModule.h"

typedef struct {
    const char * name;
    void (*func)(void);
} stateFunctionRow_t;

static stateFunctionRow_t stateFunctionA[] = {
    // NAME         // FUNC
    { "ST_INIT", &wifiModule_init},
    { "ST_MODE", &SetApMode},
    { "ST_DHCP", &SetupDhcp},
    { "ST_CONN", &ConnToAp},
    { "ST_MAXCONN", &SetMaxConn},
    { "ST_TCPSERVER", &SetupServer},
    { "ST_GETIP", &Getip},
};

typedef struct {
    state_t currState;
    event_t event;
    state_t nextState;
} stateTransMatrixRow_t;

static stateTransMatrixRow_t stateTransMatrix[] = {
    // CURR STATE // EVENT // NEXT STATE
    { ST_INIT, EV_INITOK, ST_MODE},
    { ST_MODE, EV_MODEOK, ST_DHCP},
    { ST_DHCP, EV_DHCPOK, ST_CONN},
    { ST_CONN, EV_CONNOK, ST_MAXCONN},
    { ST_MAXCONN, EV_MAXCONNOK, ST_TCPSERVER},
    { ST_TCPSERVER, EV_SERVEROK, ST_GETIP},
    { ST_GETIP, EV_GETIPOK, ST_CONN}
};

void StateMachine_Init(stateMachine_t * stateMachine) {
    stateMachine->currState = ST_INIT;
}

void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event) {
    for (int i = 0; i < sizeof (stateTransMatrix) / sizeof (stateTransMatrix[0]); i++) {
        if (stateTransMatrix[i].currState == stateMachine->currState) {
            if ((stateTransMatrix[i].event == event)) { // || (stateTransMatrix[i].event == EV_ANY)
                // Transition to the next state
                stateMachine->currState = stateTransMatrix[i].nextState;
                
                // Call the function associated with transition
                (stateFunctionA[stateMachine->currState].func)();
                break;
            }
        }
    }
}