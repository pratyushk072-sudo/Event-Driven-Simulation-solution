#ifndef QSIM_H
#define QSIM_H

#define MIN_IDLE1 1
#define MAX_IDLE1 600
#define MIN_IDLE2 1
#define MAX_IDLE2 150



typedef enum { ARRIVAL, DEPARTURE, TELLER } EvType;

typedef struct Customer {
    int id;
    double arr, start, serv, dep;
    struct Customer *next;
} Customer;

typedef struct Teller {
    int id, busy;
    double servTime, idleTime, freeAt;
    Customer *qh, *qt;
    int qlen;
} Teller;

typedef struct Event {
    EvType type;
    double time;
    void *obj;
    void (*act)(struct Event*);
    struct Event *next;
} Event;

typedef struct State {
    double clk;
    Event *head;
    Teller *t;
    Customer *c;
    Customer *comh, *comt;
    int comlen;
    int served;
    double sum, sumsq, maxWait;
    double totalServ, totalIdle;
} State;

typedef struct Params {
    int nCus, nTel;
    double simT, avgServ;
    int commonQ;
} Params;

/* functions */
void runSim(struct Params *pp);

#endif
