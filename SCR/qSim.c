#include "../include/qSim.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct State *S;
struct Params *P;

/* --- utility randoms --- */
double urand() { return rand() / (double)RAND_MAX; }
double urange(double a, double b) { return a + (b-a)*urand(); }

/* --- event queue --- */
struct Event* newEvent(EvType tp, double t, void *o, void (*f)(struct Event*)) {
    struct Event *e = malloc(sizeof(struct Event));
    e->type = tp; e->time = t; e->obj = o; e->act = f; e->next = NULL;
    return e;
}
void addEv(struct State *s, struct Event *e) {
    if (!s->head || e->time < s->head->time) {
        e->next = s->head; s->head = e; return;
    }
    struct Event *p = s->head;
    while (p->next && p->next->time <= e->time) p = p->next;
    e->next = p->next; p->next = e;
}
struct Event* popEv(struct State *s) {
    struct Event *e = s->head;
    if (e) s->head = e->next;
    return e;
}

/* --- queue helpers --- */
void comEnq(struct State *s, struct Customer *c) {
    c->next = NULL;
    if (!s->comh) s->comh = s->comt = c;
    else { s->comt->next = c; s->comt = c; }
    s->comlen++;
}
struct Customer* comDeq(struct State *s) {
    struct Customer *c = s->comh;
    if (!c) return NULL;
    s->comh = c->next; if (!s->comh) s->comt = NULL;
    s->comlen--; c->next = NULL;
    return c;
}
void telEnq(struct Teller *t, struct Customer *c) {
    c->next = NULL;
    if (!t->qh) t->qh = t->qt = c;
    else { t->qt->next = c; t->qt = c; }
    t->qlen++;
}
struct Customer* telDeq(struct Teller *t) {
    struct Customer *c = t->qh;
    if (!c) return NULL;
    t->qh = c->next; if (!t->qh) t->qt = NULL;
    t->qlen--; c->next = NULL;
    return c;
}
int shortestQ(struct State *s) {
    int min = s->t[0].qlen, idx = 0;
    for (int i=1;i<P->nTel;i++) if (s->t[i].qlen < min) { min=s->t[i].qlen; idx=i; }
    return idx;
}

/* --- forward declarations --- */
void actArr(struct Event *e);
void actDep(struct Event *e);
void actTel(struct Event *e);

/* --- schedule departure --- */
void schedDep(struct Customer *c, struct Teller *t, double st) {
    c->start = st;
    c->dep = st + c->serv;
    addEv(S, newEvent(DEPARTURE, c->dep, c, actDep));
    addEv(S, newEvent(TELLER, c->dep, t, actTel));
}

/* --- event actions --- */
void actArr(struct Event *e) {
    struct Customer *c = e->obj;
    double now = e->time; S->clk = now;
    if (P->commonQ) {
        comEnq(S, c);
        for (int i=0;i<P->nTel;i++) {
            if (!S->t[i].busy) {
                struct Customer *cc = comDeq(S);
                if (!cc) break;
                cc->serv = urange(0, 2*P->avgServ);
                S->t[i].busy = 1; S->t[i].servTime += cc->serv;
                schedDep(cc, &S->t[i], now);
                break;
            }
        }
    } else {
        int idx = shortestQ(S);
        telEnq(&S->t[idx], c);
        if (!S->t[idx].busy) {
            struct Customer *cc = telDeq(&S->t[idx]);
            if (cc) {
                cc->serv = urange(0, 2*P->avgServ);
                S->t[idx].busy=1; S->t[idx].servTime += cc->serv;
                schedDep(cc, &S->t[idx], now);
            }
        }
    }
}
void actDep(struct Event *e) {
    struct Customer *c = e->obj;
    double tot = c->dep - c->arr;
    S->served++; S->sum += tot; S->sumsq += tot*tot;
    double wait = c->start - c->arr;
    if (wait > S->maxWait) S->maxWait = wait;
}
void actTel(struct Event *e) {
    struct Teller *t = e->obj; double now=e->time; S->clk=now; t->busy=0;
    struct Customer *c=NULL;
    if (P->commonQ) {
        if (S->comlen>0) c = comDeq(S);
    } else {
        if (t->qlen>0) c = telDeq(t);
    }
    if (c) {
        c->serv = urange(0, 2*P->avgServ);
        t->busy=1; t->servTime+=c->serv;
        schedDep(c,t,now);
    } else {
        double idle = urange(MIN_IDLE2, MAX_IDLE2)/60.0;
        if (now+idle <= P->simT) {
            t->idleTime+=idle;
            addEv(S, newEvent(TELLER, now+idle, t, actTel));
        }
    }
}

/* --- main simulation --- */
void runSim(struct Params *pp) {
    struct State st; S=&st; P=pp;
    st.head=NULL; st.clk=0; st.served=0; st.sum=st.sumsq=st.maxWait=0;
    st.totalServ=st.totalIdle=0; st.comh=st.comt=NULL; st.comlen=0;
    st.t=calloc(P->nTel, sizeof(struct Teller));
    st.c=calloc(P->nCus, sizeof(struct Customer));

    for (int i=0;i<P->nTel;i++) {
        st.t[i].id=i;
        double idle=urange(MIN_IDLE1,MAX_IDLE1)/60.0;
        if (idle <= P->simT)
            addEv(&st,newEvent(TELLER,idle,&st.t[i],actTel));
    }
    for (int i=0;i<P->nCus;i++) {
        st.c[i].id=i;
        st.c[i].arr=urange(0,P->simT);
        addEv(&st,newEvent(ARRIVAL,st.c[i].arr,&st.c[i],actArr));
    }

    while (st.head && st.head->time <= P->simT) {
        struct Event *e=popEv(&st);
        if (e->act) e->act(e);
        free(e);
    }

    for (int i=0;i<P->nTel;i++) {
        st.totalServ+=st.t[i].servTime;
        st.totalIdle+=st.t[i].idleTime;
    }
    printf("\n-- %s queue --\n",P->commonQ?"Single":"Separate");
    double mean=st.sum/st.served;
    double var=st.sumsq/st.served - mean*mean;
    printf("Served=%d  Avg=%.2f  Std=%.2f  MaxWait=%.2f\n",st.served,mean,sqrt(var),st.maxWait);
    printf("Teller service=%.2f  idle=%.2f\n",st.totalServ,st.totalIdle);

    if (P->commonQ) {
        FILE *pf = fopen("output/plot_data.txt","a");
        if (pf) {
            fprintf(pf,"%d %.2f\n",P->nTel,mean);
            fclose(pf);
        }
    }

    free(st.t); free(st.c);
}

/* --- main entry --- */
int main(int argc,char **argv) {
    if (argc!=5) { printf("Usage: %s nCus nTel simTime avgServ\n",argv[0]); return 1; }
    struct Params p;
    p.nCus=atoi(argv[1]); p.nTel=atoi(argv[2]);
    p.simT=atof(argv[3]); p.avgServ=atof(argv[4]);
    srand(time(NULL));

    p.commonQ=1; runSim(&p);
    p.commonQ=0; runSim(&p);

    return 0;
}
