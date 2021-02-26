// System includes
#include <stdio.h>

// Local includes
#include "StateMachine.h"
#include "wifiModule.h"

typedef struct {
    const char * name;
    void (*func)(void);
} stateFunctionRow_t;

/// \brief  Maps a state to it's state transition function, which should be called
///         when the state transitions into this state.
/// \warning    This has to stay in sync with the state_t enum!
static stateFunctionRow_t stateFunctionA[] = {
    // NAME         // FUNC
    { "ST_INIT", &wifiModule_init},
    { "ST_MODE", &wifiModule_mode},
    { "ST_DHCP", &wifiModule_dhcp},
    { "ST_CONN", &wifiModule_conn},
    { "ST_MAXCONN", &wifiModule_maxconn},
    { "ST_TCPSERVER", &wifiModule_server},
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
    { ST_CONN, EV_GOTIP, ST_MAXCONN},
    { ST_MAXCONN, EV_MAXCONNOK, ST_TCPSERVER},
    { ST_TCPSERVER, EV_SERVEROK, ST_MODE}
};

void StateMachine_Init(stateMachine_t * stateMachine) {
    //    printf("Initialising state machine.\r\n");
    stateMachine->currState = ST_INIT;
}

void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event) {
    // Iterate through the state transition matrix, checking if there is both a match with the current state
    // and the event
    for (int i = 0; i < sizeof (stateTransMatrix) / sizeof (stateTransMatrix[0]); i++) {
        if (stateTransMatrix[i].currState == stateMachine->currState) {
            if ((stateTransMatrix[i].event == event)) { // || (stateTransMatrix[i].event == EV_ANY)
                // Call the function associated with transition
                (stateFunctionA[stateMachine->currState].func)();

                // Transition to the next state
                stateMachine->currState = stateTransMatrix[i].nextState;
                break;
            }
        }
    }
}

const char * StateMachine_GetStateName(state_t state) {
    return stateFunctionA[state].name;
}