#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    ST_INIT,
    ST_MODE,
    ST_DHCP,
    ST_CONN,
    ST_MAXCONN,
    ST_TCPSERVER
} state_t;

typedef struct {
    state_t currState;
} stateMachine_t;

/// \brief      All the possible events that can occur for this state machine.
/// \details    Unlike states_t, these do not need to be kept in a special order.
typedef enum {
    EV_INITOK,
    EV_MODEOK,
    EV_DHCPOK,
    EV_MAXCONNOK,
    EV_SERVEROK,
    EV_ERROR,
    EV_GOTIP,
    EV_CONNECTED,
    EV_DISCONNECTED,
    EV_BUSY,
} event_t;

void StateMachine_Init(stateMachine_t * stateMachine);
void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event);
const char * StateMachine_GetStateName(state_t state);

#endif