#ifndef RUN_H_
#define RUN_H_

typedef enum { R_DEBUG, R_RUN } RunState;
extern RunState run_state;

void run_init();
void run_step();

#endif