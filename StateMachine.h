#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    ST_INIT,
    ST_MODE,
    ST_DHCP,
    ST_CONN,
    ST_MAXCONN,
    ST_TCPSERVER,
    ST_GETIP
} state_t;

typedef struct {
    state_t currState;
} stateMachine_t;

typedef enum {
    EV_INITOK,
    EV_MODEOK,
    EV_DHCPOK,
    EV_CONNOK,
    EV_MAXCONNOK,
    EV_SERVEROK,
    EV_GETIPOK,
    EV_ERROR,
    EV_GOTIP,
    EV_CONNECTED,
    EV_DISCONNECTED,
    EV_BUSY,
} event_t;

void StateMachine_Init(stateMachine_t * stateMachine);
void StateMachine_RunIteration(stateMachine_t *stateMachine, event_t event);

#endif